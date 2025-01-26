#pragma once

#include "hitrecord.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <utility>
#include <vector>
class KDTree {
    public:
        friend class KDTree;
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
                bool inside (Vec3 point);
                bool inside (Triangle *triangle);
                std::pair<BoundingBox, BoundingBox> split (int axis, double value);
                std::pair<double, double> _one_dim_ray_intersection (Ray r, int axis, bool &reversed);
                int longest_dim ();
                bool is_reversed (Ray r, int axis);
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
                std::vector<Triangle *> triangles;
                struct BoundingBoxNode *left;
                struct BoundingBoxNode *right;
        };

        KDTree ();
        ~KDTree ();
        KDTree (const KDTree &kdtree);
        KDTree &operator= (const KDTree &other);
        KDTree (std::vector<Vec3> points);
        KDTree (std::vector<Triangle *> triangles);

        bool ray_hit (Ray r, HitRecord &record);

    private:
        struct BoundingBoxNode *bounding_box_root;
        bool _compute_bounding_box_tree_ray_hit (struct BoundingBoxNode *root, Ray r, HitRecord &record);
        struct BoundingBoxNode *_construct_bounding_box_tree (BoundingBox box, std::vector<Triangle *> triangles);
        BoundingBox _compute_bounding_box (std::vector<Vec3> points);
        Vec3 _median_select (std::vector<Vec3> points, int axis);
};
