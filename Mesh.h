#pragma once

#include <SFML/Graphics.hpp>
#include <set>
#include <vector>

#include "Vertex3.h"
#include "VectorMath.h"

using namespace std;

struct Mesh
{
    int indicesStart;
    int indicesLength;
    int materialId;
    Vector3f boxMin = Vector3f(0, 0, 0);
    Vector3f boxMax = Vector3f(0, 0, 0);
    Vector3f position = Vector3f(0, 0, 0);
    Vector3f size = Vector3f(1, 1, 1);
    Vector3f rotation = Vector3f(0, 0, 0);

    Mesh(int indicesStart, int indicesLength, int materialId)
        : indicesStart(indicesStart), indicesLength(indicesLength), materialId(materialId)
    {
    }

    void set(Shader& shader, const string name)
    {
        shader.setUniform(name + ".IndicesStart", this->indicesStart);
        shader.setUniform(name + ".IndicesLength", this->indicesLength);
        shader.setUniform(name + ".MaterialId", this->materialId);
        shader.setUniform(name + ".BoxMin", this->boxMin);
        shader.setUniform(name + ".BoxMax", this->boxMax);
    }

    void offset(Vector3f offset, const vector<int>& indices, vector<Vertex3>& verticies)
    {
        this->position += offset;

        Vector3f boxMin(INFINITY, INFINITY, INFINITY);
        Vector3f boxMax(-INFINITY, -INFINITY, -INFINITY);

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

    void scale(Vector3f scale, const vector<int>& indices, vector<Vertex3>& verticies)
    {
        this->size = mult(this->size, scale);

        Vector3f boxMin(INFINITY, INFINITY, INFINITY);
        Vector3f boxMax(-INFINITY, -INFINITY, -INFINITY);

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

    void rotate(Vector3f rotation, const vector<int>& indices, vector<Vertex3>& verticies)
    {
        this->rotation += rotation;

        Vector3f boxMin(INFINITY, INFINITY, INFINITY);
        Vector3f boxMax(-INFINITY, -INFINITY, -INFINITY);

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
};
