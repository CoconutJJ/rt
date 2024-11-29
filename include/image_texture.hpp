#pragma once

#include "texture.hpp"
#include "vec3.hpp"
#include <cstdint>

class ImageTexture : public Texture {
    public:
        struct rgb_pixel {
                uint8_t red;
                uint8_t green;
                uint8_t blue;
        };
        int image_width;
        int image_height;
        int image_channels;
        struct rgb_pixel *pixels;

        ImageTexture (const char *image_filename);
        Vec3 read_texture_uv (Vec3 uv) override;
        Vec3 read_rgb255(Vec3 uv) override;
};