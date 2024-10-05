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
     * @brief The size of the triangles in the mesh.
     * 
     * Optional, does not affect the rendering of the mesh.
     */
    int triangleSize = 0;
private:
    int nodeOffset = 0;
    int triangleOffset = 0;

    friend class Scene;
};

/**
 * @brief Represents a mesh instance in the TracerX system.
 */
struct MeshInstance
{
public:
    /**
     * @brief The transformation matrix for the mesh instance.
     * @see Mesh::transformInv
     */
    glm::mat4 transform = glm::mat4(1);

    /**
     * @brief The inverse transformation matrix for the mesh instance.
     * 
     * Must be updated whenever the transform matrix is updated.
     */
    glm::mat4 transformInv = glm::mat4(1);

    /**
     * @brief The material ID of the mesh instance.
     * 
     * The material ID is the index of the Scene::materials list.
     */
    int materialId = -1;

    /**
     * @brief The mesh ID of the mesh instance.
     * 
     * The mesh ID is the index of the Scene::meshes list.
     */
    int meshId = -1;
private:
    int padding1;
    int padding2;
};

}
