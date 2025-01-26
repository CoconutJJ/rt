#pragma once
#include "mat3.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"
class BRDF : public Material {
    public:
        BRDF (const char *filename, Texture *texture);
        double *brdf;
        Mat3 sph_basis (double theta, double phi);
        Vec3 compute (Mat3 tnb, Vec3 in_direction, Vec3 out_direction);
        Vec3 scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob) override;
};