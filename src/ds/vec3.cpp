#include "vec3.hpp"
#include "utils.hpp"
#include <cfloat>
#include <cmath>
#include <ostream>
#include <stdexcept>

#ifdef USE_ACCELERATE
#include <simd/common.h>
#include <simd/geometry.h>
#include <simd/matrix.h>
#include <simd/simd.h>
#include <simd/vector_make.h>
#include <simd/vector_types.h>
#include <vecLib/vecLib.h>
#endif

// ------------------------- Constructors -------------------------

#ifndef USE_ACCELERATE

Vec3::Vec3 () : x (0), y (0), z (0)
{
}

Vec3::Vec3 (const Vec3 &other) : x (other.x), y (other.y), z (other.z)
{
}

Vec3::Vec3 (double x, double y, double z) : x (x), y (y), z (z)
{
}

#else // USE_ACCELERATE

Vec3::Vec3 ()
{
        this->vec = simd_make_double3 (0, 0, 0);
}

Vec3::Vec3 (simd_double3 v)
{
        this->vec = v;
}

Vec3::Vec3 (const Vec3 &other)
{
        this->vec = other.vec;
}

Vec3::Vec3 (double x, double y, double z)
{
        this->vec = simd_make_double3 (x, y, z);
}

#endif

// ------------------------- Assignment -------------------------

Vec3 &Vec3::operator= (Vec3 other)
{
#ifdef USE_ACCELERATE
        this->vec = other.vec;
#else
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
#endif
        return *this;
}

// ------------------------- Stream -------------------------

std::ostream &operator<< (std::ostream &out, Vec3 v)
{
#ifdef USE_ACCELERATE
        return out << v[0] << ",\t" << v[1] << ",\t" << v[2];
#else
        return out << v.x << ",\t" << v.y << ",\t" << v.z;
#endif
}

// ------------------------- Operators -------------------------

Vec3 Vec3::operator+ (Vec3 a)
{
#ifdef USE_ACCELERATE
        return Vec3 (this->vec + a.vec);
#else
        return Vec3 (this->x + a.x, this->y + a.y, this->z + a.z);
#endif
}

Vec3 Vec3::operator+ (double d)
{
#ifdef USE_ACCELERATE
        return Vec3 (this->vec + simd_make_double3 (d, d, d));
#else
        return Vec3 (this->x + d, this->y + d, this->z + d);
#endif
}

Vec3 Vec3::operator- ()
{
#ifdef USE_ACCELERATE
        return Vec3 (-this->vec);
#else
        return Vec3 (-this->x, -this->y, -this->z);
#endif
}

Vec3 Vec3::operator- (Vec3 a)
{
#ifdef USE_ACCELERATE
        return Vec3 (this->vec - a.vec);
#else
        return Vec3 (this->x - a.x, this->y - a.y, this->z - a.z);
#endif
}

Vec3 Vec3::operator* (double d)
{
#ifdef USE_ACCELERATE
        return Vec3 (d * this->vec);
#else
        return Vec3 (this->x * d, this->y * d, this->z * d);
#endif
}

Vec3 operator* (double scalar, Vec3 a)
{
        return a * scalar;
}

Vec3 Vec3::operator* (Vec3 b)
{
#ifdef USE_ACCELERATE
        return Vec3 (this->vec * b.vec);
#else
        return Vec3 (this->x * b.x, this->y * b.y, this->z * b.z);
#endif
}

Vec3 Vec3::operator/ (double d)
{
#ifdef USE_ACCELERATE
        return Vec3 (this->vec / d);
#else
        return Vec3 (this->x / d, this->y / d, this->z / d);
#endif
}

// compound assignment operators are same for both — reuse scalar/vector operators
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

// ------------------------- Comparisons -------------------------

bool Vec3::operator== (Vec3 b)
{
#ifdef USE_ACCELERATE
        return simd_equal (this->vec, b.vec);
#else
        return (this->x == b.x) && (this->y == b.y) && (this->z == b.z);
#endif
}

bool Vec3::operator< (Vec3 b) const
{
        for (int i = 0; i < 3; i++) {
                if ((*this)[i] == b[i])
                        continue;
                if ((*this)[i] < b[i])
                        return true;
                if ((*this)[i] > b[i])
                        return false;
        }
        return false;
}

// ------------------------- Math -------------------------

double Vec3::dot (Vec3 b)
{
#ifdef USE_ACCELERATE
        return simd_dot (this->vec, b.vec);
#else
        return std::fma (this->z, b.z, sum_of_products (this->x, b.x, this->y, b.y));
#endif
}

Vec3 Vec3::cross (Vec3 b)
{
#ifdef USE_ACCELERATE
        return Vec3 (simd_cross (this->vec, b.vec));
#else
        return Vec3 (difference_of_products (this->y, b.z, this->z, b.y),
                     difference_of_products (this->z, b.x, this->x, b.z),
                     difference_of_products (this->x, b.y, this->y, b.x));
#endif
}

double Vec3::length_squared ()
{
#ifdef USE_ACCELERATE
        return simd_length_squared (this->vec);
#else
        return this->dot (*this);
#endif
}

double Vec3::length ()
{
#ifdef USE_ACCELERATE
        return simd_length (this->vec);
#else
        return std::sqrt (this->length_squared ());
#endif
}

Vec3 Vec3::unit ()
{
#ifdef USE_ACCELERATE
        return Vec3 (simd_normalize (this->vec));
#else
        return *this / this->length ();
#endif
}

// ------------------------- Utility -------------------------

Vec3 Vec3::clamp (double min, double max)
{
#ifdef USE_ACCELERATE
        return Vec3 (
                ::clamp (min, this->vec[0], max), ::clamp (min, this->vec[1], max), ::clamp (min, this->vec[2], max));
#else
        return Vec3 (::clamp (min, this->x, max), ::clamp (min, this->y, max), ::clamp (min, this->z, max));
#endif
}

Vec3 Vec3::random ()
{
        return Vec3 (random_double (-1, 1), random_double (-1, 1), random_double (-1, 1)).unit ();
}

Vec3 Vec3::reflect (Vec3 normal)
{
#ifdef USE_ACCELERATE
        return Vec3 (simd_reflect (this->vec, simd_normalize (normal.vec)));
#else
        return *this - normal * (2 * this->dot (normal));
#endif
}

bool Vec3::near_zero ()
{
#ifdef USE_ACCELERATE
        return std::fabs (this->vec[0]) < 1e-8 && std::fabs (this->vec[1]) < 1e-8 && std::fabs (this->vec[2]) < 1e-8;
#else
        return std::fabs (this->x) < 1e-8 && std::fabs (this->y) < 1e-8 && std::fabs (this->z) < 1e-8;
#endif
}

Vec3 Vec3::refract (Vec3 n, double mu)
{
#ifdef USE_ACCELERATE
        return Vec3 (simd_refract (this->vec, n.vec, mu));
#else
        Vec3 l = this->unit ();
        n = -n.unit ();
        return mu * l + (n * sqrt (1 - mu * mu * (1 - (n.dot (l) * n.dot (l))))) - (mu * (n.dot (l)) * n);
#endif
}

Vec3 Vec3::zero ()
{
        return Vec3 (0, 0, 0);
}

Vec3 Vec3::rotate (Vec3 axis, double angle)
{
        return *this * std::cos (angle) + (axis.cross (*this)) * std::sin (angle) +
               axis * (axis.dot (*this)) * (1 - std::cos (angle));
}

Vec3 Vec3::random_hemisphere ()
{
        return Vec3 (1, random_double (0, 2 * M_PI), random_double (0, M_PI / 2)).sph_inv ();
}

double Vec3::argument (double y_opp, double x_adj)
{
        double angle = std::atan2 (y_opp, x_adj);
        return angle < 0 ? 2 * M_PI + angle : angle;
}

Vec3 Vec3::sph ()
{
#ifdef USE_ACCELERATE
        double theta = this->argument (-this->vec[2], this->vec[0]);
        double phi = std::acos (this->vec[1] / this->length ());
        return Vec3 (this->length (), theta, phi);
#else
        double theta = this->argument (-this->z, this->x);
        double phi = std::acos (this->y / this->length ());
        return Vec3 (this->length (), theta, phi);
#endif
}

Vec3 Vec3::sph_inv ()
{
#ifdef USE_ACCELERATE
        double p = this->vec[0], phi = this->vec[2], theta = this->vec[1];
#else
        double p = this->x, phi = this->z, theta = this->y;
#endif
        return Vec3 (p * sin (phi) * cos (theta), p * cos (phi), p * sin (phi) * sin (theta));
}

Vec3 Vec3::inf ()
{
        return Vec3 (DBL_MAX, DBL_MAX, DBL_MAX);
}

// ------------------------- Indexing -------------------------

#ifndef USE_ACCELERATE
double &Vec3::operator[] (int index)
{
        switch (index) {
        case 0: return this->x;
        case 1: return this->y;
        case 2: return this->z;
        }
        throw std::logic_error ("error: invalid dimension!");
}
#endif

double Vec3::operator[] (int index) const
{
#ifdef USE_ACCELERATE
        if (index < 0 || index > 2)
                throw std::logic_error ("error: invalid dimension!");
        return this->vec[index];
#else
        switch (index) {
        case 0: return this->x;
        case 1: return this->y;
        case 2: return this->z;
        }
        throw std::logic_error ("error: invalid dimension!");
#endif
}
