#include "lambertian.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

Lambertian::Lambertian (double reflectivity, Vec3 solid_color)
        : Material (new SolidTexture (solid_color), nullptr), reflectivity (reflectivity)
{
}

Lambertian::Lambertian (double reflectivity, Texture *texture)
        : Material (texture, nullptr), reflectivity (reflectivity)
{
}

Vec3 Lambertian::scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob)
{
        Vec3 random_direction = Vec3 (1, random_double (0, 2 * M_PI), random_double (0, M_PI / 2));

        Vec3 out_direction = record.object->tnb (record) * random_direction.sph_inv ();

        ray_prob = 1;

        brdf = this->reflectivity / M_PI * Vec3 (1, 1, 1);

        return out_direction;
}
