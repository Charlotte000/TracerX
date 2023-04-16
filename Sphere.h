#pragma once

#include "Material.h"

struct Sphere
{
    Vector3f origin;
    float radius;
    int materialId;

    Sphere(Vector3f origin, float radius)
        : origin(origin), radius(radius), materialId(0)
    {
    }

    Sphere(Vector3f origin, float radius, int materialId)
        : origin(origin), radius(radius), materialId(materialId)
    {
    }

    void set(Shader &shader, const std::string name)
    {
        shader.setUniform(name + ".Origin", this->origin);
        shader.setUniform(name + ".Radius", this->radius);
        shader.setUniform(name + ".MaterialId", this->materialId);
    }
};