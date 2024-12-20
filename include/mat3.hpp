#pragma once

#include "vec3.hpp"
class Mat3 {
    public:
        Vec3 c1, c2, c3;
        Mat3 (Vec3 c1, Vec3 c2, Vec3 c3);
        Mat3 transpose ();
        Vec3 operator* (Vec3 x);
        Mat3 operator* (double d);
        Mat3 operator* (Mat3 B);
};