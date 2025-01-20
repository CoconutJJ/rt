#include "phong.hpp"
#include "hitrecord.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include "vec3.hpp"

Phong::Phong (double rs,
              double rd,
              double ra,
              double rg,
              double shininess,
              double gamma,
              double mu,
              Texture *texture,
              Texture *normal_map)
        : Material (texture, normal_map), rs (rs), rd (rd), ra (ra), rg (rg), shininess (shininess), gamma (gamma),
          mu (mu)
{
}

Phong::Phong (double rs, double rd, double ra, double rg, double shininess, double gamma, double mu, Texture *texture)
        : Material (texture, nullptr), rs (rs), rd (rd), ra (ra), rg (rg), shininess (shininess), gamma (gamma), mu (mu)
{
}
Phong::PhongParams Phong::phong (Ray r, HitRecord &record)
{
        return PhongParams{ .rs = this->rs,
                            .ra = this->ra,
                            .rd = this->rd,
                            .alpha = this->shininess,
                            .gamma = this->gamma,
                            .mu = this->mu,
                            .color = this->texture->read_texture_uv (record.uv, record.hit_point),
                            .rg = this->rg };
}
