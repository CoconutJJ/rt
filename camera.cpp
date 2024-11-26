#include "camera.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <cmath>
#include <cstdio>
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

void Camera::initialize (double aspect_ratio, int image_width, double vfov)
{
        this->aspect_ratio = aspect_ratio;
        this->image_width = image_width;
        this->lookat = Vec3 (0, 0, -1);
        this->center = Vec3 (0, 0, 0);
        this->defocus_angle = 0.0;
        this->focus_dist = 3.4;
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

Vec3 Camera::ray_color (Ray r, World *world, int depth)
{
        if (depth == 0)
                return Vec3 (0, 0, 0);

        HitRecord record;
        if (world->hit (r, record)) {
                Vec3 color;
                Ray scatter;
                if (record.mat->scatter (r, record, color, scatter)) {
                        return color + this->ray_color (scatter, world, depth - 1) * 0.01;
                }

                return Vec3 (0, 0, 0);
        }

        return Vec3 (0, 0, 0);
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

void progress (int progress, int total)
{
        struct winsize size;
        ioctl (STDOUT_FILENO, TIOCGWINSZ, &size);

        double percentage = (double)progress / total;

        int current = int (percentage * (size.ws_col - 10));
        std::cerr << '|';
        for (int i = 0; i < size.ws_col - 10; i++) {
                if (i <= current)
                        std::cerr << '#';
                else
                        std::cerr << ' ';
        }
        std::cerr << "| ";
        std::fprintf (stderr, "%.2lf%%", percentage * 100);
        std::cerr << '\r';
        std::cerr.flush ();
}

void Camera::render_multithreaded (World *world, const char *filename, int max_threads)
{
        std::ofstream file_stream = std::ofstream (std::string (filename));
        this->stream.rdbuf (file_stream.rdbuf ());

        this->stream << "P3\n"
                     << this->image_width << ' ' << this->image_height << "\n"
                     << "255"
                     << "\n";

        int p = 0;

        std::mutex pixel_lock;
        std::counting_semaphore<> sem (max_threads);
        std::vector<std::thread> threads;
        std::vector<Vec3> pixels (this->image_width * this->image_height);

        for (int j = 0; j < this->image_height; j++) {
                threads.push_back (std::thread ([&, j] {
                        sem.acquire ();
                        for (int i = 0; i < this->image_width; i++) {
                                Vec3 pixel_color (0, 0, 0);
                                for (int sample = 0; sample < this->samples_per_pixel; sample++) {
                                        // std::cerr << "Thread " << i << ", " << j << ", " << sample << " start\n";
                                        Ray r = this->ray (i + random_double (-0.5, 0.5),
                                                           j + random_double (-0.5, 0.5));

                                        Vec3 color = this->ray_color (r, world, this->max_depth);
                                        pixel_color += color;
                                        // std::cerr << "Thread " << i << ", " << j << ", " << sample << " finish\n";
                                }

                                pixel_color *= 1.0 / double (this->samples_per_pixel);

                                pixel_lock.lock ();
                                pixels[i + j * image_width] = pixel_color;
                                pixel_lock.unlock ();
                        }
                        sem.release ();
                }));
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

        int p = 0;
        for (int j = 0; j < this->image_height; j++) {
                progress (p++, image_height);

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
