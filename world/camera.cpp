#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "progress_bar.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ostream>
#include <semaphore>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

Camera::Camera () : pixel_du (0, 0, 0), pixel_dv (0, 0, 0), pixel_00 (0, 0, 0), center (0, 0, 0), stream (std::cout)
{
}

void Camera::initialize (double aspect_ratio, int image_width, double vfov, double defocus_angle, int arealight_samples)
{
        this->aspect_ratio = aspect_ratio;
        this->image_width = image_width;
        this->lookat = Vec3 (0, 0, 3);
        this->center = Vec3 (0, 0, 4);
        this->defocus_angle = defocus_angle;
        this->focus_dist = 1;
        Vec3 w = (this->center - this->lookat).unit ();
        Vec3 vup (0, 1, 0);
        Vec3 u = vup.cross (w).unit ();
        Vec3 v = w.cross (u);

        this->vfov = vfov;
        this->max_depth = 80;
        this->samples_per_pixel = 150;
        this->image_height = int (this->image_width / this->aspect_ratio);
        this->viewport_height = 2 * this->focus_dist * std::tan (deg2rad (this->vfov / 2));
        this->viewport_width = this->viewport_height * (double (this->image_width) / this->image_height);

        Vec3 viewport_u = u * this->viewport_width;
        Vec3 viewport_v = -v * this->viewport_height;
        Vec3 viewport_top_left = this->center - w * focus_dist - viewport_u / 2 - viewport_v / 2;

        this->pixel_du = viewport_u / this->image_width;
        this->pixel_dv = viewport_v / this->image_height;
        this->pixel_00 = viewport_top_left + ((pixel_du + pixel_dv) / 2.0);

        double defocus_radius = this->focus_dist * std::tan (deg2rad (this->defocus_angle / 2));

        this->defocus_disk_u = u * defocus_radius;
        this->defocus_disk_v = v * defocus_radius;
        this->arealight_samples = arealight_samples;
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

Vec3 Camera::sample_light_rays (World *world, HitRecord &record, Light *light, Material::PhongParams params, int K)
{
        Vec3 to_camera = (this->center - record.hit_point).unit ();
        Vec3 diffuse_component (0, 0, 0), specular_component (0, 0, 0);

        if (light->is_point_light ()) {
                Vec3 point = light->sample_point ();
                Vec3 light_direction = (point - record.hit_point).unit ();
                Vec3 mirror_direction = (-light_direction).reflect (record.normal);

                if (world->has_path (record.hit_point, point)) {
                        diffuse_component =
                                light->diffuse_intensity (point) * std::max (0.0, light_direction.dot (record.normal));
                        specular_component = light->specular_intensity (point) *
                                             std::pow (std::max (0.0, mirror_direction.dot (to_camera)), params.alpha);
                }
        } else {
                for (int i = 0; i < K; i++) {
                        Vec3 point = light->sample_point ();
                        Vec3 light_direction = (point - record.hit_point).unit ();
                        Ray to_light = Ray (record.hit_point, light_direction);

                        if (world->has_path (record.hit_point, point)) {
                                diffuse_component += light->diffuse_intensity (point) *
                                                     std::max (0.0, light_direction.dot (record.outward_normal ())) / K;
                                Vec3 mirror_direction = (-light_direction).reflect (record.normal);

                                specular_component +=
                                        light->specular_intensity (point) *
                                        std::pow (std::max (0.0, mirror_direction.dot (to_camera)), params.alpha) / K;
                        }
                }
        }

        diffuse_component *= params.rd;
        specular_component *= params.rs;

        return diffuse_component + specular_component;
}

Vec3 Camera::ray_color (Ray r, World *world, int depth)
{
        if (depth == 0)
                return Vec3 (0, 0, 0);

        HitRecord record;

        if (!world->hit (r, record))
                return Vec3 (0, 0, 0);

        Material::PhongParams params = record.mat->phong (r, record);

        Vec3 radiance = Vec3 (1, 1, 1) * params.ra;

        for (Light *light_source : world->lights)
                radiance += this->sample_light_rays (world, record, light_source, params, this->arealight_samples);

        Vec3 color = radiance.elem_mul (params.color);

        Vec3 normal = record.normal;
        Ray scatter (record.hit_point, r.direction.reflect (normal).unit ());

        color += this->ray_color (scatter, world, depth - 1) * params.rg;

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

void Camera::render_multithreaded (World *world, const char *filename, int max_threads)
{
        std::ofstream file_stream = std::ofstream (std::string (filename));
        this->stream.rdbuf (file_stream.rdbuf ());

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

                                        Vec3 color = this->ray_color (r, world, this->max_depth);
                                        pixel_color += color;
                                }

                                pixel_color *= 1.0 / double (this->samples_per_pixel);

                                pixel_lock.lock ();
                                pixels[i + j * image_width] = pixel_color;
                                pixel_lock.unlock ();
                        }
                        sem.release ();
                        progress.release ();
                }));
        }
        progressbar bar (image_height);

        for (int p = 0; p < image_height; p++) {
                progress.acquire ();
                bar.update ();
        }

        for (std::thread &t : threads)
                t.join ();

        for (Vec3 &pixel_color : pixels)
                this->write_color (pixel_color);

        file_stream.close ();
}

void Camera::render (World *world, const char *filename)
{
        std::ofstream file_stream = std::ofstream (std::string (filename));
        this->stream.rdbuf (file_stream.rdbuf ());

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
                                pixel_color += this->ray_color (r, world, this->max_depth);
                        }
                        pixel_color *= 1.0 / double (this->samples_per_pixel);
                        this->write_color (pixel_color);
                }
        }

        file_stream.close ();
}
