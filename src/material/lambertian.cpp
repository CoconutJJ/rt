#include "lambertian.hpp"
#include "camera.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

extern Camera::RendererSettings config;

Lambertian::Lambertian (Vec3 solid_color)
        : Material (new SolidTexture (solid_color), nullptr)
{
}

Lambertian::Lambertian (Texture *texture)
        : Material (texture, nullptr)
{
}

inline double Lambertian::pdf (double phi)
{
        if (!config.use_importance_sampling)
                return 1 / (2 * M_PI);

        return cos (phi) / M_PI;
}

inline Vec3 Lambertian::sample_direction (HitRecord &record, double &phi)
{
        if (!config.use_importance_sampling) {
                phi = random_double (0, M_PI / 2);
                return record.object->tnb (record) * Vec3 (1, random_double (0, 2 * M_PI), phi).sph_inv ();
        }

        double theta = 2 * M_PI * random_double (0, 1);
        phi = acos (sqrt (random_double (0, 1)));

        return record.object->tnb (record) * Vec3 (1, theta, phi).sph_inv ();
}

Vec3 Lambertian::scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob)
{
        double phi;
        Vec3 out_direction = this->sample_direction (record, phi);
        ray_prob = this->pdf (phi);
        brdf = this->color(record) / M_PI;

        return out_direction;
}
