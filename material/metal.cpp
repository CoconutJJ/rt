#include "metal.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "vec3.hpp"

Metal::Metal (Vec3 albedo, double fuzz) : Material (new SolidTexture(albedo), nullptr), albedo (albedo), fuzz (fuzz)
{
}

Vec3 Metal::scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob)
{
        Vec3 reflect_direction = r.direction.reflect (record.normal).unit () + Vec3::random ().unit () * fuzz;

        brdf = Vec3(1,1,1);

        return reflect_direction.unit ();
}

