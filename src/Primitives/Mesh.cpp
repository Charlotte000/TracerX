#include <TracerX/VectorMath.h>
#include <TracerX/Primitives/Mesh.h>
#include <limits>

namespace TracerX
{

Mesh::Mesh(int indicesStart, int indicesLength, int materialId)
    : indicesStart(indicesStart), indicesLength(indicesLength), materialId(materialId)
{
}

void Mesh::offset(sf::Vector3f offset, const std::vector<int>& indices, std::vector<Vertex3>& verticies)
{
    this->position += offset;

    sf::Vector3f boxMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    sf::Vector3f boxMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesStart + this->indicesLength; i++)
    {
        int index = indices[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        verticies[index].position += offset;
        boxMin = min(boxMin, verticies[index].position);
        boxMax = max(boxMax, verticies[index].position);
    }

    this->boxMin = boxMin;
    this->boxMax = boxMax;
}

void Mesh::scale(sf::Vector3f scale, const std::vector<int>& indices, std::vector<Vertex3>& verticies)
{
    this->size = mult(this->size, scale);

    sf::Vector3f boxMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    sf::Vector3f boxMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesStart + this->indicesLength; i++)
    {
        int index = indices[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        verticies[index].position = mult(verticies[index].position - this->position, scale) + this->position;
        verticies[index].normal = normalized(mult(verticies[index].normal, scale));
        boxMin = min(boxMin, verticies[index].position);
        boxMax = max(boxMax, verticies[index].position);
    }

    this->boxMin = boxMin;
    this->boxMax = boxMax;
}

void Mesh::rotate(sf::Vector3f rotation, const std::vector<int>& indices, std::vector<Vertex3>& verticies)
{
    this->rotation += rotation;

    sf::Vector3f boxMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    sf::Vector3f boxMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesStart + this->indicesLength; i++)
    {
        int index = indices[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        verticies[index].position = rotateZ(rotateY(rotateX(verticies[index].position - this->position, rotation.x), rotation.y), rotation.z) + this->position;
        verticies[index].normal = rotateZ(rotateY(rotateX(verticies[index].normal, rotation.x), rotation.y), rotation.z);
        boxMin = min(boxMin, verticies[index].position);
        boxMax = max(boxMax, verticies[index].position);
    }

    this->boxMin = boxMin;
    this->boxMax = boxMax;
}

}