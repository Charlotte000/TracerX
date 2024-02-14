#pragma once

#include "Image.h"

#include <vector>
#include <FastBVH.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>


struct Material
{
public:
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

struct Index
{
    int vertex;
    int normal;
    int uv;
};

struct Triangle
{
public:
    Index v1;
    Index v2;
    Index v3;
    int meshId;
private:
    int padding1 = 0;
    int padding2 = 0;
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
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvCoords;
    std::vector<Triangle> triangles;
    std::vector<Image> textures;
    std::vector<Material> materials;
    std::vector<std::string> materialNames;
    std::vector<Mesh> meshes;
    std::vector<std::string> meshNames;
    Image environment = Image::empty;
    std::string name = "Empty";

    void loadOBJ(const std::string& fileName);
    void loadEnvironmentMap(const std::string& fileName);
    int loadTexture(const std::string& fileName);
    int loadMaterial(const Material& material, const std::string& name);
    const std::vector<glm::vec3> createBVH();

    static Scene load(const std::string& folder);
    static Scene loadGLTF(const std::string& folder);
};
