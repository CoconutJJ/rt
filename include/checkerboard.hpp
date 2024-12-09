#pragma once
#include "texture.hpp"
#include "vec3.hpp"
class CheckerboardTexture : public Texture {
    public:
        CheckerboardTexture (Vec3 even_color, Vec3 odd_color);
        Vec3 even_color, odd_color;
        Vec3 read_texture_uv (Vec3 uv, Vec3 point) override;
        Vec3 read_rgb255 (Vec3 uv) override;
        Vec3 photon_map (Vec3 point) override;
};