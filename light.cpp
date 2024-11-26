#include "light.hpp"
#include "vec3.hpp"

Light::Light (Vec3 location, Vec3 Id, Vec3 Is) : location (location), diffuse_intensity (Id), specular_intensity (Is)
{
}