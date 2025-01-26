#include "light.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "smooth_object.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <iostream>
#include <ostream>
#include <vector>

class Camera {
    public:
        struct RendererSettings {
                int image_width;
                int arealight_samples;
                int samples_per_pixel;
                int max_depth;

                double vfov;
                double aspect_ratio;
                double defocus_angle;

                bool use_path_tracer;
                bool use_light_sampling;
                bool use_scene_sig;
                bool use_importance_sampling;
                Texture *background_texture;
        };

        std::ostream &stream;

        Camera ();
        void initialize (struct RendererSettings settings);
        void render (World *world, const char *filename);
        void render_multithreaded (World *world, const char *filename, int max_threads);
        Ray ray (int i, int j);
        Vec3 ray_color (Ray r, World *world, int depth);
        Vec3 single_path_color (Ray starting_ray, World *world, int max_depth);
        Vec3 scene_signature_color (Ray starting_ray, World *world);
        Vec3 sample_pixel (World *world, int i, int j);
        Vec3 sample_light_rays (World *world, HitRecord &record, Light *light, Material::PhongParams params, int K);
        Vec3 sample_light (World *world, HitRecord &record, SmoothObject *&hit_light);
        Vec3 defocus_disk_sample ();
        void print_arguments ();
        void export_p6 (const char *filename, std::vector<Vec3> pixels);

    private:
        int image_width;
        int image_height;
        int samples_per_pixel;
        int max_depth;
        int arealight_samples;

        double aspect_ratio;
        double viewport_width;
        double viewport_height;
        double vfov;
        double defocus_angle;
        double focus_dist;

        bool use_path_tracer;
        bool use_light_sampling;
        bool use_importance_sampling;
        bool use_scene_sig;

        Vec3 defocus_disk_u;
        Vec3 defocus_disk_v;
        Vec3 pixel_du;
        Vec3 pixel_dv;
        Vec3 pixel_00;
        Vec3 center;
        Vec3 lookat;

        Texture *background_texture;
};
