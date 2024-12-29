#include "catch_amalgamated.hpp"
#include "kdtree.hpp"
#include "vec3.hpp"
#include <cfloat>
#include <vector>

TEST_CASE ("KDTree_Insert", "")
{
        KDTree tree = KDTree ();
        SECTION ("KDTree::insert inserts a point into the KDTree")
        {
                tree.insert (Vec3 (0, 0, 0));

                REQUIRE (tree.root->value == Vec3 (0, 0, 0));
        }

        SECTION ("KDTree::insert inserts multiple points into the KDTree")
        {
                std::vector<Vec3> random_verticies;

                for (int i = 0; i < 1000; i++) {
                        random_verticies.push_back (Vec3 (rand () % 1000, rand () % 1000, rand () % 1000));
                }

                for (int i = 0; i < 1000; i++) {
                        tree.insert (random_verticies[i]);
                }

                REQUIRE (tree.root->value == random_verticies[0]);
        }
}

TEST_CASE ("KDTree_Search", "")
{
        SECTION ("KDTree:nearest_neighbour finds nearest neighbour from static list")
        {
                Vec3 query = Vec3::random ();
                Vec3 closest;
                double closest_distance = DBL_MAX;

                std::vector<Vec3> random_verticies;

                for (int i = 0; i < 1000; i++) {
                        Vec3 random = Vec3::random ();
                        double distance = (random - query).length_squared ();

                        if (distance < closest_distance) {
                                closest = random;
                                closest_distance = distance;
                        }

                        random_verticies.push_back (random);
                }

                KDTree tree = KDTree (random_verticies);

                Vec3 nearest = tree.nearest_neighbour (query);

                REQUIRE (nearest == closest);
        }

        SECTION ("KDTree:nearest_neighbour finds nearest neighbour from dynamic list")
        {
                Vec3 query = Vec3::random ();
                Vec3 closest;
                double closest_distance = DBL_MAX;

                KDTree tree = KDTree ();

                for (int i = 0; i < 1000; i++) {
                        Vec3 random = Vec3::random ();
                        double distance = (random - query).length_squared ();

                        if (distance < closest_distance) {
                                closest = random;
                                closest_distance = distance;
                        }
                        tree.insert (random);
                        Vec3 nearest = tree.nearest_neighbour (query);
                        REQUIRE (nearest == closest);
                }
        }
}

TEST_CASE ("KDTree_Bounding_Box", "")
{
        SECTION ("KDTree::BoundingBox::split splits the bounding box into two")
        {
                KDTree::BoundingBox box = KDTree::BoundingBox (Vec3 (0, 0, 0), Vec3 (1, 1, 1));
                std::pair<KDTree::BoundingBox, KDTree::BoundingBox> boxes = box.split (0, 0.5);

                REQUIRE (boxes.first.min == Vec3 (0, 0, 0));
                REQUIRE (boxes.first.max == Vec3 (0.5, 1, 1));
                REQUIRE (boxes.second.min == Vec3 (0.5, 0, 0));
                REQUIRE (boxes.second.max == Vec3 (1, 1, 1));
        }

        SECTION ("KDTree::BoundingBox::hit returns true if ray hits the bounding box")
        {
                KDTree::BoundingBox box = KDTree::BoundingBox (Vec3 (0, 0, 0), Vec3 (1, 1, 1));
                Ray r = Ray (Vec3 (0.5, 0.5, 0.5), Vec3 (1, 1, 1));
                REQUIRE (box.hit (r));
        }

        SECTION ("KDTree::BoundingBox::hit returns false if ray does not hit the bounding box")
        {
                KDTree::BoundingBox box = KDTree::BoundingBox (Vec3 (0, 0, 0), Vec3 (1, 1, 1));
                Ray r = Ray (Vec3 (-0.5, -0.5, -0.5), Vec3 (-1, -1, -1));
                REQUIRE (!box.hit (r));
        }
}

TEST_CASE ("KDTree_Bounding_Box_Hit", "")
{
        SECTION ("KDTree::_bounding_box_hit returns points that hit the bounding box")
        {
                KDTree tree = KDTree ();
                tree.insert (Vec3 (0, 0, 0));
                tree.insert (Vec3 (1, 1, 1));
                tree.insert (Vec3 (2, 2, 2));

                Ray r = Ray (Vec3 (0.5, 0.5, 0.5), Vec3 (1, 1, 1));
                std::vector<Vec3> points = tree.hit_bbox (r);

                REQUIRE (points.size () == 3);
                REQUIRE (points[0] == Vec3 (0, 0, 0));
                REQUIRE (points[1] == Vec3 (1, 1, 1));
        }

        SECTION ("KDTree::_bounding_box_hit returns empty vector if no points hit the bounding box")
        {
                KDTree tree = KDTree ();
                tree.insert (Vec3 (0, 0, 0));
                tree.insert (Vec3 (1, 1, 1));
                tree.insert (Vec3 (2, 2, 2));

                Ray r = Ray (Vec3 (-0.5, -0.5, -0.5), Vec3 (-1, -1, -1));
                std::vector<Vec3> points = tree.hit_bbox (r);

                REQUIRE (points.size () == 1);
        }
}

TEST_CASE ("KDTree_Destructor", "")
{
        SECTION ("KDTree::~KDTree deletes the KDTree")
        {
                KDTree *tree = new KDTree ();
                tree->insert (Vec3 (0, 0, 0));
                tree->insert (Vec3 (1, 1, 1));
                tree->insert (Vec3 (2, 2, 2));
                
                delete tree;
        }
}

