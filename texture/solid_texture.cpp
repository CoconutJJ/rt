#include "solid_texture.hpp"
#include "vec3.hpp"

SolidTexture::SolidTexture (Vec3 color) : color (color)
{
    
}

Vec3 SolidTexture::read_texture_uv (Vec3 _)
{
        return this->color;
}