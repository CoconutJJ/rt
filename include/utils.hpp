#pragma once
#include "mat3.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "material.hpp"
#include "triangle.hpp"
double clamp (double min, double x, double max);
double random_double (double min, double max);
double deg2rad (double deg);
Vec3 random_in_unit_disk ();
bool hit_box (Vec3 point,
              Vec3 u,
              Vec3 v,
              double u_length,
              double v_length,
              Ray r,
              double &alpha,
              double &beta,
              double &lambda);
void log_error (const char * message, ...);
void log_warn (const char * message, ...);
void log_info (const char * message, ...);
Mat3 texture_projection_matrix (Vec3 xy1, Vec3 xy2, Vec3 xy3, Vec3 uv1, Vec3 uv2, Vec3 uv3);
std::vector<Triangle *> load_obj_mesh (char *obj_filename, Material *material);
Vec3 compute_mesh_centroid (std::vector<Triangle *> mesh_triangles);
double difference_of_products (double a, double b, double c, double d);
double sum_of_products (double a, double b, double c, double d);
bool nearlyEqual(double a, double b);