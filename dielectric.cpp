#include "dielectric.hpp"
#include "hitrecord.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

Dielectric::Dielectric (double refraction_index) : refraction_index (refraction_index)
{
}

bool Dielectric::scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
{
        attenuation = Vec3 (1, 1, 1);
        double mu = record.front_face ? (1 / refraction_index) : refraction_index;
        Vec3 unit_direction = r.direction.unit ();

        double cos_theta = std::fmin (-unit_direction.dot (record.normal), 1.0);
        double sin_theta = std::sqrt (1.0 - cos_theta * cos_theta);

        Vec3 direction;
        if (mu * sin_theta > 1.0 || reflectance (cos_theta, mu) > random_double (0, 1)) {
                direction = unit_direction.reflect (record.normal);
        } else {
                direction = unit_direction.refract (record.normal, mu);
        }

        scattered = Ray (record.hit_point, direction);

        return true;
}

double Dielectric::reflectance (double cosine, double refraction_index)
{
        double r0 = (1 - refraction_index) / (1 + refraction_index);

        r0 = r0 * r0;

        return r0 + (1 - r0) * std::pow ((1 - cosine), 5.0);
}