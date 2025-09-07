#include "camera.hpp"
#include "dielectric.hpp"
#include "hitrecord.hpp"
#include "lambertian.hpp"
#include "light.hpp"
#include "material.hpp"
#include "progress_bar.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <ostream>
#include <semaphore>
#include <string>
#include <sys/ioctl.h>
#include <thread>
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
        this->max_depth = settings.max_depth;
        this->use_light_sampling = settings.use_light_sampling;
        this->use_scene_sig = settings.use_scene_sig;
        this->background_texture = settings.background_texture;
        this->use_importance_sampling = settings.use_importance_sampling;

        this->lookat = Vec3 (0, 1, -1);
        this->center = Vec3 (0, 1, 1); // (1, 1, 4)
        this->focus_dist = 1;

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

void Camera::print_arguments ()
{
        log_info ("Image Size (w x h):      %d x %d", this->image_width, this->image_height);
        log_info ("Samples Per Pixel:       %d", this->samples_per_pixel);
        log_info ("Vertical Field of View:  %lf degrees", this->vfov);
        log_info ("Ray Bounce Max Depth:    %d", this->max_depth);
        log_info ("Defocus Angle:           %lf degrees", this->defocus_angle);
        log_info ("Path Tracing:            %s", this->use_path_tracer ? "Enabled" : "Disabled");
        log_info ("Importance Sampling:     %s", this->use_importance_sampling ? "Enabled" : "Disabled");
        log_info ("Explicit Light Sampling: %s", this->use_light_sampling ? "Enabled" : "Disabled");
        // log_info ("Compiler Optimization Level: %d", __OPTIMIZE__);
}

Vec3 Camera::defocus_disk_sample ()
{
        Vec3 p = random_in_unit_disk ();

        return this->center + this->defocus_disk_u * p[0] + this->defocus_disk_v * p[1];
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

        if (!world->hit (r, record)) {
                Vec3 sph = r.direction.unit ().sph ();

                Vec3 uv (sph[1] / (2 * M_PI), sph[2] / M_PI, 0);

                return this->background_texture->read_texture_uv (uv, uv).clamp (0, 1);
        }

        if (depth == 0)
                return record.object->material->texture->read_texture_uv (record.uv, record.hit_point);

        Material::PhongParams params = record.object->material->phong (r, record);

        Vec3 color = Vec3 (1, 1, 1) * params.ra * params.color;

        for (Light *light_source : world->lights)
                color += this->sample_light_rays (world, record, light_source, params, this->arealight_samples);

        // color += world->photon_map_color (record.hit_point) * 1.5;
        color *= params.gamma;

        Vec3 normal = record.normal;

        if (params.gamma < 1) {
                // mu is the refractive index:
                // air has a refractive index of 1.
                double mu = record.front_face ? 1 / params.mu : params.mu;

                if (r.can_refract (normal, mu)) {
                        double refl = Dielectric::reflectance ((-(r.direction.unit ())).dot (normal), mu);
                        if (random_double (0, 1) < refl) {
                                Ray reflected_ray (record.hit_point, r.direction.reflect (normal).unit ());
                                reflected_ray.nudge_forward();
                                color += this->ray_color (reflected_ray, world, depth - 1) * params.rg;
                        } else {
                                Ray refraction (record.hit_point, r.direction.unit ().refract (normal, mu));
                                refraction.nudge_forward ();
                                color += this->ray_color (refraction, world, depth - 1) * (1 - params.gamma);
                        }
                } else {
                        Ray reflected_ray (record.hit_point, r.direction.reflect (normal).unit ());
                        color += this->ray_color (reflected_ray, world, depth - 1) * params.rg;
                }
        } else {
                Ray specular_reflection (record.hit_point, r.direction.unit ().reflect (normal).unit ());

                specular_reflection.nudge_forward ();

                color += this->ray_color (specular_reflection, world, depth - 1) * params.rg;
        }
        return color.clamp (0, 1);
}

void Camera::export_p6 (const char *filename, std::vector<Vec3> pixels)
{
        FILE *fp = fopen (filename, "wb");

        if (!fp) {
                log_error ("Failed to write image file %s: %s", filename, strerror (errno));
                exit (EXIT_FAILURE);
        }

        size_t pixel_data_size = pixels.size () * 3 * sizeof (uint8_t);
        uint8_t *pixel_data = (uint8_t *)malloc (pixel_data_size);
        size_t j = 0;

        for (Vec3 &p : pixels)
                for (int i = 0; i < 3; i++)
                        pixel_data[j++] = uint8_t (clamp (0, sqrt (p[i]), 0.999) * 256);

        fprintf (fp,
                 "P6\n"
                 "%d %d\n"
                 "%d\n",
                 this->image_width,
                 this->image_height,
                 255);
        fwrite (pixel_data, pixel_data_size, 1, fp);
        fclose (fp);
        free (pixel_data);
}

Vec3 Camera::sample_light (World *world, HitRecord &record, SmoothObject *&hit_light)
{
        /**
                Light sampling only applies to diffuse materials.
                Currently, Lambertian is the only diffuse material, this may
                need to change in the future.
         */
        if (!dynamic_cast<Lambertian *> (record.object->material))
                return Vec3 (0, 0, 0);

        /**
                Pick a light at random, if there are no lights, then it isn't
                possible to perform light sampling.
         */
        SmoothObject *light = world->random_light ();

        if (!light)
                return Vec3::zero ();

        /**
                Randomly select a point on the light surface and check if
                there is a clear path between the hit point and the randomly
                selected point. If light ray is blocked, then there is no
                contribution.
         */
        Vec3 light_point = light->sample_point ();

        if (!world->has_path (record.hit_point, light_point))
                return Vec3::zero ();

        hit_light = light;

        /**
                Compute the probability that a randomly emitted ray will hit the
                light source.

                This is done with the unit hemisphere oriented in the surface
                normal direction at the hit point.

                The area of the light multiplied by the foreshortening factor
                determines how much area of the sphere, with radius the distance
                from the light source to the hit point, the light covers from
                the perspective of the hit point.

                The area is divided the sphere's radius squared, to compute the
                solid angle of the light.

                Then finally divided by 2 PI (the number of steradians in a
                hemisphere)

                LIGHT_AREA * dot(L, N)

         */
        Vec3 to_light = light_point - record.hit_point;
        double light_area = light->area ();
        double light_distance = to_light.length ();
        double projected_area_of_light = -to_light.unit ().dot (light->normal (light_point).unit ()) * light_area;

        double steradians_on_unit_hemisphere = projected_area_of_light / (light_distance * light_distance);

        double ray_prob = steradians_on_unit_hemisphere / (2 * M_PI);

        return light->material->emission () * fmax (0, ray_prob * to_light.unit ().dot (record.normal)) *
               record.object->material->color (record);
}

Vec3 Camera::single_path_color (Ray starting_ray, World *world, int depth)
{
        std::vector<Vec3> radiances;
        std::vector<Vec3> throughput;

        for (int i = 0; i < depth; i++) {
                HitRecord record;

                if (!world->hit (starting_ray, record)) {
                        Vec3 sph = starting_ray.direction.unit ().sph ();

                        Vec3 uv (sph[1] / (2 * M_PI), sph[2] / M_PI, 0);

                        radiances.push_back (this->background_texture->read_texture_uv (uv, uv));
                        throughput.push_back (Vec3 (0, 0, 0));

                        break;
                }

                double pdf;
                Vec3 brdf;
                Vec3 scatter_dir = record.object->material->scatter (starting_ray, record, brdf, pdf);
                double lambert_cos = scatter_dir.unit ().dot (record.normal.unit ());

                Vec3 radiance = record.object->material->emission ();

                throughput.push_back (brdf * lambert_cos / pdf);

                starting_ray = Ray (record.hit_point, scatter_dir);

                starting_ray.nudge_forward ();

                if (this->use_light_sampling) {
                        SmoothObject *light;

                        Vec3 light_sample = this->sample_light (world, record, light);

                        if (light_sample != Vec3::zero ()) {
                                HitRecord next_record;

                                if (!world->hit (starting_ray, next_record)) {
                                        radiance += light_sample;
                                } else if (next_record.object != light) {
                                        radiance += light_sample;
                                }
                        }
                }

                radiances.emplace_back (radiance);

                if (record.object->material->is_emissive ())
                        break;
        }
        Vec3 total_radiance (0, 0, 0);

        std::vector<Vec3> throughput_partial_products{ Vec3 (1, 1, 1) };

        Vec3 throughput_product (1, 1, 1);

        for (size_t j = 0; j < radiances.size (); j++) {
                throughput_product = throughput_product * throughput[j];
                throughput_partial_products.push_back (throughput_product);
        }

        for (size_t i = 0; i < radiances.size (); i++) {
                total_radiance += radiances[i] * throughput_partial_products[i];
        }

        return total_radiance;
}

Vec3 Camera::scene_signature_color (Ray starting_ray, World *world)
{
        HitRecord record;
        if (!world->hit (starting_ray, record))
                return Vec3 (0, 0, 0);

        return (record.normal.unit () + Vec3 (1, 1, 1)) / 2;
}

Vec3 Camera::sample_pixel (World *world, int i, int j)
{
        Vec3 pixel_color (0, 0, 0);
        for (int sample = 0; sample < this->samples_per_pixel; sample++) {
                Ray r = this->ray (i + random_double (-0.5, 0.5), j + random_double (-0.5, 0.5));

                if (this->use_path_tracer)
                        pixel_color += this->single_path_color (r, world, this->max_depth);
                else if (this->use_scene_sig)
                        pixel_color += this->scene_signature_color (r, world);
                else
                        pixel_color += this->ray_color (r, world, this->max_depth);
        }

        pixel_color /= double (this->samples_per_pixel);

        return pixel_color;
}

void Camera::render_multithreaded (World *world, const char *filename, int max_threads)
{
        std::counting_semaphore<> sem (max_threads);
        std::counting_semaphore<> progress (0);
        std::vector<std::thread> threads;
        std::vector<Vec3> pixels (this->image_width * this->image_height);

#ifndef __OPTIMIZE__
        log_warn ("You are running a DEBUG build. Rendering may be slower than expected.");
#endif

        log_info ("Rendering on %d threads with the following arguments:", max_threads);
        this->print_arguments ();

        // #ifdef THOROTTLED_PARALLEL
        for (int j = 0; j < this->image_height; j++) {
                threads.push_back (std::thread ([&, j] {
                        sem.acquire ();

                        for (int i = 0; i < this->image_width; i++)
                                pixels[i + j * image_width] = this->sample_pixel (world, i, j);

                        sem.release ();
                        progress.release ();
                }));
        }
        // #else
        //         for (int tid = 0; tid < max_threads; tid++) {
        //                 threads.push_back (std::thread ([&, tid] {
        //                         for (int j = 0; j < this->image_height; j++) {
        //                                 sem.acquire ();

        //                                 for (int i = 0; i < this->image_width; i++)
        //                                         pixels[i + j * image_width] = this->sample_pixel (world, i, j);

        //                                 sem.release ();
        //                                 progress.release ();
        //                         }
        //                 }));
        //         }
        // #endif

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

        this->export_p6 (filename, pixels);

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

        std::vector<Vec3> pixels;

        for (int j = 0; j < this->image_height; j++) {
                bar.update ();

                for (int i = 0; i < this->image_width; i++)
                        pixels.push_back (this->sample_pixel (world, i, j));
        }

        this->export_p6 (filename, pixels);
}
