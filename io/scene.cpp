#include "scene.hpp"
#include "object.hpp"
#include <cstring>
#include <iostream>
#include <string>

#include "toml.hpp"

SceneFile::SceneFile (std::string filename)
{
        auto scene = toml::parse_file (filename);

        std::map<std::string, Object *> object_map;

        for (auto key = scene.cbegin (); key != scene.cend (); key++) {
                auto table_key = key->first.data ();
                auto table_values = key->second.as_table ();

                if (scene[table_key]["object"] == "sphere") {
                        auto radius = scene[table_key]["radius"].value<int> ().value ();
                }
        }
}
