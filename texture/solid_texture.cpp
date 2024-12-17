#include "solid_texture.hpp"
#include "vec3.hpp"

SolidTexture::SolidTexture (Vec3 color) : color (color)
{
}

Vec3 SolidTexture::photon_map (Vec3 point)
{
        return Vec3::zero ();
}

Vec3 SolidTexture::read_texture_uv (Vec3 _, Vec3 __)
{
        return this->color;
}

Vec3 SolidTexture::read_rgb255 (Vec3 _)
{
        return this->color * 255.999;
}