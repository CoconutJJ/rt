#include "material.hpp"
#include "vec3.hpp"
class Lambertian : public Material {
    public:
        Lambertian (Vec3 albedo);
        bool scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered) override;

    private:
        Vec3 albedo;
};