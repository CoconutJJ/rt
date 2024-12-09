#include "triangle.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Triangle::Triangle (Vec3 point, Vec3 u, Vec3 v, Material *material) : Plane (point, u, v, material)
{
}

bool Triangle::inside (Vec3 point)
{
        Vec3 v1 = u, v2 = v - u, v3 = -v;
        Vec3 p1 = this->location, p2 = p1 + v1, p3 = p2 + v2;
        double d1 = v1.cross (point - p1).dot (this->n), d2 = v2.cross (point - p2).dot (this->n),
               d3 = v3.cross (point - p3).dot (this->n);

        bool upward_orientation = d1 > 0 && d2 > 0 && d3 > 0;

        bool downward_orientation = d1 < 0 && d2 < 0 && d3 < 0;

        return upward_orientation || downward_orientation;
}

bool Triangle::hit (Ray r, HitRecord &record)
{
        if (!this->hit_point (r, record.hit_point, record.lambda))
                return false;

        if (!this->inside (record.hit_point))
                return false;

        record.mat = this->mat;
        record.setNormal (r, this->mapped_normal (record.hit_point));
        record.obj = this;
        
        return true;
}
