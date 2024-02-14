#define TINYOBJLOADER_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "TracerX/Scene.h"
#include "TracerX/Application.h"

#include <stdexcept>
#include <tinydir.h>
#include <tiny_obj_loader.h>
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

void Scene::loadOBJ(const std::string& fileName)
{
    const std::string folder = fileName.substr(0, fileName.find_last_of("/\\") + 1);

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    if (!reader.ParseFromFile(fileName, config))
    {
        throw std::runtime_error(reader.Error());
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();

    // Vertices
    size_t vertexOffset = this->vertices.size();
    this->vertices.reserve(attrib.vertices.size() / 3);
    for (size_t i = 0; i < attrib.vertices.size(); i += 3)
    {
        this->vertices.push_back(glm::vec3(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]));
    }

    // Normals
    size_t normalOffset = this->normals.size();
    this->normals.reserve(attrib.normals.size() / 3);
    for (size_t i = 0; i < attrib.normals.size(); i += 3)
    {
        this->normals.push_back(glm::vec3(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]));
    }

    // Texture coordinates
    size_t texCoordOffset = this->uvCoords.size();
    this->uvCoords.reserve(attrib.texcoords.size() / 2);
    for (size_t i = 0; i < attrib.texcoords.size(); i += 2)
    {
        this->uvCoords.push_back(glm::vec2(attrib.texcoords[i], 1 - attrib.texcoords[i + 1]));
    }

    // Materials
    size_t materialOffset = this->materials.size();
    for (const tinyobj::material_t& material : reader.GetMaterials())
    {
        Material m;

        if (!material.diffuse_texname.empty())
        {
            m.albedoTextureId = this->loadTexture(folder + material.diffuse_texname);
        }

        if (!material.specular_texname.empty())
        {
            m.roughnessTextureId = this->loadTexture(folder + material.specular_texname);
        }
        else if (!material.specular_highlight_texname.empty())
        {
            m.roughnessTextureId = this->loadTexture(folder + material.specular_highlight_texname);
        }

        if (!material.reflection_texname.empty())
        {
            m.metalnessTextureId = this->loadTexture(folder + material.reflection_texname);
        }

        m.albedoColor = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        m.roughness = material.roughness;
        m.metalness = material.metallic;
        m.ior = material.ior;
        this->loadMaterial(m, material.name);
    }

    // Meshes
    for (const tinyobj::shape_t& shape : reader.GetShapes())
    {
        Mesh mesh;
        mesh.materialId = shape.mesh.material_ids[0] == -1 ? this->loadMaterial(Material::matte(glm::vec3(1)), shape.name) : shape.mesh.material_ids[0] + materialOffset;
        mesh.triangleSize = shape.mesh.indices.size() / 3;

        this->meshes.push_back(mesh);
        this->meshNames.push_back(shape.name);
        int meshId = this->meshes.size() - 1;

        // Triangles
        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3)
        {
            const tinyobj::index_t& index1 = shape.mesh.indices[i + 0];
            const tinyobj::index_t& index2 = shape.mesh.indices[i + 1];
            const tinyobj::index_t& index3 = shape.mesh.indices[i + 2];

            Triangle triangle;
            triangle.v1 = Index
            {
                (int)(index1.vertex_index + vertexOffset),
                (int)(index1.normal_index + normalOffset),
                (int)(index1.texcoord_index + texCoordOffset)
            };

            triangle.v2 = Index
            {
                (int)(index2.vertex_index + vertexOffset),
                (int)(index2.normal_index + normalOffset),
                (int)(index2.texcoord_index + texCoordOffset)
            };

            triangle.v3 = Index
            {
                (int)(index3.vertex_index + vertexOffset),
                (int)(index3.normal_index + normalOffset),
                (int)(index3.texcoord_index + texCoordOffset)
            };

            triangle.meshId = meshId;
            this->triangles.push_back(triangle);
        }
    }
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
    if (this->triangles.size() == 0)
    {
        return std::vector<glm::vec3>();
    }

    class TriangleConverter
    {
    public:
        const std::vector<glm::vec3>* vertices;
        const std::vector<Mesh>* meshes;

        TriangleConverter(const std::vector<glm::vec3>* vertices, const std::vector<Mesh>* meshes)
            : vertices(vertices), meshes(meshes)
        {
        }

        FastBVH::BBox<float> operator()(const Triangle& triangle) const noexcept
        {
            const Mesh& mesh = this->meshes->at(triangle.meshId);

            glm::vec3 v1 = mesh.transform * glm::vec4(this->vertices->at(triangle.v1.vertex), 1);
            glm::vec3 v2 = mesh.transform * glm::vec4(this->vertices->at(triangle.v2.vertex), 1);
            glm::vec3 v3 = mesh.transform * glm::vec4(this->vertices->at(triangle.v3.vertex), 1);
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

Scene Scene::load(const std::string& folder)
{
    Scene scene;
    scene.name = folder.substr(folder.find_last_of("/\\") + 1);

    tinydir_dir dir;
    tinydir_open_sorted(&dir, folder.c_str());
    for (size_t i = 2; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);
        if (strcmp(file.extension, "obj") == 0)
        {
            scene.loadOBJ(file.path);
        }
    }

    return scene;
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
    for (const tinygltf::Texture& gltfTexture : model.textures)
    {
        const tinygltf::Image& gltfImage = model.images[gltfTexture.source];
        Image image;
        image.width = gltfImage.width;
        image.height = gltfImage.height;
        image.name = gltfTexture.name;

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
                // Positions
                const uint8_t* positionData = positionBufferAddress + positionBufferView.byteOffset + positionAccessor.byteOffset + (i * sizeof(float) * 3);
                glm::vec3 position;
                std::memcpy(glm::value_ptr(position), positionData, sizeof(float) * 3);
                scene.vertices.push_back(position);

                // Normals
                const uint8_t* normalData = normalBufferAddress + normalBufferView.byteOffset + normalAccessor.byteOffset + (i * sizeof(float) * 3);
                glm::vec3 normal;
                std::memcpy(glm::value_ptr(normal), normalData, sizeof(float) * 3);
                scene.normals.push_back(normal);

                // Texture coordinates
                const uint8_t* uvData = uvBufferAddress + uvBufferView.byteOffset + uvAccessor.byteOffset + (i * sizeof(float) * 2);
                glm::vec2 uv;
                std::memcpy(glm::value_ptr(uv), uvData, sizeof(float) * 2);
                scene.uvCoords.push_back(uv);
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
                    triangle.v1 = {(int)(index.x + vertexOffset), (int)(index.x + vertexOffset), (int)(index.x + vertexOffset)};
                    triangle.v2 = {(int)(index.y + vertexOffset), (int)(index.y + vertexOffset), (int)(index.y + vertexOffset)};
                    triangle.v3 = {(int)(index.z + vertexOffset), (int)(index.z + vertexOffset), (int)(index.z + vertexOffset)};
                }
                else if (indexStride == 4)
                {
                    glm::vec<3, uint32_t> index;
                    std::memcpy(glm::value_ptr(index), data, sizeof(uint32_t) * 3);
                    triangle.v1 = {(int)(index.x + vertexOffset), (int)(index.x + vertexOffset), (int)(index.x + vertexOffset)};
                    triangle.v2 = {(int)(index.y + vertexOffset), (int)(index.y + vertexOffset), (int)(index.y + vertexOffset)};
                    triangle.v3 = {(int)(index.z + vertexOffset), (int)(index.z + vertexOffset), (int)(index.z + vertexOffset)};
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
