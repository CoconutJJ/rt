#pragma once
#include "mat3.hpp"
#include "vec3.hpp"
class BRDF {
    public:
        BRDF (const char *filename);
        double *brdf;
        Mat3 sph_basis (double theta, double phi);
        Vec3 compute (Mat3 tnb, Vec3 in_direction, Vec3 out_direction);
};