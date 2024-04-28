#define TINYGLTF_IMPLEMENTATION

#include "TracerX/Scene.h"

#include <stdexcept>
#include <tinydir.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace TracerX;
using namespace TracerX::core;

void Scene::loadEnvironmentMap(const std::string& fileName)
{
    this->environment = Image::loadFromFile(fileName);
    this->environmentName = fileName.substr(fileName.find_last_of("/\\") + 1);
}

int Scene::loadTexture(const std::string& fileName)
{
    this->textures.push_back(Image::loadFromFile(fileName));
    return this->textures.size() - 1;
}

int Scene::loadMaterial(const Material& material, const std::string& name)
{
    this->materials.push_back(material);
    this->materialNames.push_back(name);
    return this->materials.size() - 1;
}

const std::vector<glm::vec3> Scene::createBVH()
{
    if (this->triangles.empty())
    {
        return std::vector<glm::vec3>();
    }

    class TriangleConverter
    {
    public:
        const std::vector<Vertex>* vertices;
        const std::vector<Mesh>* meshes;

        TriangleConverter(const std::vector<Vertex>* vertices, const std::vector<Mesh>* meshes)
            : vertices(vertices), meshes(meshes)
        {
        }

        FastBVH::BBox<float> operator()(const Triangle& triangle) const noexcept
        {
            const Mesh& mesh = this->meshes->at(triangle.meshId);

            glm::vec3 v1 = mesh.transform * glm::vec4(this->vertices->at(triangle.v1).position, 1);
            glm::vec3 v2 = mesh.transform * glm::vec4(this->vertices->at(triangle.v2).position, 1);
            glm::vec3 v3 = mesh.transform * glm::vec4(this->vertices->at(triangle.v3).position, 1);
            return FastBVH::BBox<float>(glm::min(glm::min(v1, v2), v3), glm::max(glm::max(v1, v2), v3));
        }
    };

    FastBVH::BVH<float, Triangle> bvh = FastBVH::DefaultBuilder<float>()(this->triangles, TriangleConverter(&this->vertices, &this->meshes));

    std::vector<glm::vec3> nodes;
    nodes.reserve(bvh.getNodes().size() * 3);
    for (const FastBVH::Node<float>& node : bvh.getNodes())
    {
        nodes.push_back(node.bbox.min);
        nodes.push_back(node.bbox.max);
        nodes.push_back(glm::vec3(node.start, node.primitive_count, node.right_offset));
    }

    return nodes;
}

Scene Scene::loadGLTF(const std::string& folder)
{
    Scene scene;
    scene.name = folder.substr(folder.find_last_of("/\\") + 1);

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;

    if (folder.substr(folder.find_last_of('.') + 1) == "glb")
    {
        if (!loader.LoadBinaryFromFile(&model, &err, nullptr, folder))
        {
            throw std::runtime_error(err);
        }
    }
    else
    {
        if (!loader.LoadASCIIFromFile(&model, &err, nullptr, folder))
        {
            throw std::runtime_error(err);
        }
    }

    scene.GLTFtextures(model.textures, model.images);
    scene.GLTFmaterials(model.materials);
    scene.GLTFnodes(model, glm::mat4(1));
    return scene;
}

void Scene::GLTFtextures(const std::vector<tinygltf::Texture>& textures, const std::vector<tinygltf::Image>& images)
{
    for (size_t textureId = 0; textureId < textures.size(); textureId++)
    {
        const tinygltf::Texture& gltfTexture = textures[textureId];
        const tinygltf::Image& gltfImage = images[gltfTexture.source];

        std::string name = gltfTexture.name != "" ? gltfTexture.name : std::to_string(textureId);
        glm::ivec2 size = glm::ivec2(gltfImage.width, gltfImage.height);

        std::vector<float> pixels;
        pixels.reserve(gltfImage.width * gltfImage.height * 3);
        for (size_t i = 0; i < gltfImage.image.size(); i += gltfImage.component)
        {
            pixels.push_back(gltfImage.image[i + 0] / 255.f);
            pixels.push_back(gltfImage.image[i + 1] / 255.f);
            pixels.push_back(gltfImage.image[i + 2] / 255.f);
        }

        this->textures.push_back(Image::loadFromMemory(size, pixels));
        this->textureNames.push_back(name);
    }
}

void Scene::GLTFmaterials(const std::vector<tinygltf::Material>& materials)
{
    for (const tinygltf::Material& gltfMaterial : materials)
    {
        const tinygltf::PbrMetallicRoughness& pbr = gltfMaterial.pbrMetallicRoughness;

        Material material;

        material.albedoColor = glm::vec3(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]);
        material.roughness = pbr.roughnessFactor;
        material.emissionColor = glm::vec3(gltfMaterial.emissiveFactor[0], gltfMaterial.emissiveFactor[1], gltfMaterial.emissiveFactor[2]);
        material.emissionStrength = 1;
        material.metalness = pbr.metallicFactor;

        material.albedoTextureId = pbr.baseColorTexture.index;
        material.metalnessTextureId = pbr.metallicRoughnessTexture.index;
        material.emissionTextureId = gltfMaterial.emissiveTexture.index;
        material.roughnessTextureId = pbr.metallicRoughnessTexture.index;
        material.normalTextureId = gltfMaterial.normalTexture.index;

        if (gltfMaterial.extensions.find("KHR_materials_ior") != gltfMaterial.extensions.end())
        {
            const tinygltf::Value& ext = gltfMaterial.extensions.find("KHR_materials_ior")->second;
            if (ext.Has("ior"))
            {
                material.ior = ext.Get("ior").Get<double>();
            }
        }

        if (gltfMaterial.extensions.find("KHR_materials_transmission") != gltfMaterial.extensions.end())
        {
            material.ior = 1;
        }

        if (gltfMaterial.extensions.find("KHR_materials_emissive_strength") != gltfMaterial.extensions.end())
        {
            const tinygltf::Value& ext = gltfMaterial.extensions.find("KHR_materials_emissive_strength")->second;
            if (ext.Has("emissiveStrength"))
            {
                material.emissionStrength = ext.Get("emissiveStrength").Get<double>();
            }
        }

        this->materials.push_back(material);
        this->materialNames.push_back(gltfMaterial.name);
    }

    if (this->materials.empty())
    {
        this->materials.push_back(Material::matte(glm::vec3(1)));
        this->materialNames.push_back("Default");
    }
}

void Scene::GLTFmesh(const tinygltf::Model& model, const tinygltf::Mesh& gltfMesh, const glm::mat4 transform)
{
    for (tinygltf::Primitive primitive : gltfMesh.primitives)
    {
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
        {
            continue;
        }

        const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes["POSITION"]];
        const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
        const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
        const uint8_t* positionBufferAddress = positionBuffer.data.data();

        const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
        const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
        const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
        const uint8_t* normalBufferAddress = normalBuffer.data.data();

        const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
        const tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];
        const tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];
        const uint8_t* uvBufferAddress = uvBuffer.data.data();

        size_t vertexOffset = this->vertices.size();
        for (size_t i = 0; i < positionAccessor.count; i++)
        {
            Vertex v;

            // Position
            const uint8_t* positionData = positionBufferAddress + positionBufferView.byteOffset + positionAccessor.byteOffset + (i * sizeof(float) * 3);
            std::memcpy(glm::value_ptr(v.position), positionData, sizeof(float) * 3);

            // Normal
            const uint8_t* normalData = normalBufferAddress + normalBufferView.byteOffset + normalAccessor.byteOffset + (i * sizeof(float) * 3);
            std::memcpy(glm::value_ptr(v.normal), normalData, sizeof(float) * 3);

            // Texture coordinate
            const uint8_t* uvData = uvBufferAddress + uvBufferView.byteOffset + uvAccessor.byteOffset + (i * sizeof(float) * 2);
            std::memcpy(glm::value_ptr(v.uv), uvData, sizeof(float) * 2);

            this->vertices.push_back(v);
        }

        // Indices
        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
        const uint8_t* indexBufferAddress = indexBuffer.data.data();
        int indexStride = tinygltf::GetComponentSizeInBytes(indexAccessor.componentType) * tinygltf::GetNumComponentsInType(indexAccessor.type);
        for (size_t i = 0; i < indexAccessor.count; i += 3)
        {
            const uint8_t* data = indexBufferAddress + indexBufferView.byteOffset + indexAccessor.byteOffset + (i * indexStride);

            Triangle triangle;
            triangle.meshId = this->meshes.size();

            if (indexStride == 2)
            {
                glm::vec<3, uint16_t> index;
                std::memcpy(glm::value_ptr(index), data, sizeof(uint16_t) * 3);
                triangle.v1 = (int)(index.x + vertexOffset);
                triangle.v2 = (int)(index.y + vertexOffset);
                triangle.v3 = (int)(index.z + vertexOffset);
            }
            else if (indexStride == 4)
            {
                glm::vec<3, uint32_t> index;
                std::memcpy(glm::value_ptr(index), data, sizeof(uint32_t) * 3);
                triangle.v1 = (int)(index.x + vertexOffset);
                triangle.v2 = (int)(index.y + vertexOffset);
                triangle.v3 = (int)(index.z + vertexOffset);
            }

            this->triangles.push_back(triangle);
        }

        // Mesh
        Mesh mesh;
        mesh.materialId = primitive.material;
        mesh.triangleSize = indexAccessor.count / 3;
        mesh.transform = transform;
        this->meshes.push_back(mesh);
        this->meshNames.push_back(gltfMesh.name);
    }
}

void Scene::GLTFcamera(const glm::mat4 transform)
{
    Camera c;
    c.position = glm::vec3(transform[3]);
    c.forward = -glm::normalize(glm::vec3(transform[2]));
    c.up = glm::normalize(glm::vec3(transform[1]));
    this->cameras.push_back(c);
}

void Scene::GLTFnodes(const tinygltf::Model& model, const glm::mat4& world)
{
    for (int index : model.scenes[model.defaultScene].nodes)
    {
        this->GLTFtraverseNode(model, model.nodes[index], world);
    }
}

void Scene::GLTFtraverseNode(const tinygltf::Model& model, const tinygltf::Node& node, const glm::mat4& globalTransform)
{
    glm::mat4 localTranform(1);
    if (!node.matrix.empty())
    {
        localTranform[0][0] = node.matrix[0];
        localTranform[0][1] = node.matrix[1];
        localTranform[0][2] = node.matrix[2];
        localTranform[0][3] = node.matrix[3];

        localTranform[1][0] = node.matrix[4];
        localTranform[1][1] = node.matrix[5];
        localTranform[1][2] = node.matrix[6];
        localTranform[1][3] = node.matrix[7];

        localTranform[2][0] = node.matrix[8];
        localTranform[2][1] = node.matrix[9];
        localTranform[2][2] = node.matrix[10];
        localTranform[2][3] = node.matrix[11];

        localTranform[3][0] = node.matrix[12];
        localTranform[3][1] = node.matrix[13];
        localTranform[3][2] = node.matrix[14];
        localTranform[3][3] = node.matrix[15];
    }
    else
    {
        glm::mat4 translate(1), rotate(1), scale(1);
        if (!node.scale.empty())
        {
            scale = glm::scale(glm::mat4(1), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }

        if (!node.rotation.empty())
        {
            rotate = glm::toMat4(glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]));
        }

        if (!node.translation.empty())
        {
            translate = glm::translate(glm::mat4(1), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }

        localTranform = translate * rotate * scale;
    }

    glm::mat4 transform = globalTransform * localTranform;

    if (node.children.empty() && node.mesh != -1)
    {
        this->GLTFmesh(model, model.meshes[node.mesh], transform);
    }

    if (node.children.empty() && node.camera != -1)
    {
        this->GLTFcamera(transform);
    }

    for (int child : node.children)
    {
        this->GLTFtraverseNode(model, model.nodes[child], transform);
    }
}
