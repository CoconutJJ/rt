#include "scene_parser.hpp"
#include "image_texture.hpp"
#include "lambertian.hpp"
#include "material.hpp"
#include "metal.hpp"
#include "solid_texture.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

void SceneParser::parse_texture (json &obj)
{
        if (!obj.contains ("name")) {
                std::cerr << "Texture must include key `name`";
                exit (EXIT_FAILURE);
        }

        std::string name = obj["name"];
        Texture *texture = nullptr;

        if (obj.contains ("file")) {
                texture = new ImageTexture (obj["file"].get<std::string> ().c_str ());
        } else if (obj.contains ("color")) {
                json &rgb = obj["color"];

                if (!rgb.contains ("r")) {
                        std::cerr << "Texture " << name << " is declared as a solid color, but missing `r` key";
                        exit (EXIT_FAILURE);
                }

                if (!rgb.contains ("g")) {
                        std::cerr << "Texture " << name << " is declared as a solid color, but missing `g` key";
                        exit (EXIT_FAILURE);
                }

                if (!rgb.contains ("b")) {
                        std::cerr << "Texture " << name << " is declared as a solid color, but missing `b` key";
                        exit (EXIT_FAILURE);
                }

                int r = obj["color"]["r"], g = obj["color"]["g"], b = obj["color"]["b"];

                texture = new SolidTexture (Vec3 (r / 255.0, g / 255.0, b / 255.0));
        }

        this->textures[obj["name"]] = texture;
}

void SceneParser::parse_material (json &obj)
{
        if (!obj.contains ("name")) {
                std::cerr << "Material must specify `name` key";
                exit (EXIT_FAILURE);
        }

        std::string name = obj["name"];

        if (!obj.contains ("type")) {
                std::cerr << "Missing `type` key for material";
                exit (EXIT_FAILURE);
        }

        Material *material;

        if (obj["type"] == "lambertian") {
                if (!obj.contains ("reflectivity")) {
                        std::cerr << "Missing `reflectivity` key for Lambertian Material: " << name;
                        exit (EXIT_FAILURE);
                }

                double reflectivity = obj["reflectivity"];

                if (!obj.contains ("texture")) {
                        std::cerr << "Missing `texture` key for Lambertian Material: " << name;
                        exit (EXIT_FAILURE);
                }

                material = new Lambertian (reflectivity, this->textures[name]);

        } else if (obj["type"] == "Metal") {
        }
}

Sphere *SceneParser::parse_sphere (SceneParser::json &obj)
{
        double radius = obj["radius"];

        double x = obj["x"];
        double y = obj["y"];
        double z = obj["z"];

        return new Sphere (Vec3 (x, y, z), radius, this->read_material (obj["material"]));
}

World SceneParser::parse (const char *filename)
{
        std::ifstream scene_file ((std::string (filename)));

        json scene = json::parse (scene_file);

        World world;

        for (auto &obj : scene) {
        
                
        }
}