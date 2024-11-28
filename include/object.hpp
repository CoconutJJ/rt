#pragma once
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

class Object {
    public:
        Vec3 location;
        Vec3 location2;
        Ray displacement;
        Material *mat;
        Object (Vec3 location, Material *material);
        Object (Vec3 location1, Vec3 location2, Material *material);
        virtual bool hit (Ray r, HitRecord &record) = 0;
        virtual bool is_light_source ();
        Material *material ();
        Vec3 location_at_time (double time);
};