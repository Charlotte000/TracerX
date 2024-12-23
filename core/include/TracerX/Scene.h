/**
 * @file Scene.h
 */
#pragma once

#include "Mesh.h"
#include "Image.h"
#include "Camera.h"
#include "Vertex.h"
#include "Material.h"
#include "Triangle.h"

#include <vector>
#include <string>
#include <FastBVH.h>
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
     * @brief The default cameras in the scene.
     */
    std::vector<Camera> cameras;

    /**
     * @brief The name of the scene.
     */
    std::string name = "Empty";

    /**
     * @brief Loads a texture from a file and adds it to the scene.
     * @param fileName The name of the file containing the texture.
     * @return The index (texture ID) of the loaded texture in the textures vector.
     */
    int loadTexture(const std::string& fileName);

    /**
     * @brief Loads a material and adds it to the scene.
     * @param material The material to be loaded.
     * @param name The name of the material.
     * @return The index (material ID) of the loaded material in the materials vector.
     */
    int loadMaterial(const Material& material, const std::string& name);

    /**
     * @brief Loads a scene from a GLTF file.
     * @param fileName The name of the file to load the GLTF scene from.
     * @return The loaded scene.
     * @throws std::runtime_error Thrown if the GLTF file fails to load.
     */
    static Scene loadGLTF(const std::string& fileName);
private:
    std::vector<glm::vec3> bvh;

    void GLTFtextures(const std::vector<tinygltf::Texture>& textures, const std::vector<tinygltf::Image>& images);
    void GLTFmaterials(const std::vector<tinygltf::Material>& materials);
    void GLTFmesh(const tinygltf::Model& model, const tinygltf::Mesh& gltfMesh, const glm::mat4 transform);
    void GLTFcamera(const glm::mat4 transform);
    void GLTFnodes(const tinygltf::Model& model, const glm::mat4& world);
    void GLTFtraverseNode(const tinygltf::Model& model, const tinygltf::Node& node, const glm::mat4& globalTransform);
    void buildBVH(Mesh& mesh);

    friend class Renderer;
};

}
