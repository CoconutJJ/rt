#include "lambertian.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "hitrecord.hpp"

Lambertian::Lambertian (Vec3 albedo) : Material(), albedo(albedo) {

}


bool Lambertian::scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
{
        Vec3 scatter_direction = record.normal.unit() + Vec3::random ().unit ();

        if (scatter_direction.near_zero ())
                scatter_direction = record.normal;

        scattered = Ray (record.hit_point, scatter_direction);

        attenuation = this->albedo;

        return true;
}