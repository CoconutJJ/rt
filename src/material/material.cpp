#include "material.hpp"
#include "hitrecord.hpp"
#include "mat3.hpp"
#include "texture.hpp"
#include "vec3.hpp"

Material::Material ()
{
}

Material::Material (Texture *texture, Texture *normal_map)
        : texture (texture), normal_map (normal_map), emission_value (0, 0, 0)
{
}

Vec3 Material::normal (Mat3 tbn, Vec3 n, Vec3 uv)
{
        if (!this->normal_map)
                return n;

        Vec3 normal = this->normal_map->read_rgb255 (uv);

        normal = normal * 2.0 / 255 + Vec3 (-1, -1, -1);

        return tbn * normal;
}

Vec3 Material::color (HitRecord &record)
{
        return this->texture->read_texture_uv (record.uv, record.hit_point);
}

void Material::emission (Vec3 color)
{
        this->emission_value = color;
}

Vec3 Material::emission ()
{
        return this->emission_value;
}

bool Material::is_emissive ()
{
        return this->emission_value.length_squared () > 0;
}