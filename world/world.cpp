
#include "world.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <cfloat>
World::World ()
{
}

void World::add (Object *obj)
{
        this->objects.push_back (obj);
}

void World::add_light (Light *light)
{
        this->lights.push_back (light);
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

bool World::has_path (Ray r, Object *obj)
{
        HitRecord obj_rec;
        if (!obj->hit (r, obj_rec))
                return false;

        double lambda_min = 0.001;

        for (Object *curr : this->objects) {
                if (curr == obj)
                        continue;

                HitRecord rec;

                if (!obj->hit (r, rec))
                        continue;

                if (rec.lambda < obj_rec.lambda && rec.lambda > lambda_min) {
                        return false;
                }
        }

        return true;
}

bool World::has_path (Vec3 a, Vec3 b)
{
        Ray r (a, (b - a).unit ());

        HitRecord rec;

        if (!this->hit (r, rec))
                return true;

        return rec.lambda > (b - a).length () - 0.001;
}