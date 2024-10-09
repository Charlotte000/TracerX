/**
 * @file BvhNode.h
 */
#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

/**
 * @brief The element of the Bound Volume Hierarchy (BVH) tree.
 * 
 * A BVH is a tree structure used to accelerate path tracing by reducing the number of intersection tests.
 * The BVH is constructed from the bounding boxes of the scene's triangles.
 * 
 * @remark Should not be used directly and is only used internally by the renderer.
 */
struct BvhNode
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
