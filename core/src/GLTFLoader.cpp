/**
 * @file GLTFLoader.cpp
 */
#define TINYGLTF_IMPLEMENTATION

#include "TracerX/GLTFLoader.h"

#include <map>
#include <stdexcept>
#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace TracerX;
using namespace TracerX::core;

#pragma region GLTF Helper Functions
std::map<int, std::vector<glm::ivec2>> GLTFmeshes(Scene& scene, const tinygltf::Model& model)
{
    std::map<int, std::vector<glm::ivec2>> meshMap;
    scene.meshes.reserve(scene.meshes.size() + model.meshes.size());
    scene.meshNames.reserve(scene.meshes.size() + model.meshes.size());

    for (size_t gltfMeshId = 0; gltfMeshId < model.meshes.size(); gltfMeshId++)
    {
        const tinygltf::Mesh& gltfMesh = model.meshes[gltfMeshId];
        for (tinygltf::Primitive primitive : gltfMesh.primitives)
        {
            if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
            {
                continue;
            }

            // Get vertex data
            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes["POSITION"]];
            const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
            const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
            const uint8_t* positionBufferAddress = positionBuffer.data.data();
            const uint8_t* positionData = positionBufferAddress + positionBufferView.byteOffset + positionAccessor.byteOffset;
            size_t positionStride = positionAccessor.ByteStride(positionBufferView);

            const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
            const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
            const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
            const uint8_t* normalBufferAddress = normalBuffer.data.data();
            const uint8_t* normalData = normalBufferAddress + normalBufferView.byteOffset + normalAccessor.byteOffset;
            size_t normalStride = normalAccessor.ByteStride(normalBufferView);

            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
            const tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];
            const uint8_t* uvBufferAddress = uvBuffer.data.data();
            const uint8_t* uvData = uvBufferAddress + uvBufferView.byteOffset + uvAccessor.byteOffset;
            size_t uvStride = uvAccessor.ByteStride(uvBufferView);

            size_t vertexOffset = scene.vertices.size();
            scene.vertices.reserve(vertexOffset + positionAccessor.count);
            for (size_t i = 0; i < positionAccessor.count; i++)
            {
                Vertex v;

                // Position
                std::memcpy(glm::value_ptr(v.positionU), positionData + i * positionStride, sizeof(float) * 3);

                // Normal
                std::memcpy(glm::value_ptr(v.normalV), normalData + i * normalStride, sizeof(float) * 3);

                // Texture coordinate
                glm::vec2 uv;
                std::memcpy(glm::value_ptr(uv), uvData + i * uvStride, sizeof(float) * 2);
                v.positionU.w = uv.x;
                v.normalV.w = uv.y;

                scene.vertices.push_back(v);
            }

            // Get indices data
            size_t triangleOffset = scene.triangles.size();
            if (primitive.indices >= 0)
            {
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
                const uint8_t* indexBufferAddress = indexBuffer.data.data();
                const uint8_t* indexData = indexBufferAddress + indexBufferView.byteOffset + indexAccessor.byteOffset;
                size_t indexStride = indexAccessor.ByteStride(indexBufferView);

                scene.triangles.reserve(triangleOffset + indexAccessor.count / 3);
                for (size_t i = 0; i < indexAccessor.count; i += 3)
                {
                    const uint8_t* data = indexData + (i * indexStride);
                    switch (indexStride)
                    {
                        case 1:
                            glm::vec<3, uint8_t> index8;
                            std::memcpy(glm::value_ptr(index8), data, sizeof(uint8_t) * 3);
                            scene.triangles.push_back(
                                Triangle { .v1 = (int)(index8.x + vertexOffset), .v2 = (int)(index8.y + vertexOffset), .v3 = (int)(index8.z + vertexOffset) });
                            break;
                        case 2:
                            glm::vec<3, uint16_t> index16;
                            std::memcpy(glm::value_ptr(index16), data, sizeof(uint16_t) * 3);
                            scene.triangles.push_back(
                                Triangle { .v1 = (int)(index16.x + vertexOffset), .v2 = (int)(index16.y + vertexOffset), .v3 = (int)(index16.z + vertexOffset) });
                            break;
                        case 4:
                            glm::vec<3, uint32_t> index32;
                            std::memcpy(glm::value_ptr(index32), data, sizeof(uint32_t) * 3);
                            scene.triangles.push_back(
                                Triangle { .v1 = (int)(index32.x + vertexOffset), .v2 = (int)(index32.y + vertexOffset), .v3 = (int)(index32.z + vertexOffset) });
                            break;
                        case 8:
                            glm::vec<3, uint64_t> index64;
                            std::memcpy(glm::value_ptr(index64), data, sizeof(uint64_t) * 3);
                            scene.triangles.push_back(
                                Triangle { .v1 = (int)(index64.x + vertexOffset), .v2 = (int)(index64.y + vertexOffset), .v3 = (int)(index64.z + vertexOffset) });
                            break;
                        default:
                            throw std::runtime_error("Unsupported index stride");
                    }
                }
            }
            else
            {
                scene.triangles.reserve(triangleOffset + (scene.vertices.size() - vertexOffset) / 3);
                for (size_t i = vertexOffset; i < scene.vertices.size(); i += 3)
                {
                    scene.triangles.push_back(Triangle { .v1 = (int)(i + 0), .v2 = (int)(i + 1), .v3 = (int)(i + 2) });
                }
            }

            // Mesh
            Mesh mesh;
            mesh.triangleOffset = (int)triangleOffset;
            mesh.triangleSize = (int)(scene.triangles.size() - triangleOffset);
            int meshId = scene.addMesh(mesh, gltfMesh.name);

            meshMap[(int)gltfMeshId].emplace_back(meshId, primitive.material);
        }
    }

    return meshMap;
}

void GLTFtextures(Scene& scene, const std::vector<tinygltf::Texture>& textures, const std::vector<tinygltf::Image>& images)
{
    scene.textures.reserve(scene.textures.size() + textures.size());
    scene.textureNames.reserve(scene.textures.size() + textures.size());
    for (const tinygltf::Texture& gltfTexture : textures)
    {
        const tinygltf::Image& gltfImage = images[gltfTexture.source];

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

        scene.addTexture(Image::loadFromMemory(size, pixels), gltfTexture.name.empty() ? gltfImage.name : gltfTexture.name);
    }
}

void GLTFmaterials(Scene& scene, const std::vector<tinygltf::Material>& materials)
{
    scene.materials.reserve(scene.materials.size() + materials.size());
    scene.materialNames.reserve(scene.materials.size() + materials.size());
    for (const tinygltf::Material& gltfMaterial : materials)
    {
        const tinygltf::PbrMetallicRoughness& pbr = gltfMaterial.pbrMetallicRoughness;

        Material material;

        material.albedoColor = glm::vec4(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
        material.roughness = std::sqrt((float)pbr.roughnessFactor);
        material.emissionColor = glm::vec3(gltfMaterial.emissiveFactor[0], gltfMaterial.emissiveFactor[1], gltfMaterial.emissiveFactor[2]);
        material.emissionStrength = 1;
        material.metalness = (float)pbr.metallicFactor;

        material.albedoTextureId = pbr.baseColorTexture.index;
        material.metalnessTextureId = pbr.metallicRoughnessTexture.index;
        material.emissionTextureId = gltfMaterial.emissiveTexture.index;
        material.roughnessTextureId = pbr.metallicRoughnessTexture.index;
        material.normalTextureId = gltfMaterial.normalTexture.index;

        material.alphaCutoff = (float)gltfMaterial.alphaCutoff;
        if (gltfMaterial.alphaMode == "BLEND")
        {
            material.alphaMode = Material::AlphaMode::Blend;
        }
        else if (gltfMaterial.alphaMode == "MASK")
        {
            material.alphaMode = Material::AlphaMode::Mask;
        }
        else
        {
            material.alphaMode = Material::AlphaMode::Opaque;
        }

        if (gltfMaterial.extensions.find("KHR_materials_ior") != gltfMaterial.extensions.end())
        {
            const tinygltf::Value& ext = gltfMaterial.extensions.find("KHR_materials_ior")->second;
            if (ext.Has("ior"))
            {
                material.ior = (float)ext.Get("ior").Get<double>();
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
                material.emissionStrength = (float)ext.Get("emissiveStrength").Get<double>();
            }
        }

        scene.addMaterial(material, gltfMaterial.name);
    }
}

void GLTFtraverseNode(Scene& scene, const tinygltf::Model& model, const tinygltf::Node& node, const std::map<int, std::vector<glm::ivec2>>& meshMap, const glm::mat4& globalTransform)
{
    // Get transform
    glm::mat4 localTransform(1);
    if (!node.matrix.empty())
    {
        localTransform[0][0] = (float)node.matrix[0];
        localTransform[0][1] = (float)node.matrix[1];
        localTransform[0][2] = (float)node.matrix[2];
        localTransform[0][3] = (float)node.matrix[3];

        localTransform[1][0] = (float)node.matrix[4];
        localTransform[1][1] = (float)node.matrix[5];
        localTransform[1][2] = (float)node.matrix[6];
        localTransform[1][3] = (float)node.matrix[7];

        localTransform[2][0] = (float)node.matrix[8];
        localTransform[2][1] = (float)node.matrix[9];
        localTransform[2][2] = (float)node.matrix[10];
        localTransform[2][3] = (float)node.matrix[11];

        localTransform[3][0] = (float)node.matrix[12];
        localTransform[3][1] = (float)node.matrix[13];
        localTransform[3][2] = (float)node.matrix[14];
        localTransform[3][3] = (float)node.matrix[15];
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
            rotate = (glm::mat4)glm::quat((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]);
        }

        if (!node.translation.empty())
        {
            translate = glm::translate(glm::mat4(1), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }

        localTransform = translate * rotate * scale;
    }

    const glm::mat4 transform = globalTransform * localTransform;

    // Get mesh instances
    if (node.mesh >= 0)
    {
        for (glm::ivec2 pair : meshMap.at(node.mesh))
        {
            MeshInstance meshInstance;
            meshInstance.meshId = pair.x;
            meshInstance.materialId = pair.y;
            meshInstance.transform = transform;
            scene.meshInstances.push_back(meshInstance);
        }
    }

    // Get cameras
    if (node.camera >= 0)
    {
        const tinygltf::Camera& camera = model.cameras[node.camera];
        if (camera.type == "perspective")
        {
            Camera c;
            c.position = glm::vec3(transform[3]);
            c.forward = -glm::normalize(glm::vec3(transform[2]));
            c.up = glm::normalize(glm::vec3(transform[1]));
            c.zNear = (float)camera.perspective.znear;
            c.zFar = (float)camera.perspective.zfar;
            c.fov = 2.f * atanf(tanf((float)camera.perspective.yfov * .5f) * (camera.perspective.aspectRatio == 0 ? 1 : (float)camera.perspective.aspectRatio));
            scene.cameras.push_back(c);
        }
        else if (camera.type == "orthographic")
        {
            Camera c;
            c.position = glm::vec3(transform[3]);
            c.forward = -glm::normalize(glm::vec3(transform[2]));
            c.up = glm::normalize(glm::vec3(transform[1]));
            c.zNear = (float)camera.orthographic.znear;
            c.zFar = (float)camera.orthographic.zfar;
            scene.cameras.push_back(c);
        }
    }

    // Traverse children
    for (int child : node.children)
    {
        GLTFtraverseNode(scene, model, model.nodes[child], meshMap, transform);
    }
}

void GLTFnodes(Scene& scene, const tinygltf::Model& model, const std::map<int, std::vector<glm::ivec2>>& meshMap, const glm::mat4& world)
{
    if (model.scenes.empty())
    {
        throw std::runtime_error("No scenes are provided");
    }

    for (int index : model.scenes[std::clamp(model.defaultScene, 0, (int)model.scenes.size() - 1)].nodes)
    {
        GLTFtraverseNode(scene, model, model.nodes[index], meshMap, world);
    }
}
#pragma endregion

Scene TracerX::loadGLTF(const std::filesystem::path& path)
{
    Scene scene;

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string err;
    if (path.extension() == ".glb")
    {
        if (!loader.LoadBinaryFromFile(&model, &err, nullptr, path.string()))
        {
            throw std::runtime_error(err);
        }
    }
    else
    {
        if (!loader.LoadASCIIFromFile(&model, &err, nullptr, path.string()))
        {
            throw std::runtime_error(err);
        }
    }

    const std::map<int, std::vector<glm::ivec2>> meshMap = GLTFmeshes(scene, model);
    GLTFtextures(scene, model.textures, model.images);
    GLTFmaterials(scene, model.materials);
    GLTFnodes(scene, model, meshMap, glm::mat4(1));
    return scene;
}
