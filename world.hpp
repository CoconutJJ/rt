#pragma once
#include "light.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <vector>

class World {
    public:
        std::vector<Object *> objects;
        std::vector<Light *> lights;

        World ();
        void add (Object *obj);
        bool hit (Ray r, HitRecord &record);
        bool has_path (Ray r, Object *obj);
        bool has_path (Vec3 a, Vec3 b);
        void add_light (Light *light);
};