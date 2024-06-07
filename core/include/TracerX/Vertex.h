#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

/// @brief Represents a vertex in a 3D model.
struct Vertex
{
public:
    /// @brief The position of the vertex in 3D space.
    glm::vec3 position;

    /// @brief The normal vector of the vertex.
    glm::vec3 normal;

    /// @brief The texture coordinates of the vertex.
    glm::vec2 uv;
private:
    float padding1 = 0;
};

}
