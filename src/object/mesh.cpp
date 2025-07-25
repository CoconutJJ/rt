
#include "kdtree.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include <ostream>
#include "hitrecord.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "utils.hpp"
#include "vec3.hpp"

#include <iostream>
#include <vector>

Mesh::Mesh (const char *obj_filename, Vec3 location, double scale, Material *material)
        : Object (location, material), scale (scale)
{
        this->obj_filename = (char *)obj_filename;
        std::string warn, err;

        this->triangles = load_obj_mesh ((char *)obj_filename, material);

        Vec3 centroid = compute_mesh_centroid (this->triangles);

        for (Triangle *tri : triangles) 
                tri->translate (-centroid)->scale (scale)->translate (location);
        
        this->triangle_kdtree = KDTree (this->triangles);

        std::cerr << "Loaded mesh triangles: " << this->triangles.size () << std::endl;
}

bool Mesh::is_light_source ()
{
        return false;
}

bool Mesh::hit (Ray r, HitRecord &record)
{
        return this->triangle_kdtree.ray_hit (r, record);
}
