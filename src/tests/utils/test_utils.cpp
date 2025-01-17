#include "lib/catch_amalgamated.hpp"

#include "mat3.hpp"
#include "utils.hpp"
#include "vec3.hpp"

TEST_CASE ("Texture_Projection", "")
{
        SECTION ("Texture Projection matrix returns correct values")
        {
                Vec3 p1 = Vec3 (0, 0, 0);
                Vec3 p2 = Vec3 (2, 0, 0);
                Vec3 p3 = Vec3 (2, 2, 0);

                Vec3 x = p2 - p1;
                Vec3 y = p3 - p1;

                Vec3 t1 = Vec3 (0, 0, 0);
                Vec3 t2 = Vec3 (1, 0, 0);
                Vec3 t3 = Vec3 (1, 1, 0);

                Vec3 u = t2 - t1;
                Vec3 v = t3 - t1;

                Mat3 T = texture_projection_matrix (p1, p2, p3, t1, t2, t3);

                double alpha = 0.5;
                double beta = 0.5;

                REQUIRE (T * (alpha * x + beta * y) == alpha * u + beta * v);
        }

        SECTION ("Texture Projection matrix exhaustive test")
        {
                for (int i = 0; i < 1000; i++) {
                        Vec3 p1 = Vec3::random ();
                        Vec3 p2 = Vec3::random ();
                        Vec3 p3 = Vec3::random ();

                        Vec3 x = p2 - p1;
                        Vec3 y = p3 - p1;

                        Vec3 t1 = Vec3::random ();
                        Vec3 t2 = Vec3::random ();
                        Vec3 t3 = Vec3::random ();

                        Vec3 u = t2 - t1;
                        Vec3 v = t3 - t1;

                        Mat3 T = texture_projection_matrix (p1, p2, p3, t1, t2, t3);

                        double alpha = random_double (0, 1);
                        double beta = random_double (0, 1);

                        Vec3 diff = (T * (alpha * x + beta * y)) - (alpha * u + beta * v);

                        REQUIRE (diff.near_zero ());
                }
        }

        BENCHMARK ("compute texture projection matrix")
        {
                return texture_projection_matrix (Vec3::random (),
                                                  Vec3::random (),
                                                  Vec3::random (),
                                                  Vec3::random (),
                                                  Vec3::random (),
                                                  Vec3::random ());
        };
}