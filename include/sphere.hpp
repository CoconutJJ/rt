#include "material.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "vec3.hpp"

class Sphere : public SmoothObject {
    public:
        double radius;

        Sphere (Vec3 center, double radius, Material *material);
        Sphere (Vec3 center1, Vec3 center2, double radius, Material *material);
        bool hit (Ray r, HitRecord &record) override;
        Vec3 to_uv (Vec3 point) override;
        Vec3 tangent (Vec3 point) override;
        Vec3 normal (Vec3 point) override;
        Vec3 tangent (double theta, double phi);
        Vec3 bitangent (double theta, double phi);
        Vec3 spherical_coord (Vec3 point);

    private:
        double argument (double y_opp, double x_adj);
};