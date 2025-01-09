#pragma once

#include "hitrecord.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <utility>
#include <vector>
class KDTree {
    public:
        class BoundingBox {
            public:
                Vec3 min;
                Vec3 max;
                BoundingBox () : min (-Vec3::inf ()), max (Vec3::inf ())
                {
                }
                BoundingBox (Vec3 min, Vec3 max) : min (min), max (max)
                {
                }
                bool hit (Ray r, double &lambda_min, double &lambda_max);
                bool inside(Vec3 point);
                bool inside(Triangle *triangle);
                std::pair<BoundingBox, BoundingBox> split (int axis, double value);
                int longest_dim();
        };

        

        struct KDTreeNode {
                Vec3 value;
                struct KDTreeNode *left;
                struct KDTreeNode *right;
                BoundingBox bbox;
                int bbox_split_dim;
        };

        struct BoundingBoxNode {

            BoundingBox box;
            std::vector<Triangle*> triangles;
            struct BoundingBoxNode *left;
            struct BoundingBoxNode *right;
        };

        KDTree ();
        ~KDTree ();
        KDTree (const KDTree &kdtree);
        KDTree &operator= (const KDTree &other);
        KDTree (std::vector<Vec3> points);
        KDTree (std::vector<Triangle *> triangles);

        friend class KDTree;

        void insert (Vec3 node);
        Vec3 nearest_neighbour (Vec3 node);
        std::vector<Vec3> hit_bbox (Ray r);
        bool ray_hit(Ray r, HitRecord &record);
    private:
        struct KDTreeNode *root;
        struct BoundingBoxNode *bounding_box_root;
        bool _compute_bounding_box_tree_ray_hit(struct BoundingBoxNode *root, Ray r, HitRecord &record);
        struct BoundingBoxNode *_construct_bounding_box_tree(BoundingBox box, std::vector<Triangle *> triangles);
        BoundingBox _compute_bounding_box (std::vector<Vec3> points);
        std::vector<Vec3> _bounding_box_hit (struct KDTreeNode *root, Ray r);
        Vec3 _median_select (std::vector<Vec3> points, int axis);
        void _delete_kdtree (struct KDTreeNode *root);
        Vec3 _closest_to (Vec3 target, Vec3 a, Vec3 b);
        Vec3 _nn (Vec3 node, Vec3 best, struct KDTreeNode *root, int depth);
        struct KDTreeNode *_insert_depth (struct KDTreeNode *root, Vec3 node, BoundingBox box, int depth);
        struct KDTreeNode *_construct_from_list (std::vector<Vec3> points, BoundingBox box, int depth);
        struct KDTreeNode *_copy_tree (struct KDTreeNode *root);
};
