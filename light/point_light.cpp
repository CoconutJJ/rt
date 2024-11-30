#include "point_light.hpp"
#include "light.hpp"
#include "vec3.hpp"

PointLight::PointLight (Vec3 point, Vec3 Id, Vec3 Is) : Light (), point (point), Id (Id), Is (Is)
{
}

Vec3 PointLight::diffuse_intensity (Vec3 point)
{
        return this->point == point ? this->Id : Vec3::zero ();
}

Vec3 PointLight::specular_intensity (Vec3 point)
{
        return this->point == point ? this->Is : Vec3::zero ();
}

bool PointLight::is_point_light ()
{
        return true;
}

Vec3 PointLight::sample_point ()
{
        return this->point;
}