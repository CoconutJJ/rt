/**
    @file object.hpp

    @brief This file contains the definition of the base Object class. It should
    not be used directly, but rather as a base class for other objects (mesh,
    smooth object, etc.).
*/

#pragma once
#include "material.hpp"
#include "ray.hpp"

class Object {
    public:
        Object ();
        Object (Vec3 location, Material *material);
        Object (Vec3 location1, Vec3 location2, Material *material);
        
        virtual ~Object () {};
        virtual bool hit (Ray r, HitRecord &record) = 0;
        virtual bool is_light_source () = 0;
        Vec3 location_at_time (double time);

        Vec3 location;
        Vec3 location2;
        Ray displacement;
        Material *material;
};