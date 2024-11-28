#pragma once

#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
class Quad : public Object {
    public:
        Quad (Vec3 location, Vec3 v1, Vec3 v2, Material *mat);
        bool hit (Ray r, HitRecord &record) override;

        Vec3 find_alpha_beta (Vec3 point);
        Vec3 to_texture_uv (Vec3 point);
        Vec3 get_point (double alpha, double beta);
        Vec3 v1, v2;
};