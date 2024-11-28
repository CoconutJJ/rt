#include "scene_parser.hpp"
#include <fstream>
#include <regex>
#include <string>

std::string stripWhitespace (std::string s)
{
        std::string stripped;

        for (char c : s) {
                switch (c) {
                case '\n':
                case '\r':
                case '\t': continue;

                default: stripped.push_back (c);
                }
        }

        return stripped;
}

void SceneParser::parse (const char *filename)
{
        std::ifstream scene_file ((std::string (filename)));

        for (std::string line; std::getline (scene_file, line);) {
                std::regex object_regex ("\\[[a-zA-Z_]+\\]");
                std::smatch object_matches;
                line = stripWhitespace (line);

                if (std::regex_search (line, object_matches, object_regex)) {
                        std::string object_name = object_matches[1];
                }
        }
}