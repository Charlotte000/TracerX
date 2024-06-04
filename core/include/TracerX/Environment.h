#pragma once

#include "Texture.h"

#include <string>
#include <glm/glm.hpp>

namespace TracerX
{

class Environment
{
public:
    std::string name = "None";
    float intensity = 1.f;
    glm::mat3 rotation = glm::mat3(1);
    bool transparent = false;

    void reset();
    void loadFromFile(const std::string& fileName);
private:
    core::Texture texture;

    friend class Renderer;
};

}
