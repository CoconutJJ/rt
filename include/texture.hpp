#pragma once

#include "vec3.hpp"
#include <cstddef>

class Texture {
    public:
        Texture ();
        size_t photon_map_width;
        size_t photon_map_height;
        Vec3 *photon_texture;
        void store_photon (Vec3 uv, Vec3 color);
        virtual Vec3 photon_map (Vec3 point) = 0;
        virtual Vec3 read_texture_uv (Vec3 uv, Vec3 point) = 0;
        virtual Vec3 read_rgb255 (Vec3 uv) = 0;
};