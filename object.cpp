#include "object.hpp"
#include "material.hpp"
#include "vec3.hpp"



Object::Object(Vec3 location, Material *material) : location(location), mat(material) {
    
}