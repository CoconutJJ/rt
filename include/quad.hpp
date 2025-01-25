#pragma once

#include "material.hpp"
#include "smooth_object.hpp"
#include "vec3.hpp"
class Quad : public SmoothObject {
    public:
        Quad (Vec3 location, Vec3 v1, Vec3 v2, Material *mat);
        bool hit (Ray r, HitRecord &record) override;

        Vec3 find_alpha_beta (Vec3 point);
        bool &one_sided();
        Vec3 to_uv (Vec3 point) override;
        Vec3 get_point (double alpha, double beta);
        Vec3 tangent (Vec3 point) override;
        Vec3 normal (Vec3 point) override;
        Vec3 sample_point () override;
        double area () override;
        Vec3 v1, v2;
        
        private:
        bool _one_sided;
        Vec3 _normal();
};