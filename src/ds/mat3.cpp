#include "mat3.hpp"
#include "vec3.hpp"
#include <simd/matrix.h>
#include <simd/simd.h>
#include <simd/types.h>
Mat3::Mat3 ()
{
}
Mat3::Mat3 (Vec3 c1, Vec3 c2, Vec3 c3) : c1 (c1), c2 (c2), c3 (c3)
{
        this->mat = simd_matrix (c1.vec, c2.vec, c3.vec);
}

Mat3::Mat3 (simd_double3x3 mat)
{
        this->mat = mat;
}

Mat3::Mat3 (const Mat3 &other)
{
        this->mat = other.mat;
}

Mat3 &Mat3::operator= (Mat3 other)
{
        this->mat = other.mat;

        return *this;
}

Vec3 Mat3::operator* (Vec3 b)
{
        return Vec3 (simd_mul (b.vec, this->mat));
}

Mat3 Mat3::operator* (Mat3 B)
{
        return Mat3 (simd_mul(this->mat, B.mat));
}

Mat3 Mat3::operator* (double d)
{
        return Mat3 (simd_mul(d, this->mat));
}

Mat3 Mat3::transpose ()
{
        return Mat3 (simd_transpose(this->mat));
}

std::ostream &operator<< (std::ostream &out, Mat3 m)
{
        m = m.transpose ();

        out << m.c1 << '\n';
        out << m.c2 << '\n';
        out << m.c3 << '\n';

        return out;
}