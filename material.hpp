#pragma once
#include "vec3.hpp"
#include "ray.hpp"

class HitRecord;

class Material {

    public:
        virtual bool scatter(
            Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered
        ) {
            return false;
        }

};