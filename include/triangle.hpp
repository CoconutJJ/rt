#pragma once

#include "material.hpp"
#include "smooth_object.hpp"
#include "vec3.hpp"
#include <vector>

class Triangle : public SmoothObject {
    public:
        Triangle (Vec3 point, Vec3 u, Vec3 v, Material *material);
        Triangle (Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, Material *material);
        Triangle (const Triangle &triangle);
        Triangle &operator= (Triangle &triangle);

        Vec3 p2, p3, n;
        // Texture coordinates
        Vec3 t1, t2, t3;
        // Texture projection matrix
        Mat3 T;

        std::vector<Vec3> verticies ();
        void load_texture_coordinates (Vec3 t1, Vec3 t2, Vec3 t3);
        bool inside (Vec3 point);
        bool hit (Ray r, HitRecord &record) override;
        void center (Vec3 point);
        Vec3 center();
        Vec3 to_uv (Vec3 point) override;
        Vec3 tangent (Vec3 point) override;
        Vec3 normal (Vec3 point) override;
        double area () override;
        Vec3 sample_point () override;
        Triangle* translate(Vec3 v);
        Triangle* scale(double s);


    private:
        Vec3 _u ();
        Vec3 _v ();
};