#pragma once

#include <glm/glm.hpp>


struct Vertex
{
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
private:
    float padding1 = 0;
};
