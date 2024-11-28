#include "material.hpp"
#include "texture.hpp"

Material::Material() {}
Material::Material(Texture *texture) : texture(texture) {}