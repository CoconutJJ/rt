#include "light.hpp"
#include "object.hpp"
#include "vec3.hpp"

Light::Light () : object (nullptr)
{
}

Light::Light (Object *object) : object (object)
{
}
