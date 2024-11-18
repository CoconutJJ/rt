#include "camera.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <cmath>
#include <iostream>

Camera::Camera (double aspect_ratio, int image_width)
        : pixel_du (0, 0, 0), pixel_dv (0, 0, 0), pixel_00 (0, 0, 0), center (0, 0, 0)
{
        this->aspect_ratio = aspect_ratio;
        this->image_width = image_width;
        this->image_height = int (this->image_width / this->aspect_ratio);
        this->center = Vec3 (0, 0, 0);
        this->focal_length = 1.0;

        this->viewport_width = 2.0;
        this->viewport_height = this->viewport_width / (float (this->image_width) / this->image_height);

        Vec3 viewport_top_left = Vec3 (-this->viewport_width / 2, this->viewport_height / 2, -this->focal_length);

        this->pixel_du = Vec3 (this->viewport_width, 0, 0) / this->image_width;
        this->pixel_dv = Vec3 (0, -this->viewport_height, 0) / this->image_height;

        this->pixel_00 = viewport_top_left + ((pixel_du + pixel_dv) / 2.0);

        this->max_depth = 50;

        this->samples_per_pixel = 100;
}

Ray Camera::ray (int du, int dv)
{
        Vec3 direction = pixel_00 + (pixel_du * du) + (pixel_dv * dv) - this->center;

        return Ray (this->center, direction);
}

Vec3 Camera::ray_color (Ray r, World *world, int depth)
{
        if (depth == 0)
                return Vec3 (0,0, 0);

        HitRecord record;
        if (world->hit (r, record)) {
                Vec3 color;
                Ray scatter;
                if (record.mat->scatter (r, record, color, scatter)) {
                        return color * this->ray_color (scatter, world, depth - 1);
                }
                
                return Vec3 (0, 0, 0);
        }
        double s = (r.direction.unit().y + 1.0) / 2.0;
        return Vec3 (1.0, 1.0, 1.0) * (1.0 - s) + Vec3 (0.5, 0.7, 1.0) * s;
}

void Camera::write_color (Vec3 color)
{
        color.x = std::sqrt(color.x);
        color.y = std::sqrt(color.y);
        color.z = std::sqrt(color.z);
        color = color.clamp(0, 0.999);
        color *= 256;

        std::cout << int (color.x) << ' ' << int (color.y) << ' ' << int (color.z) << '\n';
}

void Camera::render (World *world)
{
        std::cout << "P3\n"
                  << this->image_width << ' ' << this->image_height << "\n"
                  << "255"
                  << "\n";
        for (int j = 0; j < this->image_height; j++) {
                for (int i = 0; i < this->image_width; i++) {
                        Vec3 pixel_color (0, 0, 0);
                        for (int sample = 0; sample < this->samples_per_pixel; sample++) {
                                Ray r = this->ray (i + random_double (-0.5, 0.5), j + random_double (-0.5, 0.5));
                                // Ray r = this->ray (i , j );
                                pixel_color += this->ray_color (r, world, this->max_depth);
                        }
                        // std::cout << pixel_color.x << ' ' << pixel_color.y << ' ' << pixel_color.z << '\n';
                        pixel_color *= 1.0 / double(this->samples_per_pixel);

                        this->write_color (pixel_color);
                }
        }
}
