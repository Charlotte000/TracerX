#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

/// @brief Represents a mesh in the TracerX system.
struct Mesh
{
public:
    /// @brief The transformation matrix for the mesh.
    glm::mat4 transform = glm::mat4(1);

    /// @brief The material ID of the mesh.
    float materialId = -1;

    /// @brief The size of the triangles in the mesh. (optional does not affect the rendering of the mesh)
    float triangleSize = 0;
private:
    float padding1 = 0;
    float padding2 = 0;
};

}
