/**
 * @file Mesh.h
 */
#pragma once

#include <glm/glm.hpp>

namespace TracerX
{

/**
 * @brief Represents a mesh in the TracerX system.
 */
struct Mesh
{
public:
    /**
     * @brief The transformation matrix for the mesh.
     * @see Mesh::transformInv
     */
    glm::mat4 transform = glm::mat4(1);

    /**
     * @brief The inverse transformation matrix for the mesh.
     * 
     * Must be updated whenever the transform matrix is updated.
     */
    glm::mat4 transformInv = glm::mat4(1);

    /**
     * @brief The material ID of the mesh.
     * 
     * The material ID is the index of the material in the scene's material list.
     * Float value to avoid padding issues.
     */
    float materialId = -1;
private:
    float nodeOffset = 0;
    float triangleOffset = 0;
public:
    /**
     * @brief The size of the triangles in the mesh.
     * 
     * Optional, does not affect the rendering of the mesh.
     * Float value to avoid padding issues.
     */
    float triangleSize = 0;

    friend class Scene;
};

}
