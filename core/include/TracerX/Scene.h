/**
 * @file Scene.h
 */
#pragma once

#include "TracerX/Mesh.h"
#include "TracerX/Image.h"
#include "TracerX/Camera.h"
#include "TracerX/Material.h"
#include "TracerX/core/Vertex.h"
#include "TracerX/core/BvhNode.h"
#include "TracerX/core/Triangle.h"

#include <string>

namespace TracerX
{

/**
 * @brief A collection of all the data that makes up a 3D scene to be rendered.
 * @see TracerX::loadGLTF to load a scene from a GLTF file.
 */
class Scene
{
public:
    /**
     * @brief The vertices of the scene.
     * 
     * The index of the vertex in the vector is the Triangle::v1, Triangle::v2, or Triangle::v3 of the triangles in the scene.
     */
    std::vector<core::Vertex> vertices;

    /**
     * @brief The triangles of the scene.
     * 
     * The range of the triangles in the vector is [Mesh::triangleOffset, Mesh::triangleOffset + Mesh::triangleSize) for each mesh in the scene.
     */
    std::vector<core::Triangle> triangles;

    /**
     * @brief The textures used in the scene.
     * 
     * The index of the texture in the vector is the material texture ID used in the scene.
     */
    std::vector<Image> textures;

    /**
     * @brief The names of the textures.
     * 
     * Does not affect the rendering of the scene and is used for debugging and UI purposes.
     */
    std::vector<std::string> textureNames;

    /**
     * @brief The materials used in the scene.
     * 
     * The index of the material in the vector is the MeshInstance::materialId used in the scene.
     */
    std::vector<Material> materials;

    /**
     * @brief The names of the materials.
     * 
     * Does not affect the rendering of the scene and is used for debugging and UI purposes.
     */
    std::vector<std::string> materialNames;

    /**
     * @brief The meshes in the scene.
     * 
     * The index of the mesh in the vector is the MeshInstance::meshId used in the scene.
     * New meshes should be added only using the Scene::addMesh method.
     */
    std::vector<Mesh> meshes;

    /**
     * @brief The names of the meshes.
     * 
     * Does not affect the rendering of the scene and is used for debugging and UI purposes.
     */
    std::vector<std::string> meshNames;

    /**
     * @brief The mesh instances in the scene.
     */
    std::vector<MeshInstance> meshInstances;

    /**
     * @brief The default cameras in the scene.
     */
    std::vector<Camera> cameras;

    /**
     * @brief Adds a texture to the scene.
     * @param texture The texture to be added.
     * @param name The name of the texture.
     * @return The index (texture ID) of the added texture in the Scene::textures vector.
     */
    int addTexture(const Image& texture, const std::string& name);

    /**
     * @brief Adds a material to the scene.
     * @param material The material to be added.
     * @param name The name of the material.
     * @return The index (material ID) of the added material in the Scene::materials vector.
     */
    int addMaterial(const Material& material, const std::string& name);

    /**
     * @brief Adds a mesh to the scene.
     * 
     * Builds a BVH tree for the mesh. May take a long time for large meshes.
     * 
     * @param mesh The mesh to be added.
     * @param name The name of the mesh.
     * @return The index (mesh ID) of the added mesh in the Scene::meshes vector.
     */
    int addMesh(const Mesh& mesh, const std::string& name);
private:
    std::vector<core::BvhNode> blas;

    void buildBLAS(Mesh& mesh);
    void buildTLAS(std::vector<core::BvhNode>& tlas, std::vector<size_t>& permutation);

    friend class Renderer;
};

}
