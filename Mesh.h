#pragma once

#include <SFML/Graphics.hpp>

struct Mesh
{
    int indicesStart;
    int indicesLength;
    int materialId;
    Vector3f boxMin;
    Vector3f boxMax;

    Mesh(int indicesStart, int indicesLength, int materialId, Vector3f boxMin, Vector3f boxMax)
        : indicesStart(indicesStart), indicesLength(indicesLength), materialId(materialId), boxMin(boxMin), boxMax(boxMax)
    {
    }

    void set(Shader& shader, const std::string name)
    {
        shader.setUniform(name + ".IndicesStart", this->indicesStart);
        shader.setUniform(name + ".IndicesLength", this->indicesLength);
        shader.setUniform(name + ".MaterialId", this->materialId);
        shader.setUniform(name + ".BoxMin", this->boxMin);
        shader.setUniform(name + ".BoxMax", this->boxMax);
    }
};
