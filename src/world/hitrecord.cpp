#include "hitrecord.hpp"
#include "vec3.hpp"
#include <iostream>

HitRecord::HitRecord () : hit_point (0, 0, 0), normal (0, 0, 0)
{
}

void HitRecord::setNormal (Ray r, Vec3 outward_normal)
{
        Vec3 opposing_normal;
        if (r.direction.dot (outward_normal) > 0) {
                opposing_normal = -outward_normal;
                this->front_face = false;
        } else {
                this->front_face = true;
                opposing_normal = outward_normal;
        }

        this->normal = opposing_normal.unit ();
}

Vec3 HitRecord::outward_normal ()
{
        return this->front_face ? this->normal : -this->normal;
}