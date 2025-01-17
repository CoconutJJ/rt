#include "utils.hpp"
#include "mat3.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "termcolor.hpp"
#include "triangle.hpp"
#include "vec3.hpp"

#include "lib/rply.h"
#include "lib/rplyfile.h"

#include <cstddef>
#include <cstdio>
#include <stdexcept>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <cmath>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
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

double difference_of_products (double a, double b, double c, double d)
{
        double cd = c * d;

        double err = fma (c, d, -cd);

        return fma (a, b, -cd) + err;
}

double sum_of_products (double a, double b, double c, double d)
{
        double cd = c * d;

        double err = fma (c, d, -cd);

        return fma (a, b, cd) - err;
}

Vec3 random_in_unit_disk ()
{
        double x = random_double (0, 1);

        double y =
                random_double (0, 1) < 0.5 ? sqrt (random_double (0, 1 - x * x)) : -sqrt (random_double (0, 1 - x * x));

        return Vec3 (x, y, 0);
}

void log_error (std::string message)
{
        std::cerr << termcolor::red << "ERROR: " << termcolor::reset << message << std::endl;
}

void log_warn (std::string message)
{
        std::cerr << termcolor::yellow << "WARNING: " << termcolor::reset << message << std::endl;
}

void log_info (std::string message)
{
        std::cerr << termcolor::green << "INFO: " << termcolor::reset << message << std::endl;
}

Mat3 rotateX (double deg)
{
        double rad = deg2rad (deg);

        return Mat3 (Vec3 (1, 0, 0), Vec3 (0, cos (rad), sin (rad)), Vec3 (0, -sin (rad), cos (rad)));
}

Mat3 rotateY (double deg)
{
        double rad = deg2rad (deg);

        return Mat3 (Vec3 (cos (rad), 0, -sin (rad)), Vec3 (0, 1, 0), Vec3 (sin (rad), 0, cos (rad)));
}

Mat3 rotateZ (double deg)
{
        double rad = deg2rad (deg);

        return Mat3 (Vec3 (cos (rad), sin (rad), 0), Vec3 (-sin (rad), cos (rad), 0), Vec3 (0, 0, 1));
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

        return (0 <= _alpha && _alpha <= u_length) && (0 <= _beta && _beta <= v_length);
}

Vec3 compute_mesh_centroid (std::vector<Triangle *> mesh_triangles)
{
        Vec3 centroid (0, 0, 0);
        double total_area = 0;

        for (Triangle *t : mesh_triangles) {
                Vec3 center = t->center ();
                double area = t->area ();

                centroid += area * center;
                total_area += area;
        }

        centroid /= total_area;

        return centroid;
}

std::vector<Triangle *> load_obj_mesh (char *obj_filename, Material *material)
{
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shape;
        std::vector<tinyobj::material_t> mats;
        std::string warn, err;
        std::vector<Triangle *> triangles;

        if (!tinyobj::LoadObj (&attrib, &shape, &mats, &warn, &err, obj_filename, NULL, true, true))
                throw std::runtime_error (warn + err);

        auto load_vertex = [attrib] (tinyobj::index_t idx) -> Vec3 {
                size_t x = 3 * idx.vertex_index, y = 3 * idx.vertex_index + 1, z = 3 * idx.vertex_index + 2;

                return Vec3 (attrib.vertices[x], attrib.vertices[y], attrib.vertices[z]);
        };

        auto load_vertex_normal = [attrib] (tinyobj::index_t idx) -> Vec3 {
                if (idx.normal_index == -1)
                        return Vec3::zero ();

                size_t x = 3 * idx.normal_index, y = 3 * idx.normal_index + 1, z = 3 * idx.normal_index + 2;

                return Vec3 (attrib.normals[x], attrib.normals[y], attrib.normals[z]);
        };

        for (auto sh : shape)
                for (auto fv : sh.mesh.num_face_vertices)
                        assert (fv == 3);

        for (auto sh : shape)
                for (size_t triangle_index = 0; triangle_index < sh.mesh.indices.size (); triangle_index += 3) {
                        tinyobj::index_t p1 = sh.mesh.indices[triangle_index];
                        tinyobj::index_t p2 = sh.mesh.indices[triangle_index + 1];
                        tinyobj::index_t p3 = sh.mesh.indices[triangle_index + 2];

                        Vec3 approx_normal =
                                (load_vertex_normal (p1) + load_vertex_normal (p2) + load_vertex_normal (p3)) / 3;

                        Vec3 v1 = load_vertex (p1), v2 = load_vertex (p2), v3 = load_vertex (p3);

                        Vec3 normal = (v2 - v1).cross (v3 - v1).unit ();

                        if (normal.dot (approx_normal) < 0)
                                normal = -normal;

                        Triangle *tri = new Triangle (v1, v2, v3, normal, material);

                        triangles.push_back (tri);
                }

        return triangles;
}

// std::vector<Triangle *> load_ply_mesh (char *ply_file_name, Material *material)
// {

//         p_ply ply = ply_open(ply_file_name, nullptr, 0, nullptr);

//         if (!ply)
//                 throw std::logic_error("Could not open ply file");

//         if (!ply_read_header(ply))
//                 throw std::logic_error("Could not open ply file");

//         long nverts, ntris;

//         ply_set_read_cb(ply, const char *element_name, const char *property_name, p_ply_read_cb read_cb, void *pdata,
//         long idata)

// }

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
