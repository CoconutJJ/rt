#include "light.hpp"
#include "smooth_object.hpp"
#include "vec3.hpp"

Light::Light () : object (nullptr)
{
}

Light::Light (SmoothObject *object) : object (object)
{
}
