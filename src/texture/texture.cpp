#include "texture.hpp"
#include "vec3.hpp"
#include <cstddef>

Texture::Texture ()
{
        this->photon_map_height = 400;
        this->photon_map_width = 400;
        this->photon_texture = new Vec3[this->photon_map_height * this->photon_map_width];
}
Texture::~Texture ()
{
        delete[] this->photon_texture;
}
void Texture::store_photon (Vec3 uv, Vec3 color)
{
        size_t x = size_t (uv[0] * this->photon_map_width);
        size_t y = size_t (uv[1] * this->photon_map_height);

        this->photon_texture[y * this->photon_map_width + x] += color;
}