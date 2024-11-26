#pragma once

#include "light.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <vector>
class Phong : public Material {

    public:
        Phong(World * world, Vec3 color, Vec3 camera_location, double rs, double rd, double ra, double shininess);
        World * world;
        Vec3 color;
        Vec3 camera_location;
        double rs, rd, ra, shininess;
        std::vector<Light*> sources; 
        bool scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered) override;
        void add_light(Light * light);

};