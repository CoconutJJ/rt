#include "sphere.hpp"
#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <cmath>

Sphere::Sphere (Vec3 center, double radius, Material *material) : Object (center, material)
{
        this->radius = radius;
}

Sphere::Sphere (Vec3 from, Vec3 to, double radius, Material *material) : Object (from, to, material)
{
        this->radius = radius;
}

bool Sphere::hit (Ray r, HitRecord &record)
{
        Vec3 current_center = this->location_at_time (r.time);

        double a = r.direction.dot (r.direction);
        double b = r.direction.dot (r.origin - current_center) * 2.0;
        double c = (current_center - r.origin).dot (current_center - r.origin) - this->radius * this->radius;

        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0)
                return false;

        record.lambda = (-b - std::sqrt (discriminant)) / (2 * a);

        if (record.lambda < 0) {
                record.lambda = (-b + sqrt (discriminant)) / (2 * a);

                if (record.lambda < 0)
                        return false;
        }

        record.hit_point = r.at (record.lambda);
        record.setNormal (r, (record.hit_point - this->location));
        record.mat = this->mat;
        record.uv = this->to_texture_uv (record.hit_point);
        return true;
}

double Sphere::argument (double y_opp, double x_adj)
{
        double y = std::abs (y_opp);
        double x = std::abs (x_adj);

        if (x_adj > 0 && y_opp >= 0)
                return std::atan (y / x);
        else if (x_adj < 0 && y_opp >= 0)
                return M_PI - std::atan (y / x);
        else if (x_adj < 0 && y_opp <= 0)
                return M_PI + std::atan (y / x);
        else
                return 2 * M_PI - std::atan (y / x);
}
Vec3 Sphere::to_texture_uv (Vec3 point)
{
        point = point - this->location;

        double theta = this->argument (-point.z, point.x);

        double phi = std::acos (point.y / point.length ());

        double v = phi / M_PI;

        double u = theta / (2 * M_PI);

        return Vec3 (u, v, 0);
}