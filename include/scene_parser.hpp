#pragma once
#include "json.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "world.hpp"
#include "sphere.hpp"
#include <map>
#include <string>

class SceneParser {
    public:
        using json = nlohmann::json;
        std::map<std::string, Material *> materials;
        std::map<std::string, Texture *> textures;
        bool has_errors;
        SceneParser ();

        World parse (const char *filename);
        void parse_texture(json &obj);
        void parse_material (json &obj);
        Material *read_material (std::string name);
        Sphere *parse_sphere(SceneParser::json &obj);
};