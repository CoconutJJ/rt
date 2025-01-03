#pragma once

#include "ray.hpp"
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
                bool hit (Ray r);
                std::pair<BoundingBox, BoundingBox> split (int axis, double value);
        };

        struct KDTreeNode {
                Vec3 value;
                struct KDTreeNode *left;
                struct KDTreeNode *right;
                BoundingBox bbox;
        };

        KDTree ();
        ~KDTree ();
        KDTree (const KDTree &kdtree);
        KDTree &operator= (const KDTree &other);
        KDTree (std::vector<Vec3> points);

        friend class KDTree;

        void insert (Vec3 node);
        Vec3 nearest_neighbour (Vec3 node);
        std::vector<Vec3> hit_bbox (Ray r);

    private:
        struct KDTreeNode *root;
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
