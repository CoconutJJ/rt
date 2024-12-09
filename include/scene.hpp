#pragma once
#include "world.hpp"
#include <string>
class SceneFile {

    public:
        SceneFile(std::string filename);

        World *generate_world();


};