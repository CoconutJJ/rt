#include "metal.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Metal::Metal (Vec3 albedo, double fuzz) : Material (), albedo (albedo), fuzz (fuzz)
{
}

Vec3 Metal::scatter (Ray r, HitRecord &record)
{
        Vec3 reflect_direction = r.direction.reflect (record.normal).unit () + Vec3::random ().unit () * fuzz;

        return reflect_direction.unit ().sph ();
}