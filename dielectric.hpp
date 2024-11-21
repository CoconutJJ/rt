#pragma once
#include "material.hpp"

class Dielectric : public Material {

        public:
        Dielectric(double refraction_index);
        bool scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered) override;
        double reflectance(double cosine, double refraction_index);
        double refraction_index;
};