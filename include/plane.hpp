#pragma once

#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
class Plane : public Object {
    public:
        Vec3 n, u, v;
        Plane (Vec3 location, Vec3 normal, Material *material);
        Plane (Vec3 location, Vec3 u, Vec3 v, Material *material);


        bool hit_point(Ray r, Vec3 &point, double &lambda);
        bool hit (Ray r, HitRecord &record) override;
        Vec3 to_uv (Vec3 point) override;
        Vec3 tangent (Vec3 point) override;
        Vec3 normal (Vec3 point) override;
};