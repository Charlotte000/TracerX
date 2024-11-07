/**
 * @file Scene.cpp
 */
#include "TracerX/Scene.h"

#include <FastBVH.h>
#include <glm/gtx/extended_min_max.hpp>

using namespace TracerX;
using namespace TracerX::core;

BvhNode toNode(const FastBVH::Node<float>& node)
{
    BvhNode myNode;
    myNode.bboxMin = glm::vec4(node.bbox.min, 0);
    myNode.bboxMax = glm::vec4(node.bbox.max, 0);
    myNode.start = node.start;
    myNode.primitiveCount = node.primitive_count;
    myNode.rightOffset = node.right_offset;
    return myNode;
}

int Scene::addTexture(const Image& texture, const std::string& name)
{
    int index = (int)this->textures.size();
    this->textures.push_back(texture);
    this->textureNames.push_back(name.empty() ? "Untitled texture " + std::to_string(index) : name);
    return index;
}

int Scene::addMaterial(const Material& material, const std::string& name)
{
    int index = (int)this->materials.size();
    this->materials.push_back(material);
    this->materialNames.push_back(name.empty() ? "Untitled material " + std::to_string(index) : name);
    return index;
}

int Scene::addMesh(const Mesh& mesh, const std::string& name)
{
    int index = (int)this->meshes.size();
    this->meshes.push_back(mesh);
    this->meshNames.push_back(name.empty() ? "Untitled mesh " + std::to_string(index) : name);
    this->buildBLAS(this->meshes.back());
    return index;
}

void Scene::buildBLAS(Mesh& mesh)
{
    class TriangleConverter
    {
    public:
        const std::vector<Vertex>* vertices;

        TriangleConverter(const std::vector<Vertex>* vertices)
            : vertices(vertices)
        {
        }

        FastBVH::BBox<float> operator()(const Triangle& triangle) const noexcept
        {
            glm::vec3 v1 = this->vertices->at(triangle.v1).positionU;
            glm::vec3 v2 = this->vertices->at(triangle.v2).positionU;
            glm::vec3 v3 = this->vertices->at(triangle.v3).positionU;
            return FastBVH::BBox<float>(glm::min(v1, v2, v3), glm::max(v1, v2, v3));
        }
    } triangleConverter(&this->vertices);
    FastBVH::DefaultBuilder<float> bvhBuilder;

    // Build BVH
    FastBVH::BVH<float, Triangle> bvh = bvhBuilder(
        FastBVH::Iterable<Triangle>(this->triangles.data() + mesh.triangleOffset, mesh.triangleSize),
        triangleConverter);

    // Convert to our format
    mesh.nodeOffset = (int)this->blas.size();
    this->blas.reserve(this->blas.size() + bvh.getNodes().size());
    for (const FastBVH::Node<float>& node : bvh.getNodes())
    {
        this->blas.push_back(toNode(node));
    }
}

void Scene::buildTLAS(std::vector<BvhNode>& tlas, std::vector<size_t>& permutation)
{
    class MeshInstanceConverter
    {
    public:
        const std::vector<MeshInstance>* meshInstances;
        const std::vector<Mesh>* meshes;
        const std::vector<BvhNode>* blas;

        MeshInstanceConverter(const std::vector<MeshInstance>* meshInstances, const std::vector<Mesh>* meshes, const std::vector<BvhNode>* blas)
            : meshInstances(meshInstances), meshes(meshes), blas(blas)
        {
        }

        FastBVH::BBox<float> operator()(const size_t& meshInstanceId) const noexcept
        {
            const MeshInstance& meshInstance = this->meshInstances->at(meshInstanceId);
            const Mesh& mesh = this->meshes->at(meshInstance.meshId);
            const BvhNode& node = this->blas->at(mesh.nodeOffset);

            // Get 8 corners of the bounding box
            glm::vec3 pos = (node.bboxMax + node.bboxMin) * 0.5f;
            glm::vec3 hSize = (node.bboxMax - node.bboxMin) * 0.5f;

            glm::vec3 v1 = pos + glm::vec3(-1, -1, -1) * hSize;
            glm::vec3 v2 = pos + glm::vec3(-1, -1,  1) * hSize;
            glm::vec3 v3 = pos + glm::vec3(-1,  1, -1) * hSize;
            glm::vec3 v4 = pos + glm::vec3(-1,  1,  1) * hSize;
            glm::vec3 v5 = pos + glm::vec3( 1, -1, -1) * hSize;
            glm::vec3 v6 = pos + glm::vec3( 1, -1,  1) * hSize;
            glm::vec3 v7 = pos + glm::vec3( 1,  1, -1) * hSize;
            glm::vec3 v8 = pos + glm::vec3( 1,  1,  1) * hSize;

            v1 = meshInstance.transform * glm::vec4(v1, 1);
            v2 = meshInstance.transform * glm::vec4(v2, 1);
            v3 = meshInstance.transform * glm::vec4(v3, 1);
            v4 = meshInstance.transform * glm::vec4(v4, 1);
            v5 = meshInstance.transform * glm::vec4(v5, 1);
            v6 = meshInstance.transform * glm::vec4(v6, 1);
            v7 = meshInstance.transform * glm::vec4(v7, 1);
            v8 = meshInstance.transform * glm::vec4(v8, 1);

            glm::vec3 minV = glm::min(glm::min(glm::min(v1, v2, v3, v4), v5, v6, v7), v8);
            glm::vec3 maxV = glm::max(glm::max(glm::max(v1, v2, v3, v4), v5, v6, v7), v8);
            return FastBVH::BBox<float>(minV, maxV);
        }
    } meshInstanceBuilder(&this->meshInstances, &this->meshes, &this->blas);
    FastBVH::DefaultBuilder<float> bvhBuilder;

    // Create mesh instance permutation
    permutation.reserve(permutation.size() + this->meshInstances.size());
    for (size_t i = 0; i < this->meshInstances.size(); i++)
    {
        permutation.push_back(i);
    }

    // Build BVH
    FastBVH::BVH<float, size_t> bvh = bvhBuilder(permutation, meshInstanceBuilder);

    // Convert to our format
    tlas.reserve(tlas.size() + bvh.getNodes().size());
    for (const FastBVH::Node<float>& node : bvh.getNodes())
    {
        tlas.push_back(toNode(node));
    }
}
