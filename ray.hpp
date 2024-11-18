#pragma once
#include "vec3.hpp"
class Ray {
    public:
        Ray();
        Ray (Vec3 origin, Vec3 direction);
        Vec3 origin;
        Vec3 direction;

        Vec3 at (double t);
        Ray nudge (double maxX, double maxY);
};