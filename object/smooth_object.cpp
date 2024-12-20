#include "smooth_object.hpp"
#include "mat3.hpp"
#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <stdexcept>

SmoothObject::SmoothObject (Vec3 location1, Vec3 location2, Material *material)
        : Object (location1, location2, material)
{
}

SmoothObject::SmoothObject (Vec3 location, Material *material) : Object (location, material)
{
}

bool SmoothObject::is_light_source ()
{
        return false;
}

Vec3 SmoothObject::mapped_normal (Vec3 point)
{
        if (!this->mat->normal_map)
                return this->normal (point);

        return this->mat->normal (this->tbn (point), this->normal (point), this->to_uv (point));
}
/**
        Tangent, BiTangent & Normal Matrix:
        This matrix converts tangent space into world space
        It assumes that +z is the tangent direction.
 */
Mat3 SmoothObject::tbn (Vec3 point)
{
        Vec3 tangent = this->tangent (point);
        Vec3 normal = this->normal (point);

        Mat3 tbn (tangent, normal.cross (tangent), normal);

        return tbn;
}
/**
        Tangent, Normal, BiTangent Matrix:
        This matrix converts tangent space into world space.

        Used in path tracer for BRDF unit hemisphere coordinate system calculations.
 */
Mat3 SmoothObject::tnb (Vec3 point)
{
        Vec3 tangent = this->tangent (point);
        Vec3 normal = this->normal (point);

        Mat3 tnb (tangent, normal, normal.cross (tangent));

        return tnb;
}

Vec3 SmoothObject::tbn_transform (Vec3 point, Vec3 tangent_v)
{
        return this->tbn (point) * tangent_v;
}

Vec3 SmoothObject::brdf (Vec3 point, Vec3 in_direction, Vec3 out_direction)
{
        if (!this->mat->brdf)
                throw std::logic_error ("No BRDF specified!");

        return this->mat->brdf->compute (this->tnb (point), in_direction, out_direction);
}