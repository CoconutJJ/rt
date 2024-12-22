#include "ray.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdlib>

Ray::Ray ()
{
}

Ray::Ray (Vec3 origin, Vec3 direction, double time) : origin (origin), direction (direction), time (time)
{
}

Ray::Ray (Vec3 origin, Vec3 direction) : origin (origin), direction (direction), time (0)
{
}

Ray::Ray (Vec3 origin, Vec3 direction, Vec3 color) : origin (origin), direction (direction), color (color)
{
}

Vec3 Ray::at (double t)
{
        return this->origin + this->direction * t * (1 - 0.0001);
}

Ray Ray::nudge (double maxX, double maxY)
{
        double dx = std::rand () / RAND_MAX - 0.5;
        double dy = std::rand () / RAND_MAX - 0.5;

        Ray r (this->origin, this->direction + Vec3 (dx * maxX, dy * maxY, 0));

        return r;
}

bool Ray::can_refract (Vec3 normal, double mu)
{
        double cos_theta = std::fmin (-this->direction.unit ().dot (normal), 1.0);
        double sin_theta = std::sqrt (1 - cos_theta * cos_theta);

        return mu * sin_theta <= 1.0;
}