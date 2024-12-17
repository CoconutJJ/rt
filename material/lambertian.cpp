#include "lambertian.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

Lambertian::Lambertian (Vec3 albedo) : Material (new SolidTexture (albedo), nullptr)
{
}

Lambertian::Lambertian (Texture *texture) : Material (texture, nullptr)
{
}

Vec3 Lambertian::scatter (Ray r, HitRecord &record, Vec3 &brdf)
{
        Vec3 out_direction = record.obj->tnb (record.hit_point) *
                             Vec3 (1, random_double (0, 2 * M_PI), random_double (0, M_PI / 2)).sph_inv ();

        brdf = record.obj->brdf (record.hit_point, r.direction, out_direction);

        return out_direction;
}
