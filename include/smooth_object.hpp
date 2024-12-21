/**
        @file smooth_object.hpp

        @author David Yue

        @brief This file contains the definition of the SmoothObject class.
        Smooth Objects are objects that have a smooth surface, such as a sphere
        or a plane. Non-smooth objects, meshes (mesh.hpp), however, are a
        collection of triangles that approximate a smooth surface.

        This distinction is important because smooth objects have a easily
        calculable normal and tangent at any point on their surface, while
        non-smooth objects may require more complexity code-wise.

        To that end, only smooth objects have the ability to calculate the
        TBN/TNB matrix and BRDFs given a point. These features are not available
        to meshes. In order to do the equivalent calculations for meshes, we
        would need to first find the triangle inside the mesh that the point
        lies on, then use the triangle's normal and tangent to calculate the TBN
        matrix (recall that Triangle's are smooth objects).
 */

#pragma once
#include "material.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "vec3.hpp"

class SmoothObject : public Object {
    public:
        SmoothObject (Vec3 location, Material *material);
        SmoothObject (Vec3 location1, Vec3 location2, Material *material);
        SmoothObject (Vec3 location1, Vec3 location2, Material *material, Texture *normal_map);
        bool is_light_source () override;

        virtual Vec3 tangent (Vec3 point) = 0;
        virtual Vec3 normal (Vec3 point) = 0;
        virtual Vec3 to_uv (Vec3 point) = 0;

        Vec3 mapped_normal (Vec3 point);
        Mat3 tbn (Vec3 point);
        Mat3 tnb (Vec3 point);
        Vec3 tbn_transform (Vec3 point, Vec3 tangent_v);
        Vec3 brdf (Vec3 point, Vec3 in_direction, Vec3 out_direction);
};