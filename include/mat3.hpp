#pragma once

#include "vec3.hpp"
class Mat3 {
    public:
        Vec3 c1, c2, c3;
        Mat3 ();
        Mat3 (const Mat3 &other);
        Mat3 &operator= (Mat3 other);
        Mat3 (Vec3 c1, Vec3 c2, Vec3 c3);
        Mat3 transpose ();
        Vec3 operator* (Vec3 x);
        Mat3 operator* (double d);
        Mat3 operator* (Mat3 B);

        friend std::ostream &operator<< (std::ostream &out, Mat3 m);
};