#pragma once
#include "ray.hpp"
#include "texture.hpp"
#include "vec3.hpp"

class HitRecord;

class Material {
    public:
        Material ();
        Material (Texture *texture);

        Texture *texture;
        struct PhongParams {
                double rs;
                double ra;
                double rd;
                double rg;
                double alpha;
                // double gamma;
                Vec3 color;
        };

        virtual bool scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
        {
                return false;
        }

        virtual PhongParams phong (Ray r, HitRecord &record)
        {
                return PhongParams{ 0, 0, 0, 0, 0, Vec3 (0, 0, 0) };
        };
};