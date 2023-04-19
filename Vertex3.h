#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

struct Vertex3
{
    Vector3f position;
    Vector3f normal;

    Vertex3(Vector3f position, Vector3f normal)
        : position(position), normal(normal)
    {
    }

    void set(Shader& shader, const std::string name)
    {
        shader.setUniform(name + ".Position", this->position);
        shader.setUniform(name + ".Normal", this->normal);
    }
};