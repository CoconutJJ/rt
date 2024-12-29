#include "camera.hpp"
#include "checkerboard.hpp"
#include "dielectric.hpp"
#include "image_texture.hpp"
#include "lambertian.hpp"
#include "mesh.hpp"
#include "metal.hpp"
#include "phong.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "quad.hpp"
#include "quad_light.hpp"
#include "solid_texture.hpp"
#include "sphere.hpp"
#include "termcolor.hpp"
#include "usage.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <thread>
#include <unistd.h>

void usage ()
{
        fprintf (stderr, "%s", help_txt);
}

World ray_trace_scene ()
{
        World world;
        PointLight light (Vec3 (0, 1, -1), Vec3 (1, 1, 1), Vec3 (1, 1, 1));

        SolidTexture white (Vec3 (1, 1, 1));
        SolidTexture blue (Vec3 (0.01, 0.01, 1));

        Phong background (1, 0.5, 1, 0.01, 0, 1, 0, &blue);

        Phong glass (2, 0.5, 1, 0.03, 100, 0, 0.01, &white);

        Plane backdrop (Vec3 (0, 0, -5), Vec3 (0, 0, 1), &background);

        SolidTexture light_t (Vec3 (0.5, 0.5, 0.5));
        Phong light_panel_mat (1, 1, 1, 0, 0, 1, 0, &light_t);
        Quad light_panel (Vec3 (-0.5, 3, -2), Vec3 (-1, 0, 0), Vec3 (0, 0, -1), &light_panel_mat);
        QuadLight q (&light_panel);

        CheckerboardTexture checkers (Vec3 (0.01, 0.01, 0.01), Vec3 (1, 1, 1));
        Phong floor_material (0.2, 0.6, 0.3, 0.3, 0, 1, 0, &checkers);
        Sphere planet (Vec3 (0, -100.5, 0), 100, &floor_material);

        Sphere obj (Vec3 (0, 0.5, -2), 0.5, &glass);

        world.add (&obj);
        world.add (&planet);
        world.add_light (&q);
        world.add (&light_panel);

        return world;
}
// -framework Metal -framework Foundation -framework Cocoa -framework CoreGraphics -framework MetalKit
struct Camera::RendererSettings process_arguments (int argc, char **argv, char *&filename, int &nthreads)
{
        struct Camera::RendererSettings config;

        memset (&config, 0, sizeof (struct Camera::RendererSettings));

        config.image_width = 600;
        config.aspect_ratio = 16.0 / 9;
        config.vfov = 60;
        config.arealight_samples = 10;
        config.samples_per_pixel = 1000;
        config.max_depth = 5;
        config.use_light_sampling = false;
        config.use_path_tracer = false;
        config.use_scene_sig = false;

        struct option longopts[] = {
                { .name = "out_file", .has_arg = 1, .val = 'f' },
                { .name = "image_width", .has_arg = 1, .val = 'w' },
                { .name = "aspect_ratio", .has_arg = 1, .val = 'r' },
                { .name = "vfov", .has_arg = 1, .val = 'v' },
                { .name = "defocus_angle", .has_arg = 1, .val = 't' },
                { .name = "help", .has_arg = 0, .val = 'h' },
                { .name = "nthreads", .has_arg = 1, .val = 'n' },
                { .name = "arealight_samples", .has_arg = 1, .val = 'a' },
                { .name = "use_path_tracer", .has_arg = 0, .val = 'p' },
                { .name = "samples_per_pixel", .has_arg = 1, .val = 's' },
                { .name = "max_depth", .has_arg = 1, .val = 'd' },
                { .name = "use_light_sampling", .has_arg = 0, .val = 'l' },
                { .name = "use_scene_sig", .has_arg = 0, .val = 'x' },
                { 0 }
        };
        int c, optidx;
        while ((c = getopt_long (argc, argv, "", longopts, &optidx)) != -1) {
                switch (c) {
                case 'f': filename = optarg; break;
                case 'w': config.image_width = strtol (optarg, NULL, 10); break;
                case 'r': {
                        char *split = NULL;
                        double width = strtod (optarg, &split);

                        if (*split != '/') {
                                std::cerr << "Error reading aspect ratio, must be of the form `w/h` (eg 16/9, 1.5/1.0)";
                                exit (EXIT_FAILURE);
                        }
                        double height = strtod (split + 1, &split);

                        config.aspect_ratio = width / height;
                        break;
                }
                case 'v': {
                        config.vfov = strtod (optarg, NULL);
                        break;
                }
                case 't': {
                        config.defocus_angle = strtod (optarg, NULL);
                        break;
                }
                case 'n': {
                        nthreads = strtol (optarg, NULL, 10);
                        break;
                }
                case 'a': {
                        config.arealight_samples = strtol (optarg, NULL, 10);
                        break;
                }
                case 'p': {
                        config.use_path_tracer = true;
                        break;
                }
                case 's': {
                        config.samples_per_pixel = strtol (optarg, NULL, 10);
                        break;
                }
                case 'd': {
                        config.max_depth = strtol (optarg, NULL, 10);
                        break;
                }
                case 'l': {
                        config.use_light_sampling = true;
                        break;
                }
                case 'x': {
                        config.use_scene_sig = true;
                        break;
                }
                case 'h': {
                        usage ();
                        exit (EXIT_SUCCESS);
                }
                }
        }

        return config;
}

int main (int argc, char **argv)
{
        int nthreads = -1;

        char *filename = NULL;

        struct Camera::RendererSettings config = process_arguments (argc, argv, filename, nthreads);

        if (!filename) {
                std::cerr << "Must provide --out_file | -f argument.\n";
                usage ();
                exit (EXIT_FAILURE);
        }

        if (config.image_width == 0) {
                std::cerr << "Must supply --image_width | -w argument.\n";
                usage ();
                exit (EXIT_FAILURE);
        }

        if (config.use_path_tracer && config.samples_per_pixel < 500) {
                std::cerr
                        << termcolor::red << "WARNING: " << termcolor::reset
                        << "path tracer is enabled, pixel sample count is very low! Consider setting sample count > 500.\n ";
        }

        Camera camera;
        camera.initialize (config);

        World world;

        Dielectric glass (1.5, 0);

        CheckerboardTexture checkers (Vec3 (0, 0, 0), Vec3 (1, 1, 1));
        ImageTexture earth_texture("assets/earthmap.jpg");
        Lambertian red_diffuse (1, Vec3 (1, 0.44, 0.45));
        Lambertian green_diffuse (1, Vec3 (0.42, 0.77, 0.09));
        Lambertian blue_diffuse (1, Vec3 (0.01, 0.86, 0.91));
        Lambertian white_diffuse (1, Vec3 (1, 1, 1));
        Lambertian checkers_diffuse (0.5, &checkers);
        Lambertian earth_diffuse(1, &earth_texture);
        Lambertian light (1, Vec3 (1, 1, 1));
        light.emission (Vec3 (20, 20, 20));
        
        Metal metal(0.1, Vec3(0.8,0.8,0.8));
        Metal metal_earth(0, &earth_texture);
        Quad left_wall (Vec3 (-1, 0, 0), Vec3 (0, 0, -2), Vec3 (0, 2, 0), &red_diffuse);
        Quad right_wall (Vec3 (1, 2, 0), Vec3 (0, 0, -2), Vec3 (0, -2, 0), &green_diffuse);
        Quad back_wall (Vec3 (1, 0, -2), Vec3 (0, 2, 0), Vec3 (-2, 0, 0), &earth_diffuse);
        Quad ceiling (Vec3 (1, 2, 0), Vec3 (-2, 0, 0), Vec3 (0, 0, -2), &white_diffuse);
        Quad floor (Vec3 (1, 0, 0), Vec3 (0, 0, -2), Vec3 (-2, 0, 0), &metal);
        Quad light_panel (Vec3 (0.5, 1.98, -0.5), Vec3 (-1, 0, 0), Vec3 (0, 0, -1), &light);
        // Quad front_wall (Vec3 (1, 2, 0), Vec3 (0, -4, 0), Vec3 (-4, 0, 0), &white_diffuse);
        // Mesh m ("assets/stanford-bunny.obj", Vec3 (0, 0.9, -0.5), &glass);
        Sphere sp (Vec3 (0, 0.55, -1), 0.25, &glass);
        Sphere sp2 (Vec3(-.5, 0.25, -1.1), 0.25, &glass);
        Sphere sp3 (Vec3(.5, 0.30, -1.1), 0.25, &red_diffuse);
        world.add (&left_wall);
        world.add (&right_wall);
        world.add (&back_wall);
        world.add (&ceiling);
        world.add (&floor);
        world.add (&light_panel);
        world.add (&sp);
        world.add(&sp2);
        world.add(&sp3);
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