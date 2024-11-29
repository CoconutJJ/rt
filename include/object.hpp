#pragma once
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include "vec3.hpp"

class Object {
    public:
        Vec3 location;
        Vec3 location2;
        Ray displacement;
        Material *mat;

        Object (Vec3 location, Material *material);
        Object (Vec3 location1, Vec3 location2, Material *material);
        Object (Vec3 location1, Vec3 location2, Material *material, Texture *normal_map);
        virtual bool hit (Ray r, HitRecord &record) = 0;
        virtual bool is_light_source ();
        Material *material ();
        Vec3 location_at_time (double time);
        virtual Vec3 tangent (Vec3 point) = 0;
        virtual Vec3 normal (Vec3 point) = 0;
        virtual Vec3 to_uv(Vec3 point) = 0;
        Vec3 mapped_normal (Vec3 point);
        Mat3 tbn (Vec3 point);
        Vec3 tbn_transform(Vec3 point, Vec3 tangent_v);
};