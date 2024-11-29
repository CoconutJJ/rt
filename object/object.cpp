#include "object.hpp"
#include "mat3.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Object::Object (Vec3 location1, Vec3 location2, Material *material)
        : location (location1), location2 (location2), mat (material)
{
        this->displacement = Ray (location1, location2 - location1);
}

Object::Object (Vec3 location, Material *material) : location (location), mat (material)
{
        this->displacement = Ray (location, Vec3 (0, 0, 0));
}

Vec3 Object::location_at_time (double time)
{
        return this->displacement.at (time);
}

bool Object::is_light_source ()
{
        return false;
}

Material *Object::material ()
{
        return this->mat;
}

Vec3 Object::mapped_normal (Vec3 point)
{

        if (!this->mat->normal_map)
                return this->normal(point);

        return this->mat->normal(this->tbn(point), this->normal(point), this->to_uv(point));
}

Mat3 Object::tbn (Vec3 point)
{
        Vec3 tangent = this->tangent (point);
        Vec3 normal = this->normal (point);

        Mat3 tbn (tangent, normal.cross (tangent), normal);

        return tbn;
}

Vec3 Object::tbn_transform (Vec3 point, Vec3 tangent_v)
{
        return this->tbn (point) * tangent_v;
}