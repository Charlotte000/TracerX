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


class Scene
{
public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Image> textures;
    std::vector<std::string> textureNames;
    std::vector<Material> materials;
    std::vector<std::string> materialNames;
    std::vector<Mesh> meshes;
    std::vector<std::string> meshNames;
    std::vector<Camera> cameras;
    Image environment = Image::empty;
    std::string environmentName = "None";
    std::string name = "Empty";

    void loadEnvironmentMap(const std::string& fileName);
    int loadTexture(const std::string& fileName);
    int loadMaterial(const Material& material, const std::string& name);
    const std::vector<glm::vec3> createBVH();

    static Scene loadGLTF(const std::string& folder);
private:
    void GLTFtextures(const std::vector<tinygltf::Texture>& textures, const std::vector<tinygltf::Image>& images);
    void GLTFmaterials(const std::vector<tinygltf::Material>& materials);
    void GLTFmesh(const tinygltf::Model& model, const tinygltf::Mesh& gltfMesh, const glm::mat4 transform);
    void GLTFcamera(const tinygltf::Camera& camera, const glm::mat4 tranform);
    void GLTFnodes(const tinygltf::Model& model, const glm::mat4& world);
    void GLTFtraverseNode(const tinygltf::Model& model, const tinygltf::Node& node, const glm::mat4& globalTransform);
};
