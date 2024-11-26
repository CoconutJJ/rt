#pragma once

#include "vec3.hpp"

class Texture {
    public:
        Texture ();
        virtual Vec3 read_texture_uv (Vec3 uv) = 0;
};