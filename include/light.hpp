#pragma once

#include "smooth_object.hpp"
#include "vec3.hpp"

class Light {
    public:
        SmoothObject *object;
        Light ();
        Light (SmoothObject *object);
        virtual Vec3 sample_point () = 0;
        virtual bool is_point_light () = 0;
        virtual Vec3 specular_intensity (Vec3 point) = 0;
        virtual Vec3 diffuse_intensity (Vec3 point) = 0;
};