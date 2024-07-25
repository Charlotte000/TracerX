/**
 * @file BVH.h
 */
#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

struct Node
{
public:
    glm::vec4 bboxMin;
    glm::vec4 bboxMax;
    int start;
    int primitiveCount;
    int rightOffset;
private:
    int padding;
};

}
