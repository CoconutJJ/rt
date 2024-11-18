#include "sphere.hpp"
#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
#include <cmath>

Sphere::Sphere (Vec3 center, double radius, Material *material) : Object (center, material)
{
        this->radius = radius;
}

bool Sphere::hit (Ray r, HitRecord &record)
{
        double a = r.direction.dot (r.direction);
        double b = r.direction.dot (r.origin - this->location) * 2.0;
        double c = (this->location - r.origin).dot (this->location - r.origin) - this->radius * this->radius;

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
        record.setNormal (r, (record.hit_point - this->location).unit ());
        record.mat = this->mat;

        return true;
}