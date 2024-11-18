#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
class Plane : public Object {
    public:
        Vec3 normal;
        Plane(Vec3 location, Vec3 norma, Material *material);

        virtual bool hit(Ray r, HitRecord &record) override;
};