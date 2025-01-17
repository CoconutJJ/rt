#include "triangle.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <vector>

Triangle::Triangle (Vec3 point, Vec3 u, Vec3 v, Material *material)
        : SmoothObject (point, material), p2 (point + u), p3 (point + v), n (u.cross (v))
{
}

Triangle::Triangle (Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, Material *material)
        : SmoothObject (p1, material), p2 (p2), p3 (p3), n (normal)
{
}

Triangle::Triangle (const Triangle &triangle) : SmoothObject (triangle)
{
        this->p2 = triangle.p2;
        this->p3 = triangle.p3;
        this->t1 = triangle.t1;
        this->t2 = triangle.t2;
        this->t3 = triangle.t3;
        this->T = triangle.T;
}

Triangle &Triangle::operator= (Triangle &triangle)
{
        this->p2 = triangle.p2;
        this->p3 = triangle.p3;
        this->t1 = triangle.t1;
        this->t2 = triangle.t2;
        this->t3 = triangle.t3;
        this->T = triangle.T;

        return *this;
}

std::vector<Vec3> Triangle::verticies ()
{
        return std::vector<Vec3> ({ this->location, p2, p3 });
}

inline Vec3 Triangle::_u ()
{
        return p2 - this->location;
}

inline Vec3 Triangle::_v ()
{
        return p3 - this->location;
}

void Triangle::center (Vec3 point)
{
        Vec3 delta = point - this->center ();

        this->location += delta;
}

Vec3 Triangle::center ()
{
        return (this->location + p2 + p3) / 3;
}

bool Triangle::inside (Vec3 point)
{
        Vec3 v1 = _u (), v2 = _v () - _u (), v3 = -_v ();
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

        if (!hit_box (this->location, this->_u (), this->_v (), 1, 1, r, alpha, beta, lambda))
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
                this->location, this->location + this->_u (), this->location + this->_v (), t1, t2, t3);
}

Vec3 Triangle::to_uv (Vec3 point)
{
        return this->T * (point - this->location) + this->t1;
}

Vec3 Triangle::tangent (Vec3 point)
{
        return this->_u ();
}

Vec3 Triangle::normal (Vec3 point)
{
        return this->n;
}

Vec3 Triangle::sample_point ()
{
        double alpha = random_double (0, 1), beta = random_double (0, 1);

        Vec3 point = alpha * _u () + beta * _v ();

        if (this->inside (this->location + point))
                return point;

        Vec3 midpoint = (_u () - _v ()) / 2;

        return this->location + point + 2 * (midpoint - point);
}

double Triangle::area ()
{
        return _u ().cross (_v ()).length () / 2;
}

Triangle *Triangle::translate (Vec3 v)
{
        this->location += v;
        this->p2 += v;
        this->p3 += v;

        return this;
}

Triangle *Triangle::scale (double s)
{
        this->location *= s;
        this->p2 *= s;
        this->p3 *= s;

        return this;
}