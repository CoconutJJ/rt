#pragma once

#include "light.hpp"
#include "vec3.hpp"
class PointLight : public Light {

    public:
        Vec3 point;
        Vec3 Id, Is;
        PointLight(Vec3 point, Vec3 Id, Vec3 Is);
        Vec3 sample_point() override;
        bool is_point_light() override;
        Vec3 specular_intensity(Vec3 point) override;
        Vec3 diffuse_intensity(Vec3 point) override;


};