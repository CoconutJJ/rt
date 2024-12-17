#pragma once

#include "vec3.hpp"
class KDTree {
    public:
        struct KDTreeNode {
                Vec3 value;
                struct KDTreeNode *left;
                struct KDTreeNode *right;
        };

        KDTree ();
        ~KDTree ();
        struct KDTreeNode *root;
        void insert (Vec3 node);
        Vec3 nearest_neighbour (Vec3 node);

    private:
        void _delete_kdtree (struct KDTreeNode *root);
        Vec3 _closest_to (Vec3 target, Vec3 a, Vec3 b);
        Vec3 _nn (Vec3 node, Vec3 best, struct KDTreeNode *root, int depth);
        struct KDTreeNode *_insert_depth (struct KDTreeNode *root, Vec3 node, int depth);
};
