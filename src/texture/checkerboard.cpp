#include "checkerboard.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include <cmath>
#include <stdexcept>

CheckerboardTexture::CheckerboardTexture (Vec3 even_color, Vec3 odd_color)
        : Texture (), even_color (even_color), odd_color (odd_color)
{
}

Vec3 CheckerboardTexture::photon_map (Vec3 point)
{
        return Vec3::zero ();
}

Vec3 CheckerboardTexture::read_texture_uv (Vec3 uv, Vec3 point)
{
        int coord_sum = int (std::floor (point[0]) + std::floor (point[1]) + std::floor (point[2]));

        return coord_sum % 2 ? odd_color : even_color;
}

Vec3 CheckerboardTexture::read_rgb255 (Vec3 uv)
{
        throw std::logic_error ("Not implemented");
}