#pragma once
#include "hitrecord.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "material.hpp"


class Object {
    public:
        Vec3 location;
        Vec3 location2;
        Ray displacement;
        Material *mat;
        Object (Vec3 location, Material *material);
        Object (Vec3 location1, Vec3 location2, Material *material);
        virtual bool hit (Ray r, HitRecord &record) = 0;
        Vec3 location_at_time(double time);
};