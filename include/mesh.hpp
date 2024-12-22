/**
    @file mesh.hpp

    @brief This file contains the definition of the Mesh class. Meshes are
    collections of triangles that approximate a smooth surface. They are
    constructed from .obj files and can be used to represent complex objects
    in a scene.

    Meshes are not smooth objects, and as such do not have the ability to
    calculate the TBN/TNB matrix or BRDFs at a point. To do this, we would
    need to first find the triangle inside the mesh that the point lies on,
    then use the triangle's normal and tangent to calculate the TBN matrix
    (recall that Triangle's are smooth objects).

    Meshes are useful for representing complex objects in a scene, but are
    not as efficient as smooth objects for rendering.

*/

#pragma once

#include "kdtree.hpp"
#include "material.hpp"
#include "object.hpp"
#include "tiny_obj_loader.h"
#include "triangle.hpp"
#include "vec3.hpp"
#include <map>
#include <vector>

class Mesh : public Object {
    public:
        Mesh (const char *filename, Vec3 location, Material *material);
        KDTree *vertex_kdtree;

        std::vector<Triangle> triangles;
        std::map<Vec3, std::vector<Triangle> > vertex_to_triangles;
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shape;
        std::vector<tinyobj::material_t> mats;

        char *obj_filename;
        bool is_light_source() override;
        bool hit (Ray r, HitRecord &record) override;
        void vertex_average (Vec3 center);
        void center (Vec3 center);
        Vec3 _compute_orig_mesh_center ();
        std::vector<int> convex_hull ();

    private:
        void _load_mesh ();
};