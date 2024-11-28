#include "vec3.hpp"
#include "utils.hpp"
#include <cmath>
#include <ostream>

Vec3::Vec3 ()
{
}

Vec3::Vec3 (double x, double y, double z) : x (x), y (y), z (z)
{
        return;
}

std::ostream &operator<< (std::ostream &out, Vec3 &v)
{
        return out << v.x << ' ' << v.y << ' ' << v.z;
}

Vec3 Vec3::operator+ (Vec3 a)
{
        return Vec3 (this->x + a.x, this->y + a.y, this->z + a.z);
}

Vec3 Vec3::operator+ (double d)
{
        return Vec3 (this->x + d, this->y + d, this->z + d);
}

Vec3 Vec3::operator- ()
{
        return Vec3 (-this->x, -this->y, -this->z);
}

Vec3 Vec3::operator- (Vec3 a)
{
        return Vec3 (this->x - a.x, this->y - a.y, this->z - a.z);
}
Vec3 Vec3::operator* (double d)
{
        return Vec3 (this->x * d, this->y * d, this->z * d);
}

Vec3 Vec3::operator* (Vec3 b)
{
        return Vec3 (this->x * b.x, this->y * b.y, this->z * b.z);
}

Vec3 Vec3::operator/ (double d)
{
        return Vec3 (this->x / d, this->y / d, this->z / d);
}

Vec3 Vec3::operator/= (double d)
{
        *this = *this / d;

        return *this;
}

Vec3 Vec3::operator+= (Vec3 b)
{
        *this = *this + b;

        return *this;
}

Vec3 Vec3::operator+= (double d)
{
        *this = *this + d;

        return *this;
}

Vec3 Vec3::operator-= (Vec3 b)
{
        *this = *this - b;

        return *this;
}

Vec3 Vec3::operator*= (double d)
{
        *this = *this * d;

        return *this;
}

bool Vec3::operator== (Vec3 b)
{
        return (this->x == b.x) && (this->y == b.y) && (this->z == b.z);
}

double Vec3::dot (Vec3 b)
{
        return this->x * b.x + this->y * b.y + this->z * b.z;
}

Vec3 Vec3::cross (Vec3 b)
{
        return Vec3 (this->y * b.z - this->z * b.y, this->z * b.x - this->x * b.z, this->x * b.y - this->y * b.x);
}

double Vec3::length_squared ()
{
        return this->x * this->x + this->y * this->y + this->z * this->z;
}

double Vec3::length ()
{
        return std::sqrt (this->length_squared ());
}

Vec3 Vec3::unit ()
{
        return *this / this->length ();
}

Vec3 Vec3::clamp (double min, double max)
{
        return Vec3 (::clamp (min, this->x, max), ::clamp (min, this->y, max), ::clamp (min, this->z, max));
}

Vec3 Vec3::random ()
{
        while (true) {
                Vec3 v = Vec3 (random_double (-1, 1), random_double (-1, 1), random_double (-1, 1));

                if (v.length_squared () > 1e-160 && v.length_squared () < 1.0)
                        return v;
        }
}

Vec3 Vec3::reflect (Vec3 normal)
{
        return *this - normal * (2 * this->dot (normal));
}

bool Vec3::near_zero ()
{
        return std::fabs (this->x) < 1e-8 && std::fabs (this->y) < 1e-8 && std::fabs (this->z) < 1e-8;
}

Vec3 Vec3::refract (Vec3 n, double mu)
{
        Vec3 perp = (*this + n * (std::fmin (-(this->dot (n)), 1.0))) * mu;
        Vec3 parallel = -n * std::sqrt (std::fabs (1.0 - perp.length_squared ()));

        return perp + parallel;
}

Vec3 Vec3::elem_mul (Vec3 b)
{
        return Vec3 (this->x * b.x, this->y * b.y, this->z * b.z);
}

Vec3 Vec3::zero ()
{
        return Vec3 (0, 0, 0);
}