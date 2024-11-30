#include "quad.hpp"
#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Quad::Quad (Vec3 location, Vec3 v1, Vec3 v2, Material *mat) : Object (location, mat)
{
        this->v1 = v1;
        this->v2 = v2;
}

Vec3 Quad::find_alpha_beta (Vec3 point)
{
        Vec3 n = this->v2.cross (v1);

        n /= n.length_squared ();

        Vec3 plane_vector = point - this->location;

        double alpha = this->v2.cross (plane_vector).dot (n);
        double beta = this->v1.cross (plane_vector).dot (-n);

        return Vec3 (alpha, beta, 0);
}

bool Quad::hit (Ray r, HitRecord &record)
{
        Vec3 normal = this->v1.cross (-this->v2);
        double top = normal.dot (this->location - r.origin);

        double bottom = normal.dot (r.direction);

        if (bottom == 0.0)
                return false;

        double lambda = top / bottom;

        Vec3 hit_point = r.at (lambda);

        Vec3 uv = this->find_alpha_beta (hit_point);

        bool hit = (0 <= uv.x && uv.x <= 1) && (0 <= uv.y && uv.y <= 1);

        if (!hit)
                return false;

        record.hit_point = hit_point;
        record.lambda = lambda;
        record.mat = this->mat;
        record.uv = uv;
        record.setNormal (r, this->mapped_normal (hit_point));

        return true;
}

Vec3 Quad::tangent (Vec3 _)
{
        return this->v1;
}

Vec3 Quad::normal (Vec3 _)
{
        return this->v1.cross (-this->v2);
}

Vec3 Quad::to_uv (Vec3 point)
{
        return find_alpha_beta (point);
}

Vec3 Quad::get_point (double alpha, double beta)
{
        return this->location + this->v1 * alpha + this->v2 * beta;
}
