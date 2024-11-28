#include "quad_light.hpp"
#include "light.hpp"
#include "object.hpp"
#include "utils.hpp"
#include "vec3.hpp"

QuadLight::QuadLight (Quad *quad) : quad (quad), Light ((Object *)quad)
{
}

bool QuadLight::is_point_light ()
{
        return false;
}

Vec3 QuadLight::sample_point ()
{
        return this->quad->get_point (random_double (0, 1), random_double (0, 1));
}

Vec3 QuadLight::diffuse_intensity (Vec3 point)
{
        Vec3 uv = this->quad->to_texture_uv (point);

        return this->quad->material ()->texture->read_texture_uv (uv);
}

Vec3 QuadLight::specular_intensity (Vec3 point)
{
        return this->diffuse_intensity (point);
}