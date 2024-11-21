#pragma once
#include "vec3.hpp"
#include "ray.hpp"
#include "material.hpp"


class HitRecord {
    public:
        HitRecord ();
        Vec3 hit_point;
        double lambda;
        Vec3 normal;
        Material *mat;
        bool front_face;
        void setNormal(Ray r, Vec3 normal);
};