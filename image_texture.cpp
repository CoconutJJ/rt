#include "vec3.hpp"
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "image_texture.hpp"
#include "stb_image.h"

ImageTexture::ImageTexture (const char *image_filename)
{
        int width, height, n;
        this->pixels = (struct rgb_pixel *)stbi_load (
                image_filename, &this->image_width, &this->image_height, &this->image_channels, 3);
}

Vec3 ImageTexture::read_texture_uv (Vec3 uv)
{
        int u = int (std::floor (uv.x * image_width));
        int v = int (std::floor (uv.y * image_height));

        struct rgb_pixel pixel = this->pixels[v * image_width + u];

        return Vec3 (pixel.red / 255.0, pixel.green / 255.0, pixel.blue / 255.0);
}
