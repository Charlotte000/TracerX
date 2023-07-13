#include <limits>
#include <set>
#include <TracerX/VectorMath.h>
#include <TracerX/Primitives/Mesh.h>

namespace TracerX
{

Mesh::Mesh(int indicesStart, int indicesEnd, int materialId)
    : indicesStart(indicesStart), indicesEnd(indicesEnd), materialId(materialId)
{
}

void Mesh::offset(sf::Vector3f offset, const SSBO<int>& indices, SSBO<Vertex3>& verticies)
{
    this->position += offset;

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices.get()[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        Vertex3 vertex = verticies.get()[index];
        vertex.position += offset;
        verticies.set(index, vertex);
    }
}

void Mesh::scale(sf::Vector3f scale, const SSBO<int>& indices, SSBO<Vertex3>& verticies)
{
    this->size = mult(this->size, scale);

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices.get()[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        Vertex3 vertex = verticies.get()[index];
        vertex.position = mult(vertex.position - this->position, scale) + this->position;
        vertex.normal = normalized(mult(vertex.normal, scale));
        verticies.set(index, vertex);
    }
}

void Mesh::rotate(sf::Vector3f rotation, const SSBO<int>& indices, SSBO<Vertex3>& verticies)
{
    this->rotation += rotation;

    std::set<int> meshIndices;
    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices.get()[i];
        if (!meshIndices.insert(index).second)
        {
            continue;
        }

        Vertex3 vertex = verticies.get()[index];
        vertex.position = rotateZ(rotateY(rotateX(vertex.position - this->position, rotation.x), rotation.y), rotation.z) + this->position;
        vertex.normal = rotateZ(rotateY(rotateX(vertex.normal, rotation.x), rotation.y), rotation.z);
        verticies.set(index, vertex);
    }
}

void Mesh::updateAABB(const SSBO<int>& indices, const SSBO<Vertex3>& verticies)
{
    this->boxMin = sf::Vector3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    this->boxMax = sf::Vector3f(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    for (int i = this->indicesStart; i < this->indicesEnd; i++)
    {
        int index = indices.get()[i];
        this->boxMin = min(this->boxMin, verticies.get()[index].position);
        this->boxMax = max(this->boxMax, verticies.get()[index].position);
    }
}

}