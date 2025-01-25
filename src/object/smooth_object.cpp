#include "smooth_object.hpp"
#include "hitrecord.hpp"
#include "mat3.hpp"
#include "material.hpp"
#include "object.hpp"
#include "vec3.hpp"

SmoothObject::SmoothObject (Vec3 location, Material *material) : Object (location, material)
{
}

SmoothObject::SmoothObject (Vec3 location1, Vec3 location2, Material *material)
        : Object (location1, location2, material)
{
}

SmoothObject::SmoothObject (Vec3 location1, Vec3 location2, Material *material, Texture *normal_map)
        : Object (location1, location2, material)
{
        this->material->normal_map = normal_map;
}

bool SmoothObject::is_light_source ()
{
        return false;
}

Vec3 SmoothObject::mapped_normal (Vec3 point)
{
        if (!this->material->normal_map)
                return this->normal (point);

        return this->material->normal (this->tbn (point), this->normal (point), this->to_uv (point));
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
Mat3 SmoothObject::tnb (HitRecord &record)
{
        Vec3 tangent = this->tangent (record.hit_point).unit();
        Vec3 normal = this->normal(record.hit_point).unit();
        Mat3 tnb (tangent, normal, normal.cross (tangent).unit());

        return tnb;
}

Vec3 SmoothObject::tbn_transform (Vec3 point, Vec3 tangent_v)
{
        return this->tbn (point) * tangent_v;
}
