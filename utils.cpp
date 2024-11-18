#include "utils.hpp"
#include <cstdlib>
double clamp (double min, double x, double max)
{
        if (x < min)
                return min;
        if (x > max)
                return max;

        return x;
}

double random_double(double min, double max) {

    return  min + (max - min) * ((double)std::rand() / RAND_MAX);

}
