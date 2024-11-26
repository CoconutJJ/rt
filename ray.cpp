#include "ray.hpp"
#include "vec3.hpp"
#include <cstdlib>

Ray::Ray ()
{
}

Ray::Ray (Vec3 origin, Vec3 direction, double time) : origin (origin), direction (direction), time (time)
{
}

Ray::Ray (Vec3 origin, Vec3 direction) : origin (origin), direction (direction)
{
}

Vec3 Ray::at (double t)
{
        return this->origin + this->direction * t * (1 - 0.00001);
}

Ray Ray::nudge (double maxX, double maxY)
{
        double dx = std::rand () / RAND_MAX - 0.5;
        double dy = std::rand () / RAND_MAX - 0.5;

        Ray r (this->origin, this->direction + Vec3 (dx * maxX, dy * maxY, 0));

        return r;
}