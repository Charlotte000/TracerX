#pragma once

#include "Texture.h"

#include <string>
#include <glm/glm.hpp>

namespace TracerX::core
{

struct Environment
{
    std::string name = "None";
    float intensity = 1.f;
    glm::mat3 rotation = glm::mat3(1);
    bool transparent = false;
    Texture texture;

    void reset();
    void loadFromFile(const std::string& fileName);
};

}
