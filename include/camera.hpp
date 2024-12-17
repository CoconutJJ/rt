#include "light.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <iostream>
#include <ostream>

class Camera {
    public:
        double aspect_ratio;
        int image_width;
        int image_height;
        double viewport_width;
        double viewport_height;
        int max_depth;
        int samples_per_pixel;
        double vfov;
        double defocus_angle;
        double focus_dist;
        int arealight_samples;
        Vec3 defocus_disk_u;
        Vec3 defocus_disk_v;

        Vec3 pixel_du;
        Vec3 pixel_dv;
        Vec3 pixel_00;
        Vec3 center;
        Vec3 lookat;

        std::ostream &stream;

        Camera ();
        void initialize (double aspect_ratio,
                         int image_width,
                         double vfov,
                         double defocus_angle,
                         int arealight_samples,
                         int samples_per_pixel);
        void render (World *world, const char *filename);
        void render_multithreaded (World *world, const char *filename, int max_threads);
        void path_render (World *world, const char *filename, int max_threads);
        void set_output_file (const char *filename);
        Ray ray (int i, int j);
        Vec3 ray_color (Ray r, World *world, int depth);
        Vec3 path_color (Ray r, World *world, int depth);
        void write_color (Vec3 color);
        Vec3 sample_light_rays (World *world, HitRecord &record, Light *light, Material::PhongParams params, int K);
        Vec3 defocus_disk_sample ();
};
