/**
 * @file Scene.cpp
 */
#define TINYGLTF_IMPLEMENTATION

#include "TracerX/Scene.h"

#include <stdexcept>
#include <tinydir.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace TracerX;
using namespace TracerX::core;

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

Scene Scene::loadGLTF(const std::string& fileName)
{
    Scene scene;
    scene.name = fileName.substr(fileName.find_last_of("/\\") + 1);

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;

    if (fileName.substr(fileName.find_last_of('.') + 1) == "glb")
    {
        if (!loader.LoadBinaryFromFile(&model, &err, nullptr, fileName))
        {
            throw std::runtime_error(err);
        }
    }
    else
    {
        if (!loader.LoadASCIIFromFile(&model, &err, nullptr, fileName))
        {
            throw std::runtime_error(err);
        }
    }

    scene.GLTFtextures(model.textures, model.images);
    scene.GLTFmaterials(model.materials);
    scene.GLTFnodes(model, glm::mat4(1));

    for (Mesh& mesh : scene.meshes)
    {
        scene.buildBVH(mesh);
    }

    return scene;
}

void Scene::GLTFtextures(const std::vector<tinygltf::Texture>& textures, const std::vector<tinygltf::Image>& images)
{
    for (size_t textureId = 0; textureId < textures.size(); textureId++)
    {
        const tinygltf::Texture& gltfTexture = textures[textureId];
        const tinygltf::Image& gltfImage = images[gltfTexture.source];

        std::string name = gltfTexture.name != "" ? gltfTexture.name : std::to_string(textureId);
        glm::uvec2 size(gltfImage.width, gltfImage.height);

        std::vector<float> pixels;
        pixels.reserve(gltfImage.width * gltfImage.height * 4);
        for (size_t i = 0; i < gltfImage.image.size(); i += gltfImage.component)
        {
            pixels.push_back(gltfImage.component > 0 ? gltfImage.image[i + 0] / 255.f : 0);
            pixels.push_back(gltfImage.component > 1 ? gltfImage.image[i + 1] / 255.f : 0);
            pixels.push_back(gltfImage.component > 2 ? gltfImage.image[i + 2] / 255.f : 0);
            pixels.push_back(gltfImage.component > 3 ? gltfImage.image[i + 3] / 255.f : 1);
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
            std::memcpy(glm::value_ptr(v.positionU), positionData, sizeof(float) * 3);

            // Normal
            const uint8_t* normalData = normalBufferAddress + normalBufferView.byteOffset + normalAccessor.byteOffset + (i * sizeof(float) * 3);
            std::memcpy(glm::value_ptr(v.normalV), normalData, sizeof(float) * 3);

            // Texture coordinate
            const uint8_t* uvData = uvBufferAddress + uvBufferView.byteOffset + uvAccessor.byteOffset + (i * sizeof(float) * 2);
            glm::vec2 uv;
            std::memcpy(glm::value_ptr(uv), uvData, sizeof(float) * 2);
            v.positionU.w = uv.x;
            v.normalV.w = uv.y;

            this->vertices.push_back(v);
        }

        // Indices
        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
        const uint8_t* indexBufferAddress = indexBuffer.data.data();
        int indexStride = tinygltf::GetComponentSizeInBytes(indexAccessor.componentType) * tinygltf::GetNumComponentsInType(indexAccessor.type);
        size_t triangleOffset = this->triangles.size();
        for (size_t i = 0; i < indexAccessor.count; i += 3)
        {
            const uint8_t* data = indexBufferAddress + indexBufferView.byteOffset + indexAccessor.byteOffset + (i * indexStride);

            Triangle triangle;
            if (indexStride == 1)
            {
                glm::vec<3, uint8_t> index;
                std::memcpy(glm::value_ptr(index), data, sizeof(uint8_t) * 3);
                triangle.v1 = (int)(index.x + vertexOffset);
                triangle.v2 = (int)(index.y + vertexOffset);
                triangle.v3 = (int)(index.z + vertexOffset);
            }
            else if (indexStride == 2)
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
            else if (indexStride == 8)
            {
                glm::vec<3, uint64_t> index;
                std::memcpy(glm::value_ptr(index), data, sizeof(uint64_t) * 3);
                triangle.v1 = (int)(index.x + vertexOffset);
                triangle.v2 = (int)(index.y + vertexOffset);
                triangle.v3 = (int)(index.z + vertexOffset);
            }
            else
            {
                throw std::runtime_error("Unsupported index stride");
            }

            this->triangles.push_back(triangle);
        }

        // Mesh
        Mesh mesh;
        mesh.materialId = primitive.material;
        mesh.triangleOffset = triangleOffset;
        mesh.triangleSize = indexAccessor.count / 3;
        mesh.transform = transform;
        mesh.transformInv = glm::inverse(transform);
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
    glm::mat4 localTransform(1);
    if (!node.matrix.empty())
    {
        localTransform[0][0] = node.matrix[0];
        localTransform[0][1] = node.matrix[1];
        localTransform[0][2] = node.matrix[2];
        localTransform[0][3] = node.matrix[3];

        localTransform[1][0] = node.matrix[4];
        localTransform[1][1] = node.matrix[5];
        localTransform[1][2] = node.matrix[6];
        localTransform[1][3] = node.matrix[7];

        localTransform[2][0] = node.matrix[8];
        localTransform[2][1] = node.matrix[9];
        localTransform[2][2] = node.matrix[10];
        localTransform[2][3] = node.matrix[11];

        localTransform[3][0] = node.matrix[12];
        localTransform[3][1] = node.matrix[13];
        localTransform[3][2] = node.matrix[14];
        localTransform[3][3] = node.matrix[15];
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

        localTransform = translate * rotate * scale;
    }

    glm::mat4 transform = globalTransform * localTransform;

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

void Scene::buildBVH(Mesh& mesh)
{
    class TriangleConverter
    {
    public:
        const std::vector<Vertex>* vertices;
        const Mesh* mesh;

        TriangleConverter(const std::vector<Vertex>* vertices, const Mesh* mesh)
            : vertices(vertices), mesh(mesh)
        {
        }

        FastBVH::BBox<float> operator()(const Triangle& triangle) const noexcept
        {
            glm::vec3 v1 = this->vertices->at(triangle.v1).positionU;
            glm::vec3 v2 = this->vertices->at(triangle.v2).positionU;
            glm::vec3 v3 = this->vertices->at(triangle.v3).positionU;
            return FastBVH::BBox<float>(glm::min(glm::min(v1, v2), v3), glm::max(glm::max(v1, v2), v3));
        }
    };

    FastBVH::BVH<float, Triangle> bvh = FastBVH::DefaultBuilder<float>()(
        FastBVH::Iterable<Triangle>(this->triangles.data() + (size_t)mesh.triangleOffset, (size_t)mesh.triangleSize),
        TriangleConverter(&this->vertices, &mesh));

    mesh.nodeOffset = (float)(this->bvh.size() / 3);
    for (const FastBVH::Node<float>& node : bvh.getNodes())
    {
        this->bvh.push_back(node.bbox.min);
        this->bvh.push_back(node.bbox.max);
        this->bvh.push_back(glm::vec3(node.start, node.primitive_count, node.right_offset));
    }
}
