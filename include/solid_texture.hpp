#pragma once

#include "texture.hpp"
#include "vec3.hpp"
class SolidTexture : public Texture {
    public:
        SolidTexture (Vec3 color);
        Vec3 color;
        Vec3 read_texture_uv (Vec3 uv) override;
        Vec3 read_rgb255(Vec3 uv) override;
};