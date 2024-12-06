#pragma once

#include "material.hpp"
#include "plane.hpp"
#include "vec3.hpp"


class Triangle : Plane {

    public:

        Triangle(Vec3 point, Vec3 u, Vec3 v, Material *material);
        bool inside(Vec3 point);
        bool hit (Ray r, HitRecord &record) override;
};