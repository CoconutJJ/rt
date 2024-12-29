#include "sphere.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

Sphere::Sphere (Vec3 center, double radius, Material *material) : SmoothObject (center, material)
{
        this->radius = radius;
}

Sphere::Sphere (Vec3 from, Vec3 to, double radius, Material *material) : SmoothObject (from, to, material)
{
        this->radius = radius;
}

/**
        Computes the tangent vector given a point on the sphere in spherical
        coordinates.

        The tangent vector is oriented horizontally according to world
        coordinates.

        Spherical coordinates in our world space is:

        x = p sin(phi) cos(theta),
        y = p cos(phi),
        z = -p sin(phi) sin(theta)

        Our coordinate system:

        +y   -z
        |   /
        |  /
        | /
        |/_______ +x
*/
Vec3 Sphere::tangent (double theta, double phi)
{
        return Vec3 (-this->radius * std::sin (phi) * std::sin (theta),
                     0,
                     -this->radius * std::sin (phi) * std::cos (theta))
                .unit ();
}

Vec3 Sphere::tangent (Vec3 point)
{
        Vec3 sphere_coord = this->spherical_coord (point);

        return this->tangent (sphere_coord.y, sphere_coord.z);
}

Vec3 Sphere::normal (Vec3 point)
{
        return (point - this->location).unit ();
}

Vec3 Sphere::spherical_coord (Vec3 point)
{
        point = point - this->location;

        double theta = this->argument (-point.z, point.x);

        double phi = std::acos (point.y / point.length ());

        return Vec3 (point.length (), theta, phi);
}

bool Sphere::hit (Ray r, HitRecord &record)
{
        Vec3 current_center = this->location_at_time (r.time);

        double a = r.direction.dot (r.direction);
        double b = r.direction.dot (r.origin - current_center) * 2.0;
        double c = (current_center - r.origin).dot (current_center - r.origin) - this->radius * this->radius;

        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0)
                return false;

        record.lambda = (-b - std::sqrt (discriminant)) / (2 * a);

        if (record.lambda < 0) {
                record.lambda = (-b + sqrt (discriminant)) / (2 * a);

                if (record.lambda < 0)
                        return false;
        }

        record.hit_point = r.at (record.lambda);
        record.setNormal (r, this->mapped_normal (record.hit_point));
        record.uv = this->to_uv (record.hit_point);
        record.object = this;
        return true;
}

double Sphere::argument (double y_opp, double x_adj)
{
        double y = std::abs (y_opp);
        double x = std::abs (x_adj);

        if (x_adj > 0 && y_opp >= 0)
                return std::atan (y / x);
        else if (x_adj < 0 && y_opp >= 0)
                return M_PI - std::atan (y / x);
        else if (x_adj < 0 && y_opp <= 0)
                return M_PI + std::atan (y / x);
        else
                return 2 * M_PI - std::atan (y / x);
}
Vec3 Sphere::to_uv (Vec3 point)
{
        Vec3 sphere_coord = this->spherical_coord (point);

        double theta = sphere_coord.y;
        double phi = sphere_coord.z;

        double v = phi / M_PI;

        double u = theta / (2 * M_PI);

        return Vec3 (u, v, 0);
}

Vec3 Sphere::sample_point ()
{
        return this->location +
               Vec3 (this->radius, random_double (0, 2 * M_PI), random_double (0, M_PI / 2)).sph_inv ();
}

double Sphere::area ()
{
        return 4 * M_PI * this->radius * this->radius;
}