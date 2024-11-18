#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"

class Sphere : public Object {
    public:
        double radius;
        Sphere (Vec3 center, double radius, Material *material);
        bool hit (Ray r, HitRecord &record) override;
};