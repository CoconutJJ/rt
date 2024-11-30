#pragma once

#include "light.hpp"
#include "quad.hpp"
#include "vec3.hpp"
class QuadLight : public Light {
    public:
        Quad *quad;
        QuadLight (Quad *quad);
        bool is_point_light () override;
        Vec3 sample_point () override;
        Vec3 diffuse_intensity (Vec3 point) override;
        Vec3 specular_intensity (Vec3 point) override;
};