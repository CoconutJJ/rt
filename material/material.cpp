#include "material.hpp"
#include "mat3.hpp"
#include "texture.hpp"
#include "vec3.hpp"
Material::Material () : texture (nullptr), normal_map (nullptr)
{
}
Material::Material (Texture *texture, Texture *normal_map) : texture (texture), normal_map (normal_map)
{
}

Vec3 Material::normal (Mat3 tbn, Vec3 n, Vec3 uv)
{
        Vec3 normal = this->normal_map->read_rgb255 (uv);

        normal = normal * 2.0 / 255 + Vec3 (-1, -1, -1);

        return tbn * normal;
}
