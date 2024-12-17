#include "dielectric.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

Dielectric::Dielectric (double refraction_index) : refraction_index (refraction_index)
{
        this->texture = new SolidTexture (Vec3 (1, 1, 1));
}

Vec3 Dielectric::scatter (Ray r, HitRecord &record, Vec3 &brdf)
{
        double mu = record.front_face ? (1 / refraction_index) : refraction_index;
        Vec3 unit_direction = r.direction.unit ();

        double cos_theta = std::fmin (-unit_direction.dot (record.normal), 1.0);
        double sin_theta = std::sqrt (1.0 - cos_theta * cos_theta);

        Vec3 direction;
        if (mu * sin_theta > 1.0 || reflectance (cos_theta, mu) > random_double (0, 1)) {
                direction = unit_direction.reflect (record.normal);
                brdf = record.obj->brdf (record.hit_point, r.direction, direction);
        } else {
                direction = unit_direction.refract (record.normal, mu);
                brdf = Vec3 (1, 1, 1);
        }

        return direction;
}

double Dielectric::reflectance (double cosine, double refraction_index)
{
        double r0 = (1 - refraction_index) / (1 + refraction_index);

        r0 = r0 * r0;

        return r0 + (1 - r0) * std::pow ((1 - cosine), 5.0);
}