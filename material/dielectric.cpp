#include "dielectric.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "utils.hpp"
#include "vec3.hpp"

#include <cmath>

Dielectric::Dielectric (double refraction_index, double absorption)
        : refraction_index (refraction_index), absorption (absorption)
{
        this->texture = new SolidTexture (Vec3 (1, 1, 1));
}

Dielectric::~Dielectric ()
{
        delete this->texture;
}

Vec3 Dielectric::scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob)
{
        double mu = record.front_face ? (1 / refraction_index) : refraction_index;
        Vec3 unit_direction = r.direction.unit ();

        double cos_theta = std::fmin (-unit_direction.dot (record.normal), 1.0);

        Vec3 direction;
        double attenuation = 1;

        if (!record.front_face) {
                double d = (record.lambda * r.direction).length ();
                attenuation = exp (-absorption * d);
        }

        double refl = reflectance (cos_theta, mu);

        // direction = unit_direction.refract (record.normal, mu);
        // brdf = (1 - refl) * Vec3 (1, 1, 1) * attenuation;
        // ray_prob = 1 - refl;

        if (!r.can_refract (record.normal, mu)) {
                brdf = Vec3 (1, 1, 1) * refl * attenuation;
                ray_prob = refl;
                return unit_direction.reflect (record.normal);
        }

        if (random_double (0, 1) < refl) {
                direction = unit_direction.reflect (record.normal);
                brdf = Vec3 (1, 1, 1) * refl * attenuation;
                ray_prob = refl;

        } else {
                direction = unit_direction.refract (record.normal, mu);
                brdf = (1 - refl) * Vec3 (1, 1, 1) * attenuation;
                ray_prob = 1 - refl;
        }

        return direction;
}

double Dielectric::reflectance (double cosine, double refraction_index)
{
        double r0 = (1 - refraction_index) / (1 + refraction_index);

        r0 = r0 * r0;

        return r0 + (1 - r0) * std::pow ((1 - cosine), 5.0);
}
