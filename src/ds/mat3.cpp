#include "mat3.hpp"
#include "vec3.hpp"

#ifdef USE_ACCELERATE
#include <simd/matrix.h>
#include <simd/simd.h>
#include <simd/types.h>
#endif

Mat3::Mat3 ()
{
}

Mat3::Mat3 (Vec3 c1, Vec3 c2, Vec3 c3) : c1 (c1), c2 (c2), c3 (c3)
{
#ifdef USE_ACCELERATE
        this->mat = simd_matrix (c1.vec, c2.vec, c3.vec);
#endif
}

#ifdef USE_ACCELERATE
Mat3::Mat3 (simd_double3x3 mat)
{
        this->mat = mat;
}
#endif

Mat3::Mat3 (const Mat3 &other)
{
#ifdef USE_ACCELERATE
        this->mat = other.mat;
#else
        this->c1 = other.c1;
        this->c2 = other.c2;
        this->c3 = other.c3;
#endif
}

Mat3 &Mat3::operator= (Mat3 other)
{
#ifdef USE_ACCELERATE
        this->mat = other.mat;
#else
        this->c1 = other.c1;
        this->c2 = other.c2;
        this->c3 = other.c3;
#endif
        return *this;
}

Vec3 Mat3::operator* (Vec3 b)
{
#ifdef USE_ACCELERATE
        return Vec3 (simd_mul (this->mat, b.vec));
#else
        return this->c1 * b.x + this->c2 * b.y + this->c3 * b.z;
#endif
}

Mat3 Mat3::operator* (Mat3 B)
{
#ifdef USE_ACCELERATE
        return Mat3 (simd_mul (this->mat, B.mat));
#else
        return Mat3 (*this * B.c1, *this * B.c2, *this * B.c3);
#endif
}

Mat3 Mat3::operator* (double d)
{
#ifdef USE_ACCELERATE
        return Mat3 (simd_mul (d, this->mat));
#else
        return Mat3 (this->c1 * d, this->c2 * d, this->c3 * d);
#endif
}

Mat3 Mat3::transpose ()
{
#ifdef USE_ACCELERATE
        return Mat3 (simd_transpose (this->mat));
#else
        return Mat3 (Vec3 (this->c1.x, this->c2.x, this->c3.x),
                     Vec3 (this->c1.y, this->c2.y, this->c3.y),
                     Vec3 (this->c1.z, this->c2.z, this->c3.z));
#endif
}

std::ostream &operator<< (std::ostream &out, Mat3 m)
{
        m = m.transpose ();
        out << m.c1 << '\n';
        out << m.c2 << '\n';
        out << m.c3 << '\n';
        return out;
}
