#pragma once

#include "Material.h"
#include "VectorMath.h"

struct Triangle
{
    Vector3f pos1;
    Vector3f pos2;
    Vector3f pos3;
    Vector3f normal1;
    Vector3f normal2;
    Vector3f normal3;
    int materialId;

    Triangle(Vector3f pos1, Vector3f pos2, Vector3f pos3, Vector3f normal1, Vector3f normal2, Vector3f normal3)
        : pos1(pos1), pos2(pos2), pos3(pos3), normal1(normal1), normal2(normal2), normal3(normal3), materialId(0)
    {
    }

    Triangle(Vector3f pos1, Vector3f pos2, Vector3f pos3, Vector3f normal1, Vector3f normal2, Vector3f normal3, int materialId)
        : pos1(pos1), pos2(pos2), pos3(pos3), normal1(normal1), normal2(normal2), normal3(normal3), materialId(materialId)
    {
    }

    Triangle(Vector3f pos1, Vector3f pos2, Vector3f pos3)
        : pos1(pos1), pos2(pos2), pos3(pos3), materialId(0)
    {
        Vector3f normal = normalized(cross(pos2 - pos1, pos3 - pos1));
        this->normal1 = normal;
        this->normal2 = normal;
        this->normal3 = normal;
    }

    Triangle(Vector3f pos1, Vector3f pos2, Vector3f pos3, int materialId)
        : pos1(pos1), pos2(pos2), pos3(pos3), materialId(materialId)
    {
        Vector3f normal = normalized(cross(pos2 - pos1, pos3 - pos1));
        this->normal1 = normal;
        this->normal2 = normal;
        this->normal3 = normal;
    }

    void set(Shader& shader, const std::string name)
    {
        shader.setUniform(name + ".Pos1", this->pos1);
        shader.setUniform(name + ".Pos2", this->pos2);
        shader.setUniform(name + ".Pos3", this->pos3);
        shader.setUniform(name + ".Normal1", this->normal1);
        shader.setUniform(name + ".Normal2", this->normal2);
        shader.setUniform(name + ".Normal3", this->normal3);
        shader.setUniform(name + ".MaterialId", this->materialId);
    }
};