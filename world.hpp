#pragma once
#include "object.hpp"
#include <vector>


class World {
    public:
        std::vector<Object*> objects;

        World ();
        void add (Object *obj);
        bool hit (Ray r, HitRecord &record);
};