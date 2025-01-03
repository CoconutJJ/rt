#include "utils.hpp"
#include "mat3.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

double clamp (double min, double x, double max)
{
        if (x < min)
                return min;
        if (x > max)
                return max;

        return x;
}

double random_double (double min, double max)
{
        return min + (max - min) * ((double)std::rand () / RAND_MAX);
}

double deg2rad (double deg)
{
        return M_PI / 180.0 * deg;
}

Vec3 random_in_unit_disk ()
{
        while (true) {
                Vec3 p = Vec3::random ();

                if (p.length () < 1)
                        return p;
        }
}

bool hit_box (Vec3 point,
              Vec3 u,
              Vec3 v,
              double u_length,
              double v_length,
              Ray r,
              double &alpha,
              double &beta,
              double &lambda)
{
        Vec3 n = u.cross (v);

        if (std::abs (n.dot (r.direction)) < 1e-8)
                return false;

        double _lambda = n.dot (point - r.origin) / n.dot (r.direction);

        if (_lambda < 0)
                return false;

        Vec3 hit_point = r.at (_lambda);

        double _alpha = v.cross (hit_point - point).dot (n) / v.cross (u).dot (n);
        double _beta = u.cross (hit_point - point).dot (n) / u.cross (v).dot (n);

        lambda = _lambda;
        alpha = _alpha;
        beta = _beta;

        // std::cerr << "AB:" << n << point << r.origin << std::endl;

        return (0 <= _alpha && _alpha <= u_length) && (0 <= _beta && _beta <= v_length);
}

/**
        @brief Compute the texture projection matrix for a triangle

        Computes a matrix T such that the texture coordinates u,v of a point p
        in the triangle are given by `T * (p - xy1) + uv1`.

        @param xy1 The first vertex of the triangle in 3D space
        @param xy2 The second vertex of the triangle in 3D space
        @param xy3 The third vertex of the triangle in 3D space
        @param uv1 The first vertex of the triangle in texture space
        @param uv2 The second vertex of the triangle in texture space
        @param uv3 The third vertex of the triangle in texture space
        @return The texture projection matrix
 */
Mat3 texture_projection_matrix (Vec3 xy1, Vec3 xy2, Vec3 xy3, Vec3 uv1, Vec3 uv2, Vec3 uv3)
{
        Vec3 v1 = xy2 - xy1;
        Vec3 v2 = xy3 - xy1;
        double v1sq = v1.dot (v1);
        double v2sq = v2.dot (v2);
        double v1v2 = v1.dot (v2);
        Vec3 u = uv2 - uv1;
        Vec3 v = uv3 - uv1;

        Mat3 basis = Mat3 (v1, v2, Vec3 (0, 0, 0));

        // bTb_adj is the adjugate of the matrix bTb, where b is the matrix with columns v1 and v2.
        Mat3 bTb_adj = Mat3 (Vec3 (v2sq, -v1v2, 0), Vec3 (-v1v2, v1sq, 0), Vec3 (0, 0, 0)).transpose ();

        Mat3 uv_basis = Mat3 (u, v, Vec3 (0, 0, 0));

        return uv_basis * bTb_adj * basis.transpose () * (1.0 / (v1sq * v2sq - v1v2 * v1v2));
}
