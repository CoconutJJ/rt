#include "camera.hpp"
#include "hitrecord.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object.hpp"
#include "progress_bar.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ostream>
#include <semaphore>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <vector>

Camera::Camera () : stream (std::cout), pixel_du (0, 0, 0), pixel_dv (0, 0, 0), pixel_00 (0, 0, 0), center (0, 0, 0)
{
}

void Camera::initialize (struct RendererSettings settings)
{
        this->aspect_ratio = settings.aspect_ratio;
        this->image_width = settings.image_width;
        this->defocus_angle = settings.defocus_angle;
        this->samples_per_pixel = settings.samples_per_pixel;
        this->vfov = settings.vfov;
        this->use_path_tracer = settings.use_path_tracer;
        this->arealight_samples = settings.arealight_samples;

        this->lookat = Vec3 (0, 1, -1);
        this->center = Vec3 (0, 1.3, 2); // (1, 1, 4)
        this->focus_dist = 1;

        this->max_depth = 5;
        this->image_height = int (this->image_width / this->aspect_ratio);
        this->viewport_height = 2 * this->focus_dist * std::tan (deg2rad (this->vfov / 2));
        this->viewport_width = this->viewport_height * (double (this->image_width) / this->image_height);

        Vec3 w = (this->center - this->lookat).unit ();
        Vec3 vup (0, 1, 0);
        Vec3 u = vup.cross (w).unit ();
        Vec3 v = w.cross (u);
        Vec3 viewport_u = u * this->viewport_width;
        Vec3 viewport_v = -v * this->viewport_height;
        Vec3 viewport_top_left = this->center - w * focus_dist - viewport_u / 2 - viewport_v / 2;

        this->pixel_du = viewport_u / this->image_width;
        this->pixel_dv = viewport_v / this->image_height;
        this->pixel_00 = viewport_top_left + ((pixel_du + pixel_dv) / 2.0);

        double defocus_radius = this->focus_dist * std::tan (deg2rad (this->defocus_angle / 2));

        this->defocus_disk_u = u * defocus_radius;
        this->defocus_disk_v = v * defocus_radius;
}

Vec3 Camera::defocus_disk_sample ()
{
        Vec3 p = random_in_unit_disk ();

        return this->center + this->defocus_disk_u * p.x + this->defocus_disk_v * p.y;
}

Ray Camera::ray (int du, int dv)
{
        Vec3 origin = this->defocus_angle <= 0.0 ? this->center : this->defocus_disk_sample ();
        Vec3 direction = pixel_00 + (pixel_du * du) + (pixel_dv * dv) - origin;

        double time = random_double (0, 1);

        return Ray (origin, direction, time);
}

/**
        For the ray tracer, we need to shoot a ray to each light source. This
        method returns the diffuse and specular lighting components for the
        Blinn-Phong lighting model.

 */
Vec3 Camera::sample_light_rays (World *world,
                                HitRecord &record,
                                Light *light,
                                Material::PhongParams params,
                                int light_num_samples)
{
        Vec3 to_camera = (this->center - record.hit_point).unit ();
        Vec3 diffuse_component (0, 0, 0), specular_component (0, 0, 0);

        /**
                Arealights and Point-source lights needed to be treated differently: area lights require us to sample
                multiple points (K) where as point source lights only have one point.
        */
        if (light->is_point_light ())
                light_num_samples = 1;

        /**
                We use the Blinn-Phong Lighting Model for the ray tracer.
         */

        for (int i = 0; i < light_num_samples; i++) {
                Vec3 point = light->sample_point ();

                /**
                        Path from hit point to light source must be clear,
                        otherwise we omit the diffuse and specular components,
                        creating a shadow.
                 */

                if (!world->has_path (record.hit_point, point))
                        continue;

                Vec3 light_direction = (point - record.hit_point).unit ();
                Ray to_light = Ray (record.hit_point, light_direction);

                diffuse_component += light->diffuse_intensity (point) *
                                     std::max (0.0, light_direction.dot (record.normal)) / light_num_samples;

                // Blinn-Phong Lighting: halfway vector
                Vec3 halfway = (to_camera + light_direction).unit ();

                specular_component = light->specular_intensity (point) *
                                     std::pow (std::max (0.0, halfway.dot (record.normal)), params.alpha) /
                                     light_num_samples;
        }

        diffuse_component *= params.rd;
        specular_component *= params.rs;
        return diffuse_component * params.color + specular_component;
}

Vec3 Camera::ray_color (Ray r, World *world, int depth)
{
        HitRecord record;

        if (!world->hit (r, record))
                return Vec3 (0, 0, 0);

        if (depth == 0)
                return record.object->material->texture->read_texture_uv (record.uv, record.hit_point);

        Material::PhongParams params = record.object->material->phong (r, record);

        Vec3 color = Vec3 (1, 1, 1) * params.ra * params.color;

        for (Light *light_source : world->lights)
                color += this->sample_light_rays (world, record, light_source, params, this->arealight_samples);

        color += world->photon_map_color (record.hit_point) * 1.5;
        color *= params.gamma;

        Vec3 normal = record.normal;

        if (params.gamma < 1) {
                // mu is the refractive index:
                // air has a refractive index of 1.
                double mu = record.front_face ? params.mu : 1 / params.mu;

                if (r.can_refract (normal, mu)) {
                        Ray refraction (record.hit_point, r.direction.unit ().refract (normal, mu));
                        color += this->ray_color (refraction, world, depth - 1) * (1 - params.gamma);
                }
        }

        Ray specular_reflection (record.hit_point, r.direction.unit ().reflect (normal).unit ());
        color += this->ray_color (specular_reflection, world, depth - 1) * params.rg;

        return color.clamp (0, 1);
}

void Camera::write_color (Vec3 color)
{
        color.x = std::sqrt (color.x);
        color.y = std::sqrt (color.y);
        color.z = std::sqrt (color.z);
        color = color.clamp (0, 0.999);
        color *= 256;

        this->stream << int (color.x) << ' ' << int (color.y) << ' ' << int (color.z) << '\n';
}

Vec3 Camera::sample_light (World *world, HitRecord &record, SmoothObject *&hit_light)
{
        SmoothObject *light = world->random_light ();

        Vec3 light_point = light->sample_point ();

        if (!world->has_path (record.hit_point, light_point))
                return Vec3::zero ();

        hit_light = light;

        Vec3 to_light = light_point - record.hit_point;

        double light_area = light->area ();

        double light_distance = to_light.length ();

        double foreshortening_factor = -to_light.unit ().dot (light->normal (light_point).unit ());

        double ray_prob = fmax (0,
                                (light_area * foreshortening_factor * to_light.unit ().dot (record.normal)) /
                                        (light_distance * light_distance * 2 * M_PI));

        return light->material->emission () * ray_prob * record.object->material->color (record);
}

Vec3 Camera::single_path_color (Ray starting_ray, World *world, int depth)
{
        std::vector<Vec3> radiances;
        std::vector<Vec3> throughput;

        for (int i = 0; i < depth; i++) {
                HitRecord record;

                if (!world->hit (starting_ray, record))
                        break;

                Vec3 brdf;
                double pdf;
                Vec3 scatter_dir = record.object->material->scatter (starting_ray, record, brdf, pdf);
                double foreshortening_factor = -starting_ray.direction.unit ().dot (record.normal.unit ());

                Vec3 radiance = record.object->material->emission ();

                throughput.push_back (brdf * foreshortening_factor * record.object->material->color (record) / pdf);

                starting_ray = Ray (record.hit_point, scatter_dir);

                SmoothObject *light;

                Vec3 light_sample = this->sample_light (world, record, light);

                if (light_sample != Vec3::zero ()) {
                        HitRecord next_record;

                        if (world->hit (starting_ray, next_record)) {
                                if (next_record.object != light) {
                                        radiance += light_sample;
                                }
                        } else {
                                radiance += light_sample;
                        }
                }

                radiances.emplace_back (radiance);

                if (record.object->material->is_emissive ())
                        break;
        }
        Vec3 total_radiance (0, 0, 0);

        std::vector<Vec3> throughput_partial_products{ Vec3 (1, 1, 1) };

        Vec3 throughput_product (1, 1, 1);

        for (int j = 0; j < radiances.size (); j++) {
                throughput_product = throughput_product * throughput[j];
                throughput_partial_products.push_back (throughput_product);
        }

        for (int i = 0; i < radiances.size (); i++) {
                total_radiance += radiances[i] * throughput_partial_products[i];
        }

        return total_radiance;
}

Vec3 Camera::path_color (Ray r, World *world, int depth)
{
        if (depth == 0)
                return Vec3 (0, 0, 0);

        HitRecord record;

        if (!world->hit (r, record))
                return Vec3 (0, 0, 0);

        Vec3 in_direction = r.direction;

        Vec3 brdf;
        double ray_prob = 1;
        Vec3 direction = record.object->material->scatter (r, record, brdf, ray_prob);
        Ray scatter_ray (record.hit_point + direction.unit() * 0.001, direction);

        size_t light_source_idx = rand () % world->emissives.size ();

        Object *light_source = world->emissives[light_source_idx];

        Vec3 scatter_color = this->path_color (scatter_ray, world, depth - 1);

        return record.object->material->emission () + scatter_color * brdf * record.normal.dot (-in_direction.unit ()) *
                                                              record.object->material->color (record) / ray_prob;
}

void Camera::set_output_file (const char *filename)
{
        std::ofstream file_stream = std::ofstream (std::string (filename));

        this->stream.rdbuf (file_stream.rdbuf ());

        this->stream << "P3\n"
                     << this->image_width << ' ' << this->image_height << "\n"
                     << "255"
                     << "\n";
}

void Camera::render_multithreaded (World *world, const char *filename, int max_threads)
{
        std::ofstream *file_stream = new std::ofstream (std::string (filename));
        this->stream.rdbuf (file_stream->rdbuf ());

        this->stream << "P3\n"
                     << this->image_width << ' ' << this->image_height << "\n"
                     << "255"
                     << "\n";
        std::mutex pixel_lock;
        std::counting_semaphore<> sem (max_threads);
        std::counting_semaphore<> progress (0);
        std::vector<std::thread> threads;
        std::vector<Vec3> pixels (this->image_width * this->image_height);

        for (int j = 0; j < this->image_height; j++) {
                threads.push_back (std::thread ([&, j] {
                        sem.acquire ();
                        for (int i = 0; i < this->image_width; i++) {
                                Vec3 pixel_color (0, 0, 0);
                                for (int sample = 0; sample < this->samples_per_pixel; sample++) {
                                        Ray r = this->ray (i + random_double (-0.5, 0.5),
                                                           j + random_double (-0.5, 0.5));

                                        pixel_color += this->use_path_tracer ?
                                                               this->single_path_color (r, world, this->max_depth) :
                                                               this->ray_color (r, world, this->max_depth);
                                }

                                pixel_color /= double (this->samples_per_pixel);

                                pixel_lock.lock ();
                                pixels[i + j * image_width] = pixel_color;
                                pixel_lock.unlock ();
                        }
                        sem.release ();
                        progress.release ();
                }));
        }
        progressbar bar (image_height);

        auto start_time = std::chrono::high_resolution_clock::now ();

        for (int p = 0; p < image_height; p++) {
                progress.acquire ();
                bar.update ();
        }
        std::cerr << "\n";

        for (std::thread &t : threads)
                t.join ();

        auto end_time = std::chrono::high_resolution_clock::now ();

        int time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end_time - start_time).count ();

        for (Vec3 &pixel_color : pixels)
                this->write_color (pixel_color);

        std::cerr << "Render took " << time_elapsed / 1000.0 << " secs.";
}

void Camera::render (World *world, const char *filename)
{
        std::ofstream *file_stream = new std::ofstream (std::string (filename));
        this->stream.rdbuf (file_stream->rdbuf ());

        this->stream << "P3\n"
                     << this->image_width << ' ' << this->image_height << "\n"
                     << "255"
                     << "\n";
        progressbar bar (image_height);

        for (int j = 0; j < this->image_height; j++) {
                bar.update ();

                for (int i = 0; i < this->image_width; i++) {
                        Vec3 pixel_color (0, 0, 0);
                        for (int sample = 0; sample < this->samples_per_pixel; sample++) {
                                Ray r = this->ray (i + random_double (-0.5, 0.5), j + random_double (-0.5, 0.5));
                                pixel_color += this->use_path_tracer ? this->path_color (r, world, this->max_depth) :
                                                                       this->ray_color (r, world, this->max_depth);
                        }
                        pixel_color /= double (this->samples_per_pixel);
                        this->write_color (pixel_color);
                }
        }
}
