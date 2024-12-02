#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
class Plane : public Object {
    public:
        Vec3 n, u, v;
        Plane (Vec3 location, Vec3 normal, Material *material);

        bool hit (Ray r, HitRecord &record) override;
        Vec3 to_uv (Vec3 point) override;
        Vec3 tangent (Vec3 point) override;
        Vec3 normal (Vec3 point) override;
};