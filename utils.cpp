#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdlib>
double clamp (double min, double x, double max)
{
        if (x < min)
                return min;
        if (x > max)
                return max;

        return x;
}

double random_double (double min, double max)
{
        return min + (max - min) * ((double)std::rand () / RAND_MAX);
}

double deg2rad (double deg)
{
        return M_PI / 180.0 * deg;
}

Vec3 random_in_unit_disk ()
{
        while (true) {
                Vec3 p = Vec3::random ();

                if (p.length () < 1)
                        return p;
        }
}