#pragma once

#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
class Box : Object {

    public:
        Box(Vec3 min, Vec3 max, Material *material);
        bool hit (Ray r, HitRecord &record) override;
    private:
        Vec3 min, max;



};