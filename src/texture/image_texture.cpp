#include "vec3.hpp"
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "image_texture.hpp"
#include "lib/stb_image.hpp"

ImageTexture::ImageTexture (const char *image_filename)
{
        this->pixels = (struct rgb_pixel *)stbi_load (
                image_filename, &this->image_width, &this->image_height, &this->image_channels, 3);

        if (!this->pixels) {
                std::cout << "Attempting to load texture " << image_filename << " failed: " << strerror (errno);
                exit (EXIT_FAILURE);
        }
}

Vec3 ImageTexture::photon_map (Vec3 point)
{
        return Vec3::zero ();
}

Vec3 ImageTexture::read_texture_uv (Vec3 uv, Vec3 point)
{
        Vec3 color = this->read_rgb255 (uv) / 255.0;

        return color * color;
}

Vec3 ImageTexture::read_rgb255 (Vec3 uv)
{
        int u = int (std::floor (uv.x * image_width));
        int v = int (std::floor (uv.y * image_height));

        struct rgb_pixel pixel = this->pixels[v * image_width + u];

        return Vec3 (pixel.red, pixel.green, pixel.blue);
}
