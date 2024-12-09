#include "kdtree.hpp"
#include "vec3.hpp"
#include <cstdlib>

KDTree::KDTree () : root(nullptr)
{
}

KDTree::~KDTree ()
{
        this->_delete_kdtree(this->root);
}

void KDTree::_delete_kdtree (struct KDTreeNode *root)
{
        if (!root)
                return;

        this->_delete_kdtree (root->left);
        this->_delete_kdtree (root->right);

        delete root;
}

struct KDTree::KDTreeNode *KDTree::_insert_depth (struct KDTreeNode *root, Vec3 node, int depth)
{
        int dim_index = depth % 3;

        if (!root) {
                struct KDTreeNode *child = new struct KDTreeNode;
                child->left = nullptr;
                child->right = nullptr;
                child->value = node;
                return child;
        }

        if (node[dim_index] < root->value[dim_index]) {
                root->left = this->_insert_depth (root->left, node, depth + 1);
        } else {
                root->right = this->_insert_depth (root->right, node, depth + 1);
        }

        return root;
}

void KDTree::insert (Vec3 node)
{
        this->root = this->_insert_depth (this->root, node, 0);
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

        if (root_plane_perp_dist * root_plane_perp_dist < (node - best).length_squared ())
                best = this->_closest_to (node, this->_nn (node, best, sibling, depth + 1), best);

        return best;
}

Vec3 KDTree::nearest_neighbour (Vec3 node)
{
        return this->_nn (node, this->root->value, this->root, 0);
}