#pragma once
#include "mat3.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include "vec3.hpp"

class HitRecord;

class Material {
    public:
        Material ();
        Material (Texture *texture, Texture *normal_map);
        Texture *texture;
        Texture *normal_map;

        struct PhongParams {
                double rs;
                double ra;
                double rd;
                double rg;
                double rt;
                double alpha;
                double gamma;
                double mu;
                Vec3 color;
        };

        virtual Vec3 scatter (Ray r, HitRecord &record)
        {
                return Vec3(0,0,0);
        }

        virtual PhongParams phong (Ray r, HitRecord &record)
        {
                return PhongParams{ 0, 0, 0, 0, 0, 0, 0, 0, Vec3 (0, 0, 0) };
        };

        virtual Vec3 normal (Mat3 tbn, Vec3 n, Vec3 uv);
};