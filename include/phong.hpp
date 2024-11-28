#pragma once

#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "world.hpp"

class Phong : public Material {
    public:
        Phong (double rs,
               double rd,
               double ra,
               double rg,
               double shininess,
               Texture *texture);
        World *world;
        Vec3 camera_location;
        double rs, rd, ra, rg, shininess;
        PhongParams phong (Ray r, HitRecord &record) override;
};