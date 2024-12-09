#pragma once
#include "material.hpp"

class Dielectric : public Material {
    public:
        Dielectric (double refraction_index);
        Vec3 scatter (Ray r, HitRecord &record) override;
        double reflectance (double cosine, double refraction_index);
        double refraction_index;
};