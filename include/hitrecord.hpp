#pragma once
#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"

class Object;

class HitRecord {
    public:
        HitRecord ();
        Vec3 hit_point;
        double lambda;
        Vec3 normal;
        Material *mat;
        Object *obj;
        Vec3 uv;
        bool front_face;
        void setNormal (Ray r, Vec3 normal);
        Vec3 outward_normal ();
};