#include "material.hpp"
#include "vec3.hpp"
class Metal : public Material {
    public:
        Metal (double fuzz, Vec3 color);
        Metal (double fuzz, Texture *texture);
        virtual Vec3 scatter (Ray r, HitRecord &record, Vec3 &brdf, double &ray_prob) override;

    private:
        Vec3 albedo;
        double fuzz;
};