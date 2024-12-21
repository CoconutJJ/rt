#include "object.hpp"
Object::Object ()
{
}
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

Material *Object::material ()
{
        return this->mat;
}