#pragma once
#include "vec3.hpp"
class Ray {
    public:
        Ray ();
        Ray (Vec3 origin, Vec3 direction, double time);
        Ray (Vec3 origin, Vec3 direction, Vec3 color);
        Ray (Vec3 origin, Vec3 direction);

        Vec3 origin;
        Vec3 direction;
        double time;
        Vec3 color;

        Vec3 at (double t);
        Ray nudge (double maxX, double maxY);

        bool can_refract (Vec3 normal, double mu);
};