
#include "kdtree.hpp"
#include <cassert>
#include <ostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "hitrecord.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <cfloat>
#include <iostream>
#include <stdexcept>
#include <vector>

Mesh::Mesh (const char *obj_filename, Vec3 location, Material *material) : Object (location, material)
{
        this->obj_filename = (char *)obj_filename;
        std::string warn, err;

        if (!tinyobj::LoadObj (&attrib, &shape, &mats, &warn, &err, obj_filename, NULL, true, true))
                throw std::runtime_error (warn + err);

        if (!warn.empty ())
                std::cerr << "warning: " << warn << std::endl;

        this->_load_mesh ();

        std::cerr << "Loaded mesh verticies: " << this->attrib.vertices.size () / 3 << std::endl;
}

bool Mesh::is_light_source ()
{
        return false;
}

void Mesh::center (Vec3 center)
{
        this->location = center;

        this->_load_mesh ();
}

Vec3 Mesh::_compute_orig_mesh_center ()
{
        Vec3 center = Vec3 (0, 0, 0);

        for (size_t i = 0; i < attrib.vertices.size () / 3; i++) {
                Vec3 vertex = Vec3 (attrib.vertices[3 * i + 0], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);
                center += vertex;
        }

        return center / (attrib.vertices.size () / 3.0);
}

void Mesh::_load_mesh ()
{
        this->triangles.clear ();
        this->vertex_to_triangles.clear ();
        Vec3 orig_center = this->_compute_orig_mesh_center ();

        Vec3 delta = -orig_center + this->location;

        std::vector<Vec3> vertices;

        for (size_t i = 0; i < attrib.vertices.size () / 3; i++) {
                Vec3 vertex = Vec3 (attrib.vertices[3 * i + 0], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);
                vertices.push_back (vertex + delta);
        }
        this->vertex_kdtree = KDTree (vertices);

        // Loop over shapes and convert each mesh into a triangle defined by two vectors
        for (size_t s = 0; s < shape.size (); s++) {
                size_t index_offset = 0;
                for (size_t f = 0; f < shape[s].mesh.num_face_vertices.size (); f++) {
                        int fv = shape[s].mesh.num_face_vertices[f];
                        assert (fv == 3);
                        std::vector<Vec3> vertices;
                        std::vector<Vec3> normals;
                        std::vector<Vec3> texcoords;
                        for (int v = 0; v < fv; v++) {
                                tinyobj::index_t idx = shape[s].mesh.indices[index_offset + v];
                                Vec3 vertex = Vec3 (attrib.vertices[3 * idx.vertex_index + 0],
                                                    attrib.vertices[3 * idx.vertex_index + 1],
                                                    attrib.vertices[3 * idx.vertex_index + 2]);
                                vertices.push_back (vertex + delta);

                                if (idx.normal_index != -1) {
                                        Vec3 normal = Vec3 (attrib.normals[3 * idx.normal_index + 0],
                                                            attrib.normals[3 * idx.normal_index + 1],
                                                            attrib.normals[3 * idx.normal_index + 2]);

                                        normals.push_back (normal);
                                }

                                if (idx.texcoord_index != -1) {
                                        Vec3 texture = Vec3 (attrib.texcoords[2 * idx.texcoord_index + 0],
                                                             attrib.texcoords[2 * idx.texcoord_index + 1],
                                                             0);

                                        texcoords.push_back (texture);
                                }
                        }
                        index_offset += fv;

                        // find face normal from vertex normals
                        Vec3 approx_normal = (normals[0] + normals[1] + normals[2]) / 3.0;

                        // compute face normal from triangle vertices
                        Vec3 u = vertices[1] - vertices[0];
                        Vec3 v = vertices[2] - vertices[0];

                        // we need to flip the actual normal vector if it is pointing in the wrong direction
                        Vec3 actual_normal = u.cross (v).unit ();
                        if (actual_normal.dot (approx_normal) < 0)
                                actual_normal = -actual_normal;

                        // construct triangle: add to list of triangles and verticies to triangle map
                        Triangle tri = Triangle (vertices[0], vertices[1], vertices[2], actual_normal, this->material);

                        tri.load_texture_coordinates (texcoords[0], texcoords[1], texcoords[2]);

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

        // std::cerr << "Vertex Count: " << verticies.size () << std::endl;

        std::set<Triangle *> triangles;

        for (Vec3 vertex : verticies) {
                for (Triangle &tri : this->vertex_to_triangles[vertex]) {
                        triangles.emplace (&tri);
                }
        }

        // std::cerr << double(verticies.size()) / (attrib.vertices.size() / 3) << std::endl;

        HitRecord best_record;
        best_record.lambda = DBL_MAX;
        for (Triangle *tri : triangles) {
                HitRecord temp_record;
                if (tri->hit (r, temp_record) && temp_record.lambda < best_record.lambda) {
                        best_record = temp_record;
                }
        }

        // std::cerr << "Triangles: " << triangle_count << std::endl;

        if (best_record.lambda == DBL_MAX)
                return false;

        record = best_record;

        return true;
}
