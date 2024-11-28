#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"

class Sphere : public Object {
    public:
        double radius;

        Sphere (Vec3 center, double radius, Material *material);
        Sphere (Vec3 center1, Vec3 center2, double radius, Material *material);
        bool hit (Ray r, HitRecord &record) override;
        Vec3 to_texture_uv (Vec3 point);

    private:
        double argument (double y_opp, double x_adj);
};