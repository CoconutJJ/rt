
#include "world.hpp"
#include "object.hpp"
#include <cfloat>
World::World ()
{
}

void World::add (Object *obj)
{
        this->objects.push_back (obj);
}

bool World::hit (Ray r, HitRecord &record)
{
        HitRecord curr_record;

        double lambda_min = 0.001;
        double lambda_max = DBL_MAX;

        bool hit_anything = false;

        for (Object *obj : this->objects) {
                if (!obj->hit (r, curr_record))
                        continue;

                if (curr_record.lambda < lambda_max && curr_record.lambda > lambda_min) {
                        lambda_max = curr_record.lambda;
                        record = curr_record;
                        hit_anything = true;
                }
        }

        return hit_anything;
}
