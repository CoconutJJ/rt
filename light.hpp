#pragma once

#include "object.hpp"
#include "vec3.hpp"
class Light {

    public:
        Vec3 diffuse_intensity;
        Vec3 specular_intensity;
        Vec3 location;

        Light(Vec3 location,Vec3 Id, Vec3 Is);



};