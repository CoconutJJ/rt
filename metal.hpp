#include "material.hpp"
#include "vec3.hpp"
class Metal : public Material {
    public:
        Metal(Vec3 albedo, double fuzz);
        virtual bool scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered) override;

    private:
        Vec3 albedo;
        double fuzz;
};