#include "kdtree.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <iostream>
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
        this->root = this->_construct_from_list (points, BoundingBox (this->_compute_bounding_box (points)), 0);
}

KDTree::~KDTree ()
{
        this->_delete_kdtree (this->root);
}

KDTree::KDTree (const KDTree &kdtree)
{
        if (this->root)
                this->_delete_kdtree (this->root);

        this->root = this->_copy_tree (kdtree.root);
}

KDTree &KDTree::operator= (const KDTree &other)
{
        if (this->root)
                this->_delete_kdtree (this->root);

        this->root = this->_copy_tree (other.root);

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

bool KDTree::BoundingBox::hit (Ray r)
{
        double _;

        for (int dim_i = 0; dim_i < 3 - 1; dim_i++) {
                Vec3 dim_i_vector = Vec3 (0, 0, 0);
                dim_i_vector[dim_i] = 1;
                for (int dim_j = dim_i + 1; dim_j < 3; dim_j++) {
                        Vec3 dim_j_vector = Vec3 (0, 0, 0);
                        dim_j_vector[dim_j] = 1;

                        double u_length = this->max[dim_i] - this->min[dim_i];

                        double v_length = this->max[dim_j] - this->min[dim_j];


                        // if (u_length != DBL_MAX || v_length != DBL_MAX)
                        //         std::cerr << u_length << ", " << v_length << std::endl;

                        if (hit_box (this->min, dim_i_vector, dim_j_vector, u_length, v_length, r, _, _, _))
                                return true;

                        if (hit_box (this->max, -dim_i_vector, -dim_j_vector, u_length, v_length, r, _, _, _))
                                return true;
                }
        }

        return false;
}

std::vector<Vec3> KDTree::_bounding_box_hit (struct KDTreeNode *root, Ray r)
{
        if (!root || !root->bbox.hit (r))
                return std::vector<Vec3> ();

        if (!root->left && !root->right)
                return std::vector<Vec3> ({ root->value });

        std::vector<Vec3> left_points = this->_bounding_box_hit (root->left, r);

        std::vector<Vec3> right_points = this->_bounding_box_hit (root->right, r);

        left_points.emplace_back (root->value);
        left_points.insert (left_points.end (), right_points.begin (), right_points.end ());

        return left_points;
}

std::vector<Vec3> KDTree::hit_bbox (Ray r)
{
        return this->_bounding_box_hit (this->root, r);
}
