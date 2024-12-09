#include "material.hpp"
#include "vec3.hpp"
class Metal : public Material {
    public:
        Metal (Vec3 albedo, double fuzz);
        virtual Vec3 scatter (Ray r, HitRecord &record) override;

    private:
        Vec3 albedo;
        double fuzz;
};