#include "lambertian.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>

Lambertian::Lambertian (Vec3 albedo) : Material ()
{
        // TODO: free this...
        this->texture = new SolidTexture (albedo);
}

Lambertian::Lambertian (Texture *texture) : Material (), texture (texture)
{
        this->texture = texture;
}

Vec3 Lambertian::scatter (Ray r, HitRecord &record)
{
        return Vec3 (1, random_double (0, 2 * M_PI), random_double (0, M_PI / 2));
}
