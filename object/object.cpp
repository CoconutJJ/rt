#include "object.hpp"
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