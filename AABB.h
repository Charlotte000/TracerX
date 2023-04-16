#pragma once

#include "Material.h"

struct AABB
{
    Vector3f origin;
    Vector3f size;
    int materialId;

    AABB(Vector3f origin, Vector3f size)
        : origin(origin), size(size), materialId(0)
    {
    }

    AABB(Vector3f origin, Vector3f size, int materialId)
        : origin(origin), size(size), materialId(materialId)
    {
    }

    void set(Shader& shader, const std::string name)
    {
        shader.setUniform(name + ".Origin", this->origin);
        shader.setUniform(name + ".Size", this->size);
        shader.setUniform(name + ".MaterialId", this->materialId);
    }
};