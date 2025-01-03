#include "triangle.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "utils.hpp"
#include "vec3.hpp"

Triangle::Triangle (Vec3 point, Vec3 u, Vec3 v, Material *material) : SmoothObject (point, material)
{
        this->u = u;
        this->v = v;
        this->n = u.cross (v);
}

Triangle::Triangle (Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, Material *material) : SmoothObject (p1, material)
{
        this->u = p2 - p1;
        this->v = p3 - p1;
        this->n = normal;
}

Triangle::Triangle (const Triangle &triangle) : SmoothObject (triangle)
{
        this->u = triangle.u;
        this->v = triangle.v;
        this->n = triangle.n;

        this->t1 = triangle.t1;
        this->t2 = triangle.t2;
        this->t3 = triangle.t3;

        this->T = triangle.T;
}

Triangle &Triangle::operator= (Triangle &triangle)
{
        this->u = triangle.u;
        this->v = triangle.v;
        this->n = triangle.n;

        this->t1 = triangle.t1;
        this->t2 = triangle.t2;
        this->t3 = triangle.t3;

        this->T = triangle.T;

        return *this;
}

void Triangle::center (Vec3 point)
{
        Vec3 center = (3 * this->location + this->u + this->v) / 3;

        Vec3 delta = point - center;

        this->location += delta;
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
        double alpha, beta, lambda;

        if (!hit_box (this->location, this->u, this->v, 1, 1, r, alpha, beta, lambda))
                return false;

        Vec3 hit_point = r.at (lambda);

        if (!this->inside (hit_point))
                return false;
        
        record.lambda = lambda;
        record.hit_point = hit_point;
        record.uv = this->to_uv (record.hit_point);
        record.setNormal (r, this->mapped_normal (record.hit_point));
        record.object = this;

        return true;
}

void Triangle::load_texture_coordinates (Vec3 t1, Vec3 t2, Vec3 t3)
{
        this->t1 = t1;
        this->t2 = t2;
        this->t3 = t3;
        this->T = texture_projection_matrix (
                this->location, this->location + this->u, this->location + this->v, t1, t2, t3);
}

Vec3 Triangle::to_uv (Vec3 point)
{
        return this->T * (point - this->location) + this->t1;
}

Vec3 Triangle::tangent (Vec3 point)
{
        return this->u;
}

Vec3 Triangle::normal (Vec3 point)
{
        return this->n;
}

Vec3 Triangle::sample_point ()
{
        double alpha = random_double (0, 1), beta = random_double (0, 1);

        Vec3 point = alpha * this->u + beta * this->v;

        if (this->inside (this->location + point))
                return point;

        Vec3 midpoint = (u - v) / 2;

        return this->location + point + 2 * (midpoint - point);
}

double Triangle::area ()
{
        return this->u.cross (this->v).length ();
}