#include "catch_amalgamated.hpp"
#include "hitrecord.hpp"
#include "mesh.hpp"
#include "material.hpp"
TEST_CASE("Mesh hit", "")
{
        SECTION("Mesh::hit returns true if ray hits the mesh")
        {
                Mesh mesh = Mesh("assets/sphere.obj", Vec3(0, 0, 0), nullptr);
                Ray r = Ray(Vec3(0.5, 0.5, 0.5), Vec3(1, 1, 1));
                HitRecord record;
                REQUIRE(mesh.hit(r, record));
        }

        SECTION("Mesh::hit returns false if ray does not hit the mesh")
        {
                Mesh mesh = Mesh("assets/cube.obj", Vec3(0, 0, 0), nullptr);
                Ray r = Ray(Vec3(-3, -3, -3), Vec3(-1, -1, -1));
                HitRecord record;
                REQUIRE(!mesh.hit(r, record));
        }
}