#include "plane.hpp"
#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"

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

Plane::Plane (Vec3 location, Vec3 normal, Material *material) : Object (location, material), normal (normal)
{
        // compute orthonormal basis vectors for plane
        this->u = findVectorOnPlane (normal, location).unit ();
        this->v = normal.cross (u).unit ();
}

Vec3 Plane::to_texture_uv (Vec3 point)
{
        double alpha = normal.dot (v.cross (point - location)) / normal.dot(v.cross(u));

        double beta = normal.dot(u.cross(point - location)) / normal.dot(u.cross(v));

        return Vec3(alpha, beta, 0);
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