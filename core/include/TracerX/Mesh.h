#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

struct Mesh
{
public:
    glm::mat4 transform = glm::mat4(1);
    float materialId = -1;
    float triangleSize = 0;
private:
    float padding1 = 0;
    float padding2 = 0;
};

}
