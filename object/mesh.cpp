

#include "hitrecord.hpp"
#include "object.hpp"
#include "triangle.hpp"
#include <cfloat>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "material.hpp"
#include "mesh.hpp"
#include "tiny_obj_loader.h"
#include "vec3.hpp"
#include <stdexcept>
#include <vector>

Mesh::Mesh (const char *obj_filename, Material *material) : Object (Vec3 (0, 0, 0), material)
{
        this->obj_filename = (char *)obj_filename;
        this->_load_mesh ();
}

void Mesh::_load_mesh ()
{
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shape;
        std::vector<tinyobj::material_t> mats;
        std::string warn, err;

        if (!tinyobj::LoadObj (&attrib, &shape, &mats, &warn, &err, obj_filename, NULL, true, true))
                throw std::runtime_error (warn + err);

        if (!warn.empty ())
                std::cerr << "warning: " << warn << std::endl;

        std::vector<Vec3> vertices;

        for (size_t i = 0; i < attrib.vertices.size () / 3; i++) {
                Vec3 vertex = Vec3 (attrib.vertices[3 * i + 0], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);
                vertices.push_back (vertex);
        }
        this->vertex_kdtree = KDTree (vertices);

        // Loop over shapes and convert each mesh into a triangle defined by two vectors
        for (size_t s = 0; s < shape.size (); s++) {
                size_t index_offset = 0;
                for (size_t f = 0; f < shape[s].mesh.num_face_vertices.size (); f++) {
                        int fv = shape[s].mesh.num_face_vertices[f];
                        std::vector<Vec3> vertices;
                        std::vector<Vec3> normals;
                        for (size_t v = 0; v < fv; v++) {
                                tinyobj::index_t idx = shape[s].mesh.indices[index_offset + v];
                                Vec3 vertex = Vec3 (attrib.vertices[3 * idx.vertex_index + 0],
                                                    attrib.vertices[3 * idx.vertex_index + 1],
                                                    attrib.vertices[3 * idx.vertex_index + 2]);
                                vertices.push_back (vertex);

                                Vec3 normal = Vec3 (attrib.normals[3 * idx.normal_index + 0],
                                                    attrib.normals[3 * idx.normal_index + 1],
                                                    attrib.normals[3 * idx.normal_index + 2]);

                                normals.push_back (normal);
                        }
                        index_offset += fv;

                        // find face normal from vertex normals
                        Vec3 approx_normal = (normals[0] + normals[1] + normals[2]) / 3.0;

                        // compute face normal from triangle vertices
                        Vec3 u = vertices[1] - vertices[0];
                        Vec3 v = vertices[2] - vertices[0];

                        // we need to flip the actual normal vector if it is pointing in the wrong direction
                        Vec3 actual_normal = u.cross (v);
                        if (actual_normal.dot (approx_normal) < 0)
                                actual_normal = -actual_normal;

                        // construct triangle: add to list of triangles and verticies to triangle map
                        Triangle tri =
                                Triangle (vertices[0], vertices[1], vertices[2], actual_normal, this->material ());

                        this->triangles.push_back (tri);

                        this->vertex_to_triangles[vertices[0]].push_back (tri);
                        this->vertex_to_triangles[vertices[1]].push_back (tri);
                        this->vertex_to_triangles[vertices[2]].push_back (tri);
                }
        }
}

bool Mesh::hit (Ray r, HitRecord &record)
{
        std::vector<Vec3> verticies = this->vertex_kdtree.hit_bbox (r);

        HitRecord best_record;
        best_record.lambda = DBL_MAX;

        for (Vec3 vertex : verticies) {
                for (Triangle tri : this->vertex_to_triangles[vertex]) {
                        HitRecord temp_record;
                        if (tri.hit (r, temp_record) && temp_record.lambda < best_record.lambda) {
                                best_record = temp_record;
                        }
                }
        }

        if (best_record.lambda == DBL_MAX)
                return false;

        record = best_record;

        return true;
}
