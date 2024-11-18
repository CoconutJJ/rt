#include "metal.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Metal::Metal (Vec3 albedo) : Material (), albedo (albedo)
{
}

bool Metal::scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
{
        Vec3 reflect_direction = r.direction.reflect (record.normal.unit());

        scattered = Ray (record.hit_point, reflect_direction);

        attenuation = albedo;

        return true;
}