/**
 * @file Vertex.h
 */
#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

/**
 * @brief Represents a vertex in a 3D model.
 */
struct Vertex
{
public:
    /**
     * @brief The position of the vertex in 3D space (xyz) and the U component of the texture coordinates (w).
     */
    glm::vec4 positionU;

    /**
     * @brief The normal of the vertex in 3D space (xyz) and the V component of the texture coordinates (w).
     */
    glm::vec4 normalV;
};

}
