#include "kdtree.hpp"
#include "hitrecord.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

KDTree::KDTree () : root (nullptr)
{
}

/*
        This function constructs a KDTree from a list of points. If we have all
        the points ahead of time, this will ensure the tree is well balanced.
*/
KDTree::KDTree (std::vector<Vec3> points) : root (nullptr)
{
        this->root = this->_construct_from_list (points, this->_compute_bounding_box (points), 0);
}

KDTree::~KDTree ()
{
        this->_delete_kdtree (this->root);
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
        if (this->root)
                this->_delete_kdtree (this->root);

        this->root = this->_copy_tree (kdtree.root);
        this->bounding_box_root = kdtree.bounding_box_root;
}

KDTree &KDTree::operator= (const KDTree &other)
{
        if (this->root)
                this->_delete_kdtree (this->root);

        this->root = this->_copy_tree (other.root);
        this->bounding_box_root = other.bounding_box_root;

        return *this;
}

struct KDTree::KDTreeNode *KDTree::_copy_tree (struct KDTree::KDTreeNode *root)
{
        if (!root)
                return nullptr;

        struct KDTreeNode *new_root = new struct KDTreeNode;

        *new_root = *root;

        new_root->left = this->_copy_tree (root->left);
        new_root->right = this->_copy_tree (root->right);

        return new_root;
}

KDTree::BoundingBox KDTree::_compute_bounding_box (std::vector<Vec3> points)
{
        BoundingBox box (points[0], points[0]);

        for (Vec3 &vertex : points) {
                for (int i = 0; i < 3; i++) {
                        box.min[i] = std::min (box.min[i], vertex[i]);
                        box.max[i] = std::max (box.max[i], vertex[i]);
                }
        }

        return box;
}

void KDTree::_delete_kdtree (struct KDTreeNode *root)
{
        if (!root)
                return;

        this->_delete_kdtree (root->left);
        this->_delete_kdtree (root->right);

        delete root;
}

struct KDTree::KDTreeNode *KDTree::_insert_depth (struct KDTreeNode *root, Vec3 node, BoundingBox box, int depth)
{
        int dim_index = depth % 3;

        if (!root) {
                struct KDTreeNode *child = new struct KDTreeNode;
                child->left = nullptr;
                child->right = nullptr;
                child->value = node;
                child->bbox = box;
                child->bbox_split_dim = dim_index;
                return child;
        }

        std::pair<BoundingBox, BoundingBox> boxes = box.split (dim_index, root->value[dim_index]);

        if (node[dim_index] < root->value[dim_index]) {
                root->left = this->_insert_depth (root->left, node, boxes.first, depth + 1);
        } else {
                root->right = this->_insert_depth (root->right, node, boxes.second, depth + 1);
        }

        return root;
}

struct KDTree::KDTreeNode *KDTree::_construct_from_list (std::vector<Vec3> points, BoundingBox box, int depth)
{
        if (points.empty ())
                return nullptr;

        int dim_index = depth % 3;
        Vec3 median_point = this->_median_select (points, dim_index);

        std::vector<Vec3> left_points;
        std::vector<Vec3> right_points;

        for (auto &point : points) {
                if (point[dim_index] < median_point[dim_index]) {
                        left_points.push_back (point);
                } else if (point != median_point) {
                        right_points.push_back (point);
                }
        }

        std::pair<BoundingBox, BoundingBox> boxes = box.split (dim_index, median_point[dim_index]);

        struct KDTreeNode *node = new struct KDTreeNode;
        node->value = median_point;
        node->bbox = box;
        node->bbox_split_dim = dim_index;

        node->left = this->_construct_from_list (left_points, boxes.first, depth + 1);
        node->right = this->_construct_from_list (right_points, boxes.second, depth + 1);

        return node;
}

void KDTree::insert (Vec3 node)
{
        this->root = this->_insert_depth (this->root, node, BoundingBox (), 0);
}

Vec3 KDTree::_closest_to (Vec3 target, Vec3 a, Vec3 b)
{
        if ((target - a).length_squared () > (target - b).length_squared ()) {
                return b;
        } else {
                return a;
        }
}

Vec3 KDTree::_nn (Vec3 node, Vec3 best, struct KDTreeNode *root, int depth)
{
        if (!root)
                return best;

        int dim_index = depth % 3;

        struct KDTreeNode *sibling;

        if (node[dim_index] < root->value[dim_index]) {
                best = this->_nn (node, best, root->left, depth + 1);
                best = this->_closest_to (node, best, root->value);
                sibling = root->right;
        } else {
                best = this->_nn (node, best, root->right, depth + 1);
                best = this->_closest_to (node, best, root->value);
                sibling = root->left;
        }

        /**
            To understand this step, picture in your head what each recursive
            call of this function is doing. The `root` and `depth` parameter
            draw a plane that divides the space into two.

            The `node` parameter (the query node) either lies on the left or
            right of this plane.

            We perform our normal recursive call on the side the point lies on;
            however, if the query node is too close to the dividing plane, then
            it is possible some point on the unexplored side may in fact be
            closer to our point.

            One way to check for this is to see if this possible to measure
            the perpendicular distance between our search node and the dividing
            plane. If this distance is less than the current best node distance,
            we know there may exist a point on the other side.

            Otherwise, since all other unexplored points lie on the other side
            of the plane and the perpendicular distance is the shortest, their
            must be no other point that is closer.
        */

        double root_plane_perp_dist = std::abs (root->value[dim_index] - node[dim_index]);

        if (root_plane_perp_dist * root_plane_perp_dist <= (node - best).length_squared ())
                best = this->_closest_to (node, this->_nn (node, best, sibling, depth + 1), best);

        return best;
}

Vec3 KDTree::nearest_neighbour (Vec3 node)
{
        return this->_nn (node, this->root->value, this->root, 0);
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

        left.max[axis] = value;
        right.min[axis] = value;

        return std::make_pair (left, right);
}

int KDTree::BoundingBox::longest_dim ()
{
        int axis = -1;
        double dim_length = -DBL_MAX;

        for (int i = 0; i < 3; i++) {
                double len = this->max[i] - this->min[i];

                if (len > dim_length) {
                        dim_length = len;
                        axis = i;
                }
        }

        return axis;
}

bool KDTree::BoundingBox::hit (Ray r, double &lambda_min, double &lambda_max)
{
        lambda_min = -DBL_MAX, lambda_max = DBL_MAX;
        for (int i = 0; i < 3; i++) {
                double min = this->min[i], max = this->max[i], origin = r.origin[i], dir = r.direction[i];

                if (dir == 0)
                        continue;

                double t1, t2;
                t1 = (min - origin) / dir;
                t2 = (max - origin) / dir;

                if (t1 > t2) {
                        double t1_tmp = t1;
                        t1 = t2;
                        t2 = t1_tmp;
                }

                if (t1 > lambda_min)
                        lambda_min = t1;

                if (t2 < lambda_max)
                        lambda_max = t2;
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
                if (!(this->min[i] < point[i] && point[i] < this->max[i]))
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

        int split_dim = root->box.longest_dim ();
        double lambda_mid = (root->left->box.max[split_dim] - r.origin[split_dim]) / r.direction[split_dim];

        struct BoundingBoxNode *front = root->left;
        struct BoundingBoxNode *back = root->right;

        double min = root->box.min[split_dim], max = root->box.max[split_dim], origin = r.origin[split_dim], dir = r.direction[split_dim];

        double t1, t2;
        t1 = (min - origin) / dir;
        t2 = (max - origin) / dir;

        if (t1 > t2) 
                std::swap(front, back);
        
        
        if (lambda_mid < lambda_min) {
                return this->_compute_bounding_box_tree_ray_hit (back, r, record);
        } else if (lambda_mid > lambda_max) {
                return this->_compute_bounding_box_tree_ray_hit (front, r, record);
        } else {
                if (!this->_compute_bounding_box_tree_ray_hit (front, r, record))
                        return this->_compute_bounding_box_tree_ray_hit (back, r, record);

                return true;
        }
}

std::vector<Vec3> KDTree::_bounding_box_hit (struct KDTreeNode *root, Ray r)
{
        double lambda_min, lambda_max;

        if (!root || !root->bbox.hit (r, lambda_min, lambda_max))
                return std::vector<Vec3> ();

        if (!root->left && !root->right)
                return std::vector<Vec3> ({ root->value });

        double axis;
        if (root->left)
                axis = root->left->bbox.max[root->bbox_split_dim];
        else
                axis = root->right->bbox.min[root->bbox_split_dim];

        double lambda_mid = (axis - r.origin[root->bbox_split_dim]) / r.direction[root->bbox_split_dim];

        std::vector<Vec3> points;

        if (lambda_mid < lambda_min)
                points = this->_bounding_box_hit (root->right, r);
        else if (lambda_mid > lambda_max)
                points = this->_bounding_box_hit (root->left, r);
        else {
                // compute ray intersection with left bounding box
        }

        // std::vector<Vec3> left_points = this->_bounding_box_hit (root->left, r);

        // std::vector<Vec3> right_points = this->_bounding_box_hit (root->right, r);

        // left_points.emplace_back (root->value);
        // left_points.insert (left_points.end (), right_points.begin (), right_points.end ());

        // return left_points;
}
bool KDTree::ray_hit (Ray r, HitRecord &record)
{
        return this->_compute_bounding_box_tree_ray_hit (this->bounding_box_root, r, record);
}
std::vector<Vec3> KDTree::hit_bbox (Ray r)
{
        return this->_bounding_box_hit (this->root, r);
}
