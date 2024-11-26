#include "lambertian.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "solid_texture.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include <cstddef>

Lambertian::Lambertian (Vec3 albedo) : Material ()
{
        // TODO: free this...
        this->texture = new SolidTexture (albedo);
}

Lambertian::Lambertian (Texture *texture) : Material (), texture (texture)
{
        this->texture = texture;
}

bool Lambertian::scatter (Ray r, HitRecord &record, Vec3 &attenuation, Ray &scattered)
{
        Vec3 scatter_direction = record.normal.unit () + Vec3::random ().unit ();

        if (scatter_direction.near_zero ())
                scatter_direction = record.normal;

        scattered = Ray (record.hit_point, scatter_direction, r.time);

        attenuation = this->texture->read_texture_uv (record.uv);

        return true;
}