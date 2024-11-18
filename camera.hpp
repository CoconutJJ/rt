#include "object.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "world.hpp"

class Camera {
    public:
        double aspect_ratio;
        int image_width;
        int image_height;
        double viewport_width;
        double viewport_height;
        double focal_length;
        int max_depth;
        int samples_per_pixel;
        
        Vec3 pixel_du;
        Vec3 pixel_dv;
        Vec3 pixel_00;
        Vec3 center;


        Camera (double aspect_ratio, int image_width);

        void render(World * world);
        Ray ray (int i, int j);
        Vec3 ray_color(Ray r, World *world, int depth);
        void write_color(Vec3 color);
};
