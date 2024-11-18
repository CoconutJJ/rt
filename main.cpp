#include "camera.hpp"
#include "lambertian.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "metal.hpp"
#include "vec3.hpp"
#include "world.hpp"
int main (int argc, char **argv)
{
        Camera camera (16.0 / 9.0, 800);
        World world;

        Lambertian lamb (Vec3 (0.8, 0.8, 0.0));
        Metal metal (Vec3(0.8, 0.6, 0.2));
        Sphere sp (Vec3 (-0.5, 0, -1.5), 0.5, &metal);
        Sphere sp2(Vec3 (0.5, 0, -1.5), 0.5, &lamb);
        Sphere ground (Vec3(0, -100.5, -1.5), 100, &lamb);
        
        world.add (&sp);
        world.add(&sp2);
        world.add(&ground);
        camera.render (&world);
}