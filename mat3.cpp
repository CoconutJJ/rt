#include "mat3.hpp"
#include "vec3.hpp"

Mat3::Mat3 (Vec3 c1, Vec3 c2, Vec3 c3) : c1 (c1), c2 (c2), c3 (c3)
{
}

Vec3 Mat3::operator* (Vec3 b)
{
        return this->c1 * b.x + this->c2 * b.y + this->c3 * b.z;
}

Mat3 Mat3::operator* (Mat3 B)
{
        return Mat3 (*this * B.c1, *this * B.c2, *this * B.c3);
}

Mat3 Mat3::transpose ()
{
        return Mat3 (Vec3 (this->c1.x, this->c2.x, this->c3.x),
                     Vec3 (this->c1.y, this->c2.y, this->c3.y),
                     Vec3 (this->c1.z, this->c2.z, this->c3.z));
}