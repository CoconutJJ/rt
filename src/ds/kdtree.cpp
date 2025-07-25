#include "kdtree.hpp"
#include "hitrecord.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <vector>

KDTree::KDTree ()
{
}

KDTree::~KDTree ()
{
}

KDTree::KDTree (std::vector<Triangle *> triangles)
{
        std::vector<Vec3> points;

        for (Triangle *p : triangles) {
                std::vector<Vec3> vertices = p->verticies ();
                points.insert (points.end (), vertices.begin (), vertices.end ());
        }

        this->bounding_box_root = this->_construct_bounding_box_tree (this->_compute_bounding_box (points), triangles);
}

KDTree::KDTree (const KDTree &kdtree)
{
        this->bounding_box_root = kdtree.bounding_box_root;
}

KDTree &KDTree::operator= (const KDTree &other)
{
        this->bounding_box_root = other.bounding_box_root;

        return *this;
}

KDTree::BoundingBox KDTree::_compute_bounding_box (std::vector<Vec3> points)
{
        BoundingBox box (points[0], points[0]);

        for (Vec3 &vertex : points)
                for (int i = 0; i < 3; i++) {
                        box.min.vec[i] = std::min (box.min[i], vertex[i]);
                        box.max.vec[i] = std::max (box.max[i], vertex[i]);
                }

        return box;
}

Vec3 KDTree::_median_select (std::vector<Vec3> points, int axis)
{
        if (points.size () == 1)
                return points[0];

        int median = points.size () / 2;

        std::nth_element (points.begin (), points.begin () + median, points.end (), [axis] (Vec3 a, Vec3 b) {
                return a[axis] < b[axis];
        });

        return points[median];
}

std::pair<KDTree::BoundingBox, KDTree::BoundingBox> KDTree::BoundingBox::split (int axis, double value)
{
        BoundingBox left = *this;
        BoundingBox right = *this;

        left.max.vec[axis] = value;
        right.min.vec[axis] = value;

        return std::make_pair (left, right);
}

bool KDTree::BoundingBox::is_reversed (Ray r, int axis)
{
        return r.origin[axis] > this->max[axis] && r.direction[axis] < 0;
}

int KDTree::BoundingBox::longest_dim ()
{
        int axis = -1;
        double dim_length = 0;

        for (int i = 0; i < 3; i++) {
                double len = std::abs (this->max[i] - this->min[i]);

                if (len > dim_length) {
                        dim_length = len;
                        axis = i;
                }
        }

        return axis;
}

std::pair<double, double> KDTree::BoundingBox::_one_dim_ray_intersection (Ray r, int axis, bool &reversed)
{
        double min = this->min[axis], max = this->max[axis];

        reversed = r.direction[axis] < 0;

        if (reversed)
                std::swap (min, max);

        double t1 = (min - r.origin[axis]) / r.direction[axis];
        double t2 = (max - r.origin[axis]) / r.direction[axis];

        return std::make_pair (t1, t2);
}

bool KDTree::BoundingBox::hit (Ray r, double &lambda_min, double &lambda_max)
{
        lambda_min = -DBL_MAX, lambda_max = DBL_MAX;
        for (int i = 0; i < 3; i++) {
                bool _reversed;
                std::pair<double, double> min_max = this->_one_dim_ray_intersection (r, i, _reversed);

                if (min_max.first > lambda_min)
                        lambda_min = min_max.first;

                if (min_max.second < lambda_max)
                        lambda_max = min_max.second;
        }

        if (lambda_min > lambda_max)
                return false;

        if (lambda_max < 0)
                return false;

        return true;
}

bool KDTree::BoundingBox::inside (Vec3 point)
{
        for (int i = 0; i < 3; i++) {
                if (!(this->min[i] <= point[i] && point[i] <= this->max[i]))
                        return false;
        }

        return true;
}

bool KDTree::BoundingBox::inside (Triangle *triangle)
{
        for (Vec3 v : triangle->verticies ()) {
                if (!this->inside (v))
                        return false;
        }

        return true;
}

struct KDTree::BoundingBoxNode *KDTree::_construct_bounding_box_tree (BoundingBox box,
                                                                      std::vector<Triangle *> triangles)
{
        if (triangles.size () == 0)
                return nullptr;

        int axis = box.longest_dim ();

        std::vector<Vec3> points;

        for (Triangle *p : triangles) {
                std::vector<Vec3> vertices = p->verticies ();
                points.insert (points.end (), vertices.begin (), vertices.end ());
        }

        Vec3 median_point = this->_median_select (points, axis);

        std::pair<BoundingBox, BoundingBox> boxes = box.split (axis, median_point[axis]);

        std::vector<Triangle *> left_triangles;
        std::vector<Triangle *> right_triangles;

        for (Triangle *p : triangles) {
                if (boxes.first.inside (p)) {
                        left_triangles.push_back (p);
                } else if (boxes.second.inside (p)) {
                        right_triangles.push_back (p);
                } else {
                        left_triangles.push_back (p);
                        right_triangles.push_back (p);
                }
        }

        struct BoundingBoxNode *node = new struct BoundingBoxNode;

        node->box = box;
        node->triangles = triangles;

        // continue splitting the bounding box until splitting does not reduce
        // the number of triangles that overlap with the split boxes
        if (left_triangles.size () < triangles.size () && right_triangles.size () < triangles.size ()) {
                node->left = this->_construct_bounding_box_tree (boxes.first, left_triangles);
                node->right = this->_construct_bounding_box_tree (boxes.second, right_triangles);
        } else {
                node->left = NULL;
                node->right = NULL;
        }

        return node;
}

bool KDTree::_compute_bounding_box_tree_ray_hit (struct BoundingBoxNode *root, Ray r, HitRecord &record)
{
        double lambda_min, lambda_max;

        if (!root->box.hit (r, lambda_min, lambda_max))
                return false;

        /**
                If we hit a leaf node, check for intersection with all triangles
                inside the node's bounding box.
         */
        if (!root->left && !root->right) {
                bool hit_anything = false;

                double best_lambda = DBL_MAX;

                for (Triangle *p : root->triangles) {
                        HitRecord temp_record;
                        if (!p->hit (r, temp_record))
                                continue;

                        if (temp_record.lambda < best_lambda) {
                                record = temp_record;
                                best_lambda = temp_record.lambda;
                                hit_anything = true;
                        }
                }

                return hit_anything;
        }

        /**
                KD Tree Debug Log:

                1. Tried removing early ray termination and traversing both left and right and taking smallest lambda:
                   FIXES THE PROBLEM! This means something is wrong the early ray termination code

                2. Somtimes far lambda is less than near lambda

         */

        // Always split the longest dimension
        int split_dim = root->box.longest_dim ();

        // Early Ray Termination: compute ray intersection with bounding box
        // split plane

        double lambda_mid = (root->left->box.max[split_dim] - r.origin[split_dim]) / r.direction[split_dim];

        // We must determine which bounding box is the front box and which one
        // is the back box.
        struct BoundingBoxNode *front = root->left;
        struct BoundingBoxNode *back = root->right;

        bool reversed;
        (void)root->box._one_dim_ray_intersection (r, split_dim, reversed);

        if (reversed)
                std::swap (front, back);

        if (lambda_mid < lambda_min) {
                return this->_compute_bounding_box_tree_ray_hit (back, r, record);
        } else if (lambda_mid > lambda_max) {
                return this->_compute_bounding_box_tree_ray_hit (front, r, record);
        } else {
                // termination step: only intersect with back box if ray misses everything in front box.

                if (this->_compute_bounding_box_tree_ray_hit (front, r, record)) {
                        if (record.lambda <= lambda_mid)
                                return true;
                }

                return this->_compute_bounding_box_tree_ray_hit (back, r, record);
        }
}

bool KDTree::ray_hit (Ray r, HitRecord &record)
{
        return this->_compute_bounding_box_tree_ray_hit (this->bounding_box_root, r, record);
}
