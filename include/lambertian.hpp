#pragma once
#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"
class Lambertian : public Material {
    public:
        Lambertian (Vec3 albedo);
        Lambertian (Texture *texture);
        Vec3 scatter (Ray r, HitRecord &record, Vec3 &brdf) override;

    private:
        Vec3 albedo;
};