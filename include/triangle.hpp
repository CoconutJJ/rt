#pragma once

#include "material.hpp"
#include "plane.hpp"
#include "vec3.hpp"

class Triangle : public Plane {
    public:
        Triangle (Vec3 point, Vec3 u, Vec3 v, Material *material);
        Triangle (Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, Material *material);
        bool inside (Vec3 point);
        bool hit (Ray r, HitRecord &record) override;
};