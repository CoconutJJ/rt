#pragma once

#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "world.hpp"

class Phong : public Material {
    public:
        Phong (double rs, // specular reflection coefficient
               double rd, // diffuse reflection coefficient
               double ra, // ambient reflection coefficient
               double rg, // secondary reflection coeficient
               double shininess,
               double gamma,
               double mu,
               Texture *texture,
               Texture *normal_map);
        Phong (double rs, double rd, double ra, double rg, double shininess, double gamma, double mu, Texture *texture);

        World *world;
        Vec3 camera_location;
        double rs, rd, ra, rg, shininess, gamma, mu;
        PhongParams phong (Ray r, HitRecord &record) override;
};