#include "camera.hpp"
#include "checkerboard.hpp"
#include "image_texture.hpp"
#include "lambertian.hpp"
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

        char *filename = NULL, *scene_file = NULL;

        int image_width = -1, nthreads = -1;
        double aspect_ratio = 16.0 / 9.0, vfov = 90.0, defocus_angle = 0.0, arealight_samples = 2;

        struct option longopts[] = {
                { .name = "out_file", .has_arg = 1, .val = 'f' },
                { .name = "image_width", .has_arg = 1, .val = 'w' },
                { .name = "aspect_ratio", .has_arg = 1, .val = 'r' },
                { .name = "vfov", .has_arg = 1, .val = 'v' },
                { .name = "defocus_angle", .has_arg = 1, .val = 't' },
                { .name = "help", .has_arg = 0, .val = 'h' },
                { .name = "nthreads", .has_arg = 1, .val = 'n' },
                { .name = "arealight_samples", .has_arg = 1, .val = 'a' },
                { .name = "scene_file", .has_arg = 1, .val = 'i' },
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
                case 'i': {
                        scene_file = optarg;
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

        // SceneFile s((std::string(scene_file)));

        Camera camera;
        camera.initialize (aspect_ratio, image_width, vfov, defocus_angle, arealight_samples);

        World world;
        PointLight light (Vec3 (0, 1, -1), Vec3 (1, 1, 1), Vec3 (1, 1, 1));

        // ImageTexture earth_texture ("assets/earthmap.jpg");
        // ImageTexture wave_map ("assets/normal_map_waves.png");

        SolidTexture white (Vec3 (1, 1, 1));
        SolidTexture blue (Vec3 (0.01, 0.01, 1));

        Phong background (1, 0.5, 1, 0.01, 0, 1, 0, &blue);

        Phong glass (2, 0.5, 1, 0.03, 100, 0, 0.01, &white);

        // Phong earth_background (1, 1, 1, 0.1, 50, 1, 0, &earth_texture);
        Plane backdrop (Vec3 (0, 0, -5), Vec3 (0, 0, 1), &background);

        
        SolidTexture light_t(Vec3(0.5, 0.5, 0.5));
        Phong light_panel_mat (1, 1, 1, 0, 0, 1, 0, &light_t);
        Quad light_panel (Vec3 (-0.5, 3, -2), Vec3 (-1, 0, 0), Vec3 (0, 0, -1), &light_panel_mat);
        QuadLight q (&light_panel);


        CheckerboardTexture checkers (Vec3 (0.01, 0.01, 0.01), Vec3 (1, 1, 1));
        Phong floor_material (0.2, 0.6, 0.3, 0.3, 0, 1, 0, &checkers);
        Sphere planet (Vec3 (0, -100.5, 0), 100, &floor_material);
        
        Sphere obj (Vec3 (0, 0.5, -2), 0.5, &glass);
        // world.add (&backdrop);
        // world.add(&background_plane);
        world.add (&obj);
        // world.add_light (&light);
        world.add (&planet);
        world.add_light (&q);
        world.add (&light_panel);

        // std::cerr << "Computing Photon mapping...\n";
        // world.photon_map_forward_pass();

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