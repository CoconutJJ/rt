#include "phong.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <cmath>
#include <iostream>

Phong::Phong (World *world, Vec3 color, Vec3 camera_location, double rs, double rd, double ra, double shininess)
        : world (world), color (color), camera_location (camera_location), rs (rs), rd (rd), ra (ra),
          shininess (shininess)
{
}

bool Phong::scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
{
        Vec3 radiance = Vec3 (1, 1, 1) * this->ra;
        Vec3 to_camera = (this->camera_location - record.hit_point).unit ();
        Vec3 normal = record.front_face ? record.normal : -record.normal;

        for (Light *light_source : this->sources) {
                Vec3 to_light = (light_source->location - record.hit_point).unit ();

                Ray to_light_ray = Ray (record.hit_point, to_light);

                HitRecord rec;
                if (this->world->hit (to_light_ray, rec)) {
                        continue;
                }

                Vec3 mirror_direction = (-to_light).reflect (normal).unit ();

                radiance += light_source->diffuse_intensity * (this->rd * (std::max (0.0, to_light.dot (normal)))) +
                            light_source->specular_intensity *
                                    (this->rs *
                                     std::pow (std::max (0.0, mirror_direction.dot (to_camera)), this->shininess));
        }
        attenuation = radiance.elem_mul (this->color).clamp (0, 1);
        scattered = Ray (record.hit_point, r.direction.reflect (normal).unit ());

        return true;
}

void Phong::add_light (Light *light)
{
        this->sources.push_back (light);
}