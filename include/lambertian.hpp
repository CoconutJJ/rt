#pragma once
#include "hitrecord.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"

class Lambertian : public Material {
    public:
        Lambertian (Vec3 solid_color);
        Lambertian (Texture *texture);

        Vec3 scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob) override;

    private:
        Vec3 sample_direction (HitRecord &record, double &phi);
        double pdf (double phi);
        Vec3 albedo;
};