/**
 * @file Scene.cpp
 */
#include "TracerX/Scene.h"

#include <FastBVH.h>
#include <glm/gtx/extended_min_max.hpp>

using namespace TracerX;
using namespace TracerX::core;

int Scene::addTexture(const Image& texture, const std::string& name)
{
    this->textures.push_back(texture);
    int index = this->textures.size() - 1;
    this->textureNames.push_back(name.empty() ? "Untitled texture " + std::to_string(index) : name);
    return index;
}

int Scene::addMaterial(const Material& material, const std::string& name)
{
    this->materials.push_back(material);
    int index = this->materials.size() - 1;
    this->materialNames.push_back(name.empty() ? "Untitled material " + std::to_string(index) : name);
    return index;
}

int Scene::addMesh(const Mesh& mesh, const std::string& name)
{
    this->meshes.push_back(mesh);
    int index = this->meshes.size() - 1;
    this->meshNames.push_back(name.empty() ? "Untitled mesh " + std::to_string(index) : name);
    this->buildBVH(this->meshes.back());
    return index;
}

void Scene::buildBVH(Mesh& mesh)
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
    mesh.nodeOffset = this->bvh.size();
    for (const FastBVH::Node<float>& node : bvh.getNodes())
    {
        BvhNode myNode;
        myNode.bboxMin = glm::vec4(node.bbox.min, 0);
        myNode.bboxMax = glm::vec4(node.bbox.max, 0);
        myNode.start = node.start;
        myNode.primitiveCount = node.primitive_count;
        myNode.rightOffset = node.right_offset;
        this->bvh.push_back(myNode);
    }
}
