#pragma once
#include "material.hpp"

class Dielectric : public Material {
    public:
        Dielectric (double refraction_index, double absorption);
        ~Dielectric ();
        Vec3 scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob) override;

        static double reflectance (double cosine, double refraction_index);
        double refraction_index, absorption;
};