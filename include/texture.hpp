#pragma once

#include "vec3.hpp"

class Texture {
    public:
        Texture ();
        virtual Vec3 read_texture_uv (Vec3 uv, Vec3 point) = 0;
        virtual Vec3 read_rgb255 (Vec3 uv) = 0;
};