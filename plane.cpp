#include "plane.hpp"
#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"

Plane::Plane (Vec3 location, Vec3 normal, Material *material) : Object (location, material), normal (normal)
{
}

bool Plane::hit (Ray r, HitRecord &record)
{
        double top = (this->location - r.origin).dot (this->normal);
        double bottom = (r.direction.dot (this->normal));

        if (bottom == 0.0)
                return false;

        double t = top / bottom;

        if (t < 0)
                return false;

        record.hit_point = r.at (t);
        record.lambda = t;
        record.mat = this->mat;
        record.setNormal (r, this->normal);

        return true;
}