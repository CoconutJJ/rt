#include "plane.hpp"
#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"
#include <stdexcept>

Vec3 findVectorOnPlane (Vec3 normal, Vec3 point)
{
        if (normal.x != 0)
                return Vec3 (
                        (normal.y * (1 - point.y) + normal.z * (1 - point.z)) / -point.x, 1 - point.y, 1 - point.z);
        else if (normal.y != 0)
                return Vec3 (
                        1 - point.x, (normal.x * (1 - point.x) + normal.z * (1 - point.z)) / -point.y, 1 - point.z);
        else if (normal.z != 0)
                return Vec3 (
                        1 - point.x, 1 - point.y, (normal.x * (1 - point.x) + normal.y * (1 - point.y)) / -point.z);
        else
                return Vec3 (0, 0, 0);
}

Plane::Plane (Vec3 location, Vec3 normal, Material *material) : Object (location, material), n (normal)
{
        // compute orthonormal basis vectors for plane
        this->u = findVectorOnPlane (normal, location).unit ();
        this->v = normal.cross (u).unit ();
}

Plane::Plane (Vec3 location, Vec3 u, Vec3 v, Material *material) : Object (location, material)
{
        this->u = u.unit ();
        this->v = v.unit ();
        this->n = u.cross (v);
}

Vec3 Plane::to_uv (Vec3 point)
{
        double alpha = n.dot (v.cross (point - location)) / n.dot (v.cross (u));

        double beta = n.dot (u.cross (point - location)) / n.dot (u.cross (v));

        return Vec3 (alpha, beta, 0);
}

Vec3 Plane::tangent (Vec3 point)
{
        throw std::logic_error ("Not implemented!");
}

Vec3 Plane::normal (Vec3 point)
{
        return this->n.unit ();
}

bool Plane::hit_point (Ray r, Vec3 &point, double &lambda)
{
        double top = (this->location - r.origin).dot (this->n);
        double bottom = (r.direction.dot (this->n));

        if (bottom == 0.0)
                return false;

        double t = top / bottom;

        if (t < 0)
                return false;

        lambda = t;
        point = r.at (t);

        return true;
}

bool Plane::hit (Ray r, HitRecord &record)
{
        if (!this->hit_point (r, record.hit_point, record.lambda))
                return false;

        record.mat = this->mat;
        record.setNormal (r, this->mapped_normal (record.hit_point));
        record.obj = this;
        return true;
}