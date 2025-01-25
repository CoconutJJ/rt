#include "vec3.hpp"
#include "utils.hpp"
#include <cfloat>
#include <cmath>
#include <ostream>
#include <simd/matrix.h>
#include <stdexcept>

#define ACCELERATE_NEW_LAPACK
#include <simd/geometry.h>
#include <simd/simd.h>
#include <simd/vector_make.h>
#include <vecLib/vecLib.h>

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

Vec3 &Vec3::operator= (Vec3 other)
{
        this->vec = other.vec;

        return *this;
}

std::ostream &operator<< (std::ostream &out, Vec3 v)
{
        return out << v[0] << ",\t" << v[1] << ",\t" << v[2];
}

Vec3 Vec3::operator+ (Vec3 a)
{
        return Vec3 (this->vec + a.vec);
}

Vec3 Vec3::operator+ (double d)
{
        return Vec3 (this->vec + simd_make_double3 (d, d, d));
}

Vec3 Vec3::operator- ()
{
        return Vec3 (-this->vec);
}

Vec3 Vec3::operator- (Vec3 a)
{
        return Vec3 (this->vec - a.vec);
}
Vec3 Vec3::operator* (double d)
{
        return Vec3 (d * this->vec);
}

Vec3 operator* (double scalar, Vec3 a)
{
        return a * scalar;
}

Vec3 Vec3::operator* (Vec3 b)
{
        return Vec3 (this->vec * b.vec);
}

Vec3 Vec3::operator/ (double d)
{
        return Vec3 (this->vec / d);
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
        return simd_equal (this->vec, b.vec);
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

double Vec3::dot (Vec3 b)
{
        return simd_dot (this->vec, b.vec);
}

Vec3 Vec3::cross (Vec3 b)
{
        return Vec3 (simd_cross (this->vec, b.vec));
}

double Vec3::length_squared ()
{
        return simd_length_squared (this->vec);
}

double Vec3::length ()
{
        return simd_length (this->vec);
}

Vec3 Vec3::unit ()
{
        return Vec3 (simd_normalize (this->vec));
}

Vec3 Vec3::clamp (double min, double max)
{
        return Vec3 (
                ::clamp (min, this->vec[0], max), ::clamp (min, this->vec[1], max), ::clamp (min, this->vec[2], max));
}

Vec3 Vec3::random ()
{
        return Vec3 (random_double (-1, 1), random_double (-1, 1), random_double (-1, 1)).unit ();
}

Vec3 Vec3::reflect (Vec3 normal)
{
        return Vec3 (simd_reflect (this->vec, simd_normalize (normal.vec)));
}

bool Vec3::near_zero ()
{
        return std::fabs (this->vec[0]) < 1e-8 && std::fabs (this->vec[1]) < 1e-8 && std::fabs (this->vec[2]) < 1e-8;
}

Vec3 Vec3::refract (Vec3 n, double mu)
{
        return Vec3 (simd_refract (this->vec, n.vec, mu));
}

Vec3 Vec3::zero ()
{
        return Vec3 (0, 0, 0);
}

Vec3 Vec3::rotate (Vec3 axis, double angle)
{
        // Rodrigues' formula
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
        double theta = this->argument (-this->vec[2], this->vec[0]);

        double phi = std::acos (this->vec[1] / this->length ());

        return Vec3 (this->length (), theta, phi);
}

Vec3 Vec3::sph_inv ()
{
        double p = this->vec[0], phi = this->vec[2], theta = this->vec[1];

        return Vec3 (p * sin (phi) * cos (theta), p * cos (phi), p * sin (phi) * sin (theta));
}

Vec3 Vec3::inf ()
{
        return Vec3 (DBL_MAX, DBL_MAX, DBL_MAX);
}

double Vec3::operator[] (int index) const
{
        if (index < 0 || index > 2)
                throw std::logic_error ("error: invalid dimension!");

        return this->vec[index];
}
