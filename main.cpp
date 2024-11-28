#include "camera.hpp"
#include "image_texture.hpp"
#include "lambertian.hpp"
#include "light.hpp"
#include "metal.hpp"
#include "phong.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "quad.hpp"
#include "quad_light.hpp"
#include "solid_texture.hpp"
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
        double aspect_ratio = 16.0 / 9.0, vfov = 90.0, defocus_angle = 0.0, focus_dist = 1.0, arealight_samples = 2;

        struct option longopts[] = {
                { .name = "out_file", .has_arg = 1, .val = 'f' },
                { .name = "image_width", .has_arg = 1, .val = 'w' },
                { .name = "aspect_ratio", .has_arg = 1, .val = 'r' },
                { .name = "vfov", .has_arg = 1, .val = 'v' },
                { .name = "defocus_angle", .has_arg = 1, .val = 't' },
                { .name = "focus_dist", .has_arg = 1, .val = 'd' },
                { .name = "help", .has_arg = 0, .val = 'h' },
                { .name = "nthreads", .has_arg = 1, .val = 'n' },
                { .name = "arealight_samples", .has_arg = 1, .val = 'a' },
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
                case 'a': {
                        arealight_samples = strtol (optarg, NULL, 10);
                        break;
                }
                case 'h': {
                        usage ();
                        exit (EXIT_SUCCESS);
                }
                }
        }
        std::cout << "rt: a ray tracer\n";

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
        camera.initialize (aspect_ratio, image_width, vfov, defocus_angle, arealight_samples);

        World world;
        ImageTexture earth_texture ("earthmap.jpg");

        SolidTexture white (Vec3 (1, 1, 1));
        Phong light_material (1, 1, 1, 0, 0, &white);

        Quad q (Vec3 (0, 0, 1), Vec3 (0, 2, 0), Vec3 (2, 0, 0), &light_material);

        PointLight light (Vec3 (0, 1, 0), Vec3 (1, 1, 1), Vec3 (1, 1, 1));
        QuadLight qlight (&q);
        SolidTexture pink (Vec3 (1, 0.1, 0.1));
        SolidTexture yellow (Vec3 (0.76, 0.76, 0.16));
        SolidTexture blue (Vec3 (0.2, 0.2, 0.9));
        SolidTexture yellow_floor (Vec3 (0.2, 0.9, 0.2));

        Phong pink_material (5, 2, 0.4, 0.1, 200, &earth_texture);
        Phong yellow_material (5, 2, 0.4, 0.1, 100, &yellow);
        Phong backgroud_material (1.5, 0.5, 0.1, 0.1, 1, &blue);
        Phong floor_material (1.5, 1.5, 0.1, 0, 1, &yellow_floor);

        Plane background (Vec3 (0, 0, -2), Vec3 (0, 0, 1), &backgroud_material);
        Plane floor (Vec3 (0, -1, 0), Vec3 (0, 1, 0), &floor_material);

        Sphere sp1 (Vec3 (-1.0, 0.0, -1.2), 0.5, &pink_material);
        Sphere sp2 (Vec3 (1.0, 0.0, -1.2), 0.5, &yellow_material);

        world.add_light ((Light *)&light);
        // world.add_light ((Light *)&qlight);
        world.add (&sp1);
        world.add (&sp2);
        world.add (&background);
        world.add (&floor);
        // world.add (&q);

        if (nthreads < 0)
                nthreads = std::thread::hardware_concurrency ();

        if (nthreads > 1) {
                std::cerr << "rt running on " << nthreads << " threads.\n";
                camera.render_multithreaded (&world, filename, nthreads);
        } else {
                camera.render (&world, filename);
        }

        return 0;
}