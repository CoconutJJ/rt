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
#include "smooth_object.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <map>
#include <vector>
class Mesh : Object {
    public:
        Mesh (const char *filename, Material *material);

        std::vector<Triangle> triangles;
        std::map<Vec3, std::vector<Triangle> > vertex_to_triangles;
        KDTree vertex_kdtree;
        char *obj_filename;
        bool hit (Ray r, HitRecord &record) override;
        void vertex_average (Vec3 center);
        void centroid (Vec3 centroid);
        std::vector<int> convex_hull ();

    private:
        void _load_mesh ();
};