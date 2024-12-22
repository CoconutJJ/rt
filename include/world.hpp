#pragma once
#include "light.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "vec3.hpp"
#include <vector>

class World {
    public:
        struct Photon {
                Vec3 point;
                Vec3 color;
        };

        std::vector<Object *> objects;
        std::vector<Light *> lights;
        std::vector<struct Photon> photons;
        World ();
        void add (Object *obj);
        bool hit (Ray r, HitRecord &record);
        bool has_path (Ray r, Object *obj);
        bool has_path (Vec3 a, Vec3 b);
        void add_light (Light *light);
        Vec3 photon_map_color (Vec3 point);
        void photon_map_forward_pass ();
};