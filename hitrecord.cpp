#include "hitrecord.hpp"

HitRecord::HitRecord () : hit_point (0, 0, 0), normal (0, 0, 0)
{
}

void HitRecord::setNormal (Ray r, Vec3 normal)
{
        if (r.direction.dot (normal) > 0)
                normal = -normal;

        this->normal = normal;
}
