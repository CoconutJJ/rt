#include <cstdlib>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION

#include "progress_bar.hpp"
#include "stb_image.hpp"
#include "vec3.hpp"
#include <cstdint>
#include <fstream>
#include <getopt.h>
#include <unistd.h>

struct pixel {
        uint8_t r;
        uint8_t g;
        uint8_t b;
};

Vec3 pixel_to_vec3 (struct pixel p)
{
        return Vec3 (p.r, p.g, p.b);
}

double pixel_height (struct pixel p)
{
        Vec3 px = pixel_to_vec3 (p);

        return (px / 255.0).length ();
}

void usage ()
{
        std::cerr << "usage: heightmapconv -f | --map [FILENAME] -e | --epsilon [EPSILON]\n";
        std::cerr << "FILENAME: height map file\n";
        std::cerr << "EPSILON: height map step size\n";
}

int main (int argc, char **argv)
{
        int c, optindex;

        struct option longopts[] = {
                { .name = "map", .has_arg = 1, .val = 'f' },
                { .name = "epsilon", .has_arg = 1, .val = 'e' },
                { .name = "outfile", .has_arg = 1, .val = 'o' },
                { 0 }
        };

        char *filename = NULL, *outfile_name = NULL;
        double eps = 1e-2;

        while ((c = getopt_long (argc, argv, "hf:o:e:", longopts, &optindex)) != EOF) {
                switch (c) {
                case 'f': filename = optarg; break;
                case 'e': eps = strtod (optarg, NULL); break;
                case 'o': outfile_name = optarg; break;
                case 'h': {
                        usage ();
                        exit (EXIT_SUCCESS);
                }
                }
        }

        if (!filename) {
                std::cerr << "No height map given\n";
                usage ();
                exit (EXIT_FAILURE);
        }

        int width = 0, height = 0, nchannels = 0;

        struct pixel *pixels = (struct pixel *)stbi_load (filename, &width, &height, &nchannels, 3);

        std::ofstream file ((std::string (outfile_name)));

        progressbar bar ((height - 1));

        file << "P3\n";
        file << width - 2 << ' ' << height - 2 << "\n";
        file << "255\n";

#define P(i, j) pixels[(j) * width + (i)]

        for (int j = 1; j < height - 1; j++) {
                bar.update ();
                for (int i = 1; i < width - 1; i++) {
                        double x1 = pixel_height (P (i - 1, j));
                        double x2 = pixel_height (P (i + 1, j));

                        double y1 = pixel_height (P (i, j - 1));
                        double y2 = pixel_height (P (i, j + 1));

                        double dx = (x2 - x1) / (2 * eps);
                        double dy = (y2 - y1) / (2 * eps);

                        Vec3 v (-dx, -dy, 1);

                        v = (v.unit () + Vec3 (1, 1, 1)) * (255.0 / 2);

                        int r = int (v.x), g = int (v.y), b = int (v.z);

                        file << r << ' ' << g << ' ' << b << '\n';
                }
        }

        file.close ();

#undef P
}