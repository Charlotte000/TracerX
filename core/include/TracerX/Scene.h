/**
 * @file Scene.h
 */
#pragma once

#include "BVH.h"
#include "Mesh.h"
#include "Image.h"
#include "Camera.h"
#include "Vertex.h"
#include "Material.h"
#include "Triangle.h"

#include <map>
#include <vector>
#include <string>
#include <FastBVH.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <tiny_gltf.h>

namespace TracerX
{

/**
 * @brief Represents a scene in the TracerX rendering engine.
 */
class Scene
{
public:
    /**
     * @brief The vertices of the scene.
     */
    std::vector<core::Vertex> vertices;

    /**
     * @brief The triangles of the scene.
     */
    std::vector<core::Triangle> triangles;

    /**
     * @brief The textures used in the scene.
     * 
     * The index of the texture in the vector is the texture ID used in the scene.
     */
    std::vector<Image> textures;

    /**
     * @brief The names of the textures.
     */
    std::vector<std::string> textureNames;

    /**
     * @brief The materials used in the scene.
     * 
     * The index of the material in the vector is the material ID used in the scene.
     */
    std::vector<Material> materials;

    /**
     * @brief The names of the materials.
     */
    std::vector<std::string> materialNames;

    /**
     * @brief The meshes in the scene.
     * 
     * The index of the mesh in the vector is the mesh ID used in the scene.
     */
    std::vector<Mesh> meshes;

    /**
     * @brief The names of the meshes.
     */
    std::vector<std::string> meshNames;

    /**
     * @brief The meshe instances in the scene.
     * 
     * The index of the mesh instance in the vector is the mesh instance ID used in the scene.
     */
    std::vector<MeshInstance> meshInstances;

    /**
     * @brief The default cameras in the scene.
     */
    std::vector<Camera> cameras;

    /**
     * @brief Loads a texture from a file and adds it to the scene.
     * @param path The path of the file containing the texture.
     * @return The index (texture ID) of the loaded texture in the Scene::textures vector.
     */
    int loadTexture(const std::filesystem::path& path);

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
     * @param mesh The mesh to be added.
     * @param name The name of the mesh.
     * @return The index (mesh ID) of the added mesh in the Scene::meshes vector.
     */
    int addMesh(const Mesh& mesh, const std::string& name);

    /**
     * @brief Loads a scene from a GLTF file.
     * @param path The path of the file to load the GLTF scene from.
     * @return The loaded scene.
     * @throws std::runtime_error Thrown if the GLTF file fails to load.
     */
    static Scene loadGLTF(const std::filesystem::path& path);
private:
    std::vector<core::Node> bvh;

    std::map<int, std::vector<glm::ivec2>> GLTFmeshes(const tinygltf::Model& model);
    void GLTFtextures(const std::vector<tinygltf::Texture>& textures, const std::vector<tinygltf::Image>& images);
    void GLTFmaterials(const std::vector<tinygltf::Material>& materials);
    void GLTFnodes(const tinygltf::Model& model, const std::map<int, std::vector<glm::ivec2>>& meshMap, const glm::mat4& world);
    void GLTFtraverseNode(const tinygltf::Model& model, const tinygltf::Node& node, const std::map<int, std::vector<glm::ivec2>>& meshMap, const glm::mat4& globalTransform);
    void buildBVH(Mesh& mesh);

    friend class Renderer;
};

}
