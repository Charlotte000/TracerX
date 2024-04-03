#pragma once

#include "Image.h"
#include "Camera.h"

#include <vector>
#include <string>
#include <FastBVH.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>


struct Material
{
    glm::vec3 albedoColor = glm::vec3(1);
    float roughness = 0;
    glm::vec3 emissionColor = glm::vec3(1);
    float emissionStrength = 0;
    glm::vec3 fresnelColor = glm::vec3(1);
    float fresnelStrength = 0;
    float metalness = 0;
    float ior = 0;
    float density = 0;
    float albedoTextureId = -1;
    float metalnessTextureId = -1;
    float emissionTextureId = -1;
    float roughnessTextureId = -1;
    float normalTextureId = -1;

    static Material lightSource(glm::vec3 emissionColor, float emissionStrength);
    static Material transparent(glm::vec3 albedoColor, float ior, glm::vec3 fresnelColor = glm::vec3(1), float fresnelStrength = 0);
    static Material constantDensity(glm::vec3 albedoColor, float density);
    static Material matte(glm::vec3 albedoColor, float metalness = 0);
    static Material mirror();
};

struct Vertex
{
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
private:
    float padding1 = 0;
};

struct Triangle
{
    int v1;
    int v2;
    int v3;
    int meshId;
};

struct Mesh
{
public:
    glm::mat4 transform = glm::mat4(1);
    float materialId = -1;
    float triangleSize = 0;
private:
    float padding1 = 0;
    float padding2 = 0;
};

class Scene
{
public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Image> textures;
    std::vector<Material> materials;
    std::vector<std::string> materialNames;
    std::vector<Mesh> meshes;
    std::vector<std::string> meshNames;
    std::vector<Camera> cameras;
    Image environment = Image::empty;
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
