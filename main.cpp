#include "camera.hpp"
#include "dielectric.hpp"
#include "image_texture.hpp"
#include "lambertian.hpp"
#include "light.hpp"
#include "metal.hpp"
#include "phong.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "usage.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <thread>
#include <unistd.h>

void usage ()
{
        fprintf (stderr, "%s", help_txt);
}

int main (int argc, char **argv)
{
        int c, optidx = 0;

        char *filename = NULL;

        int image_width = -1, nthreads = 1;
        double aspect_ratio = 16.0 / 9.0, vfov = 90.0, defocus_angle = 0.0, focus_dist = 1.0;

        struct option longopts[] = {
                { .name = "out_file", .has_arg = 1, .val = 'f' },
                { .name = "image_width", .has_arg = 1, .val = 'w' },
                { .name = "aspect_ratio", .has_arg = 1, .val = 'r' },
                { .name = "vfov", .has_arg = 1, .val = 'v' },
                { .name = "defocus_angle", .has_arg = 1, .val = 't' },
                { .name = "focus_dist", .has_arg = 1, .val = 'd' },
                { .name = "help", .has_arg = 0, .val = 'h' },
                { .name = "nthreads", .has_arg = 1, .val = 'n' },
                { 0 }
        };
        while ((c = getopt_long (argc, argv, "f:w:", longopts, &optidx)) != -1) {
                switch (c) {
                case 'f': filename = optarg; break;
                case 'w': image_width = strtol (optarg, NULL, 10); break;
                case 'r': {
                        char *split = NULL;
                        double width = strtod (optarg, &split);

                        if (*split != '/') {
                                std::cerr << "Error reading aspect ratio, must be of the form `w/h` (eg 16/9, 1.5/1.0)";
                                exit (EXIT_FAILURE);
                        }
                        double height = strtod (split + 1, &split);

                        aspect_ratio = width / height;
                        break;
                }

                case 'v': {
                        vfov = strtod (optarg, NULL);
                        break;
                }
                case 't': {
                        defocus_angle = strtod (optarg, NULL);
                        break;
                }
                case 'd': {
                        focus_dist = strtod (optarg, NULL);
                        break;
                }
                case 'n': {
                        nthreads = strtol (optarg, NULL, 10);
                        break;
                }
                case 'h': {
                        usage ();
                        exit (EXIT_SUCCESS);
                }
                }
        }

        if (!filename) {
                std::cerr << "Must provide --out_file | -f argument.\n";
                usage ();
                exit (EXIT_FAILURE);
        }

        if (image_width <= 0) {
                std::cerr << "Must supply --image_width | -w argument.\n";
                usage ();
                exit (EXIT_FAILURE);
        }

        Camera camera;
        camera.initialize (aspect_ratio, image_width, vfov);

        World world;
        ImageTexture earth_texture ("earthmap.jpg");
        Lambertian lamb (Vec3 (0.8, 0.8, 0.0));
        Lambertian lamb2 (&earth_texture);
        Metal metal (Vec3 (0.7, 0.6, 0.5), 0.1);
        Dielectric glass (1.5);

        Light light (Vec3 (-1, 5, -0.5), Vec3 (0.8, 0.8, 0.8), Vec3 (0.8, 0.8, 0.8));
        Light light2 (Vec3 (2, 5, -0.5), Vec3 (0.8, 0.8, 0.8), Vec3 (0.8, 0.8, 0.8));
        Phong phong (&world, Vec3 (1, 0.1, 0.1), camera.center, 5, 5, 0.7, 200);
        Phong phong2 (&world, Vec3 (0.1, 1, 0.1), camera.center, 2, 2, 0.1, 256);

        phong.add_light (&light);
        phong2.add_light (&light);

        Plane p (Vec3 (0, -0.5, 0), Vec3 (0, 1, 0), &phong2);
        // Sphere sp (Vec3 (-1.0, 0.0, -1.0), 0.5, &metal);
        Sphere sp3 (Vec3 (0.0, 0.0, -1.2), 0.5, &phong);
        // Sphere sp2 (Vec3 (1.0, 0.0, -1.0), 0.5, &glass);
        // Sphere ground (Vec3 (0, -100.5, -1.5), 100, &lamb);

        // world.add (&sp);
        // world.add (&sp2);
        world.add (&sp3);
        world.add (&p);
        // world.add (&ground);

        if (nthreads > 1)
                camera.render_multithreaded (&world, filename, 4);
        else if (nthreads < 0)
                camera.render_multithreaded (&world, filename, std::thread::hardware_concurrency ());
        else
                camera.render (&world, filename);

        return 0;
}