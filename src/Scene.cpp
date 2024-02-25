#define TINYGLTF_IMPLEMENTATION

#include "TracerX/Scene.h"
#include "TracerX/Application.h"

#include <stdexcept>
#include <tinydir.h>
#include <glm/gtc/type_ptr.hpp>


Material Material::lightSource(glm::vec3 emissionColor, float emissionStrength)
{
    Material m;
    m.albedoColor = glm::vec3(0);
    m.emissionColor = emissionColor * emissionStrength;
    return m;
}

Material Material::transparent(glm::vec3 albedoColor, float ior, glm::vec3 fresnelColor, float fresnelStrength)
{
    Material m;
    m.albedoColor = albedoColor;
    m.ior = ior;
    m.fresnelColor = fresnelColor;
    m.fresnelStrength = fresnelStrength;
    return m;
}

Material Material::constantDensity(glm::vec3 albedoColor, float density)
{
    Material m;
    m.roughness = 1.f;
    m.albedoColor = albedoColor;
    m.ior = 1.f;
    m.density = density;
    return m;
}

Material Material::matte(glm::vec3 albedoColor, float metalness)
{
    Material m;
    m.albedoColor = albedoColor;
    m.roughness = 1.f;
    m.metalness = metalness;
    return m;
}

Material Material::mirror()
{
    Material m;
    m.roughness = 0.f;
    m.metalness = 1.f;
    return m;
}

void Scene::loadEnvironmentMap(const std::string& fileName)
{
    this->environment = Image::loadFromFile(fileName);
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

    // Textures
    for (size_t textureId = 0; textureId < model.textures.size(); textureId++)
    {
        const tinygltf::Texture& gltfTexture = model.textures[textureId];
        const tinygltf::Image& gltfImage = model.images[gltfTexture.source];
        Image image;
        image.width = gltfImage.width;
        image.height = gltfImage.height;
        image.name = gltfTexture.name != "" ? gltfTexture.name : std::to_string(textureId);

        image.pixels.reserve(gltfImage.width * gltfImage.height * 3);
        for (size_t i = 0; i < gltfImage.image.size(); i += gltfImage.component)
        {
            image.pixels.push_back(gltfImage.image[i + 0] / 255.f);
            image.pixels.push_back(gltfImage.image[i + 1] / 255.f);
            image.pixels.push_back(gltfImage.image[i + 2] / 255.f);
        }

        scene.textures.push_back(image);
    }

    // Materials
    for (const tinygltf::Material& gltfMaterial : model.materials)
    {
        const tinygltf::PbrMetallicRoughness& pbr = gltfMaterial.pbrMetallicRoughness;

        Material material;

        material.albedoColor = glm::vec3(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]);
        material.roughness = pbr.roughnessFactor;
        material.emissionColor = glm::vec3(gltfMaterial.emissiveFactor[0], gltfMaterial.emissiveFactor[1], gltfMaterial.emissiveFactor[2]);
        material.emissionStrength = glm::length(material.emissionColor);
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

        scene.materials.push_back(material);
        scene.materialNames.push_back(gltfMaterial.name);
    }

    if (scene.materials.empty())
    {
        scene.materials.push_back(Material::matte(glm::vec3(1)));
        scene.materialNames.push_back("Default");
    }

    for (const tinygltf::Mesh& gltfMesh : model.meshes)
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

            size_t vertexOffset = scene.vertices.size();
            for (size_t i = 0; i < positionAccessor.count; i++)
            {
                Vertex v;

                // Position
                const uint8_t* positionData = positionBufferAddress + positionBufferView.byteOffset + positionAccessor.byteOffset + (i * sizeof(float) * 3);
                std::memcpy(glm::value_ptr(v.position), positionData, sizeof(float) * 3);

                // Normal
                const uint8_t* normalData = normalBufferAddress + normalBufferView.byteOffset + normalAccessor.byteOffset + (i * sizeof(float) * 3);
                glm::vec3 normal;
                std::memcpy(glm::value_ptr(v.normal), normalData, sizeof(float) * 3);

                // Texture coordinate
                const uint8_t* uvData = uvBufferAddress + uvBufferView.byteOffset + uvAccessor.byteOffset + (i * sizeof(float) * 2);
                std::memcpy(glm::value_ptr(v.uv), uvData, sizeof(float) * 2);

                scene.vertices.push_back(v);
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
                triangle.meshId = scene.meshes.size();

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

                scene.triangles.push_back(triangle);
            }

            // Mesh
            Mesh mesh;
            mesh.materialId = primitive.material;
            mesh.triangleSize = indexAccessor.count / 3;
            scene.meshes.push_back(mesh);
            scene.meshNames.push_back(gltfMesh.name);
        }
    }

    return scene;
}
