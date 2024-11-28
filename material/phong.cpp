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
              Texture *texture)
        : rs (rs), rd (rd), ra (ra), rg (rg), shininess (shininess),
          Material (texture)
{
}

Phong::PhongParams Phong::phong (Ray r, HitRecord &record)
{
        return PhongParams{ .ra = this->ra,
                            .rd = this->rd,
                            .rs = this->rs,
                            .alpha = this->shininess,
                            .color = this->texture->read_texture_uv (record.uv),
                            .rg = this->rg };
}
