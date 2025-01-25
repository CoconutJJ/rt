
#include "world.hpp"
#include "hitrecord.hpp"
#include "light.hpp"
#include "material.hpp"
#include "progress_bar.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "vec3.hpp"
#include <cfloat>
#include <cstdlib>
#include <utility>
#include <vector>

World::World ()
{
}

void World::add (Object *obj)
{
        this->objects.push_back (obj);

        SmoothObject *smooth_obj = dynamic_cast<SmoothObject *> (obj);

        if (!smooth_obj)
                return;

        if (smooth_obj->material->is_emissive ())
                this->emissives.push_back (smooth_obj);
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

SmoothObject *World::random_light ()
{

        if (this->emissives.size() == 0)
                return nullptr;

        return this->emissives[rand () % this->emissives.size ()];
}

Vec3 World::photon_map_color (Vec3 point)
{
        Vec3 color (0, 0, 0);
        int count = 0;
        for (struct Photon p : this->photons) {
                if ((p.point - point).length_squared () < 0.025) {
                        color += p.color;
                        count++;
                }
        }

        if (count == 0)
                return Vec3::zero ();

        return (color / count) * (double (count) / this->photons.size ());
}

void World::photon_map_forward_pass ()
{
        int rays_per_light = 1e6;
        int max_depth = 10;
        progressbar bar (rays_per_light * this->lights.size ());

        for (Light *light : this->lights) {
                for (int i = 0; i < rays_per_light; i++) {
                        Vec3 origin = light->sample_point ();

                        Vec3 direction = Vec3::random ();

                        Ray r (origin, direction, light->specular_intensity (origin));

                        std::vector<std::pair<Ray, int> > worklist;
                        worklist.push_back (std::pair (r, max_depth));

                        while (worklist.size () > 0) {
                                std::pair<Ray, int> item = worklist.back ();

                                worklist.pop_back ();

                                Ray curr_ray = item.first;
                                int depth = item.second;

                                HitRecord record;

                                if (!this->hit (curr_ray, record)) {
                                        continue;
                                }

                                Material::PhongParams params = record.object->material->phong (curr_ray, record);

                                if (params.gamma == 1.0) {
                                        if (depth == max_depth)
                                                continue;

                                        struct Photon p = {  .point = record.hit_point, .color = curr_ray.color };
                                        this->photons.push_back (p);

                                } else {
                                        if (depth == 0)
                                                continue;

                                        Ray reflected = Ray (record.hit_point,
                                                             curr_ray.direction.unit ().reflect (record.normal),
                                                             curr_ray.color * params.color);

                                        worklist.push_back (std::pair (reflected, depth - 1));

                                        double mu = record.front_face ? params.mu : 1 / params.mu;

                                        if (curr_ray.can_refract (record.normal, mu)) {
                                                Ray refracted =
                                                        Ray (record.hit_point,
                                                             curr_ray.direction.unit ().refract (record.normal, mu),
                                                             curr_ray.color * params.color);

                                                worklist.push_back (std::pair (refracted, depth - 1));
                                        }
                                }
                        }

                        bar.update ();
                }
        }
}