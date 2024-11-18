#pragma once
#include "hitrecord.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "material.hpp"


class Object {
    public:
        Vec3 location;
        Material *mat;
        Object (Vec3 location, Material *material);
        virtual bool hit (Ray r, HitRecord &record) = 0;
};