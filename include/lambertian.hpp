#pragma once
#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"
class Lambertian : public Material {
    public:
        double reflectivity;

        Lambertian (double reflectivity, Vec3 solid_color);
        Lambertian (double reflectivity, Texture *texture);
        Vec3 scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob) override;
    private:
        Vec3 albedo;
};