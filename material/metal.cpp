#include "metal.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Metal::Metal (Vec3 albedo, double fuzz) : Material (), albedo (albedo), fuzz (fuzz)
{
}

bool Metal::scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
{
        Vec3 reflect_direction = r.direction.reflect (record.normal).unit () + Vec3::random ().unit () * fuzz;

        scattered = Ray (record.hit_point, reflect_direction, r.time);

        attenuation = albedo;

        return scattered.direction.dot (record.normal) > 0;
}