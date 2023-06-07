#include <limits>
#include <TracerX/VectorMath.h>
#include <TracerX/Primitives/Mesh.h>

namespace TracerX
{

Mesh::Mesh(int indicesStart, int indicesEnd, int materialId)
    : indicesStart(indicesStart), indicesEnd(indicesEnd), materialId(materialId)
{
}

void Mesh::offset(sf::Vector3f offset, const std::vector<int>& indices, std::vector<Vertex3>& verticies)
{
    this->position += offset;

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        verticies[index].position += offset;
    }
}

void Mesh::scale(sf::Vector3f scale, const std::vector<int>& indices, std::vector<Vertex3>& verticies)
{
    this->size = mult(this->size, scale);

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        verticies[index].position = mult(verticies[index].position - this->position, scale) + this->position;
        verticies[index].normal = normalized(mult(verticies[index].normal, scale));
    }
}

void Mesh::rotate(sf::Vector3f rotation, const std::vector<int>& indices, std::vector<Vertex3>& verticies)
{
    this->rotation += rotation;

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        verticies[index].position = rotateZ(rotateY(rotateX(verticies[index].position - this->position, rotation.x), rotation.y), rotation.z) + this->position;
        verticies[index].normal = rotateZ(rotateY(rotateX(verticies[index].normal, rotation.x), rotation.y), rotation.z);
    }
}

void Mesh::updateAABB(const std::vector<int>& indices, const std::vector<Vertex3>& verticies)
{
    this->boxMin = sf::Vector3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    this->boxMax = sf::Vector3f(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices[i];
        this->boxMin = min(this->boxMin, verticies[index].position);
        this->boxMax = max(this->boxMax, verticies[index].position);
    }
}

}