#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

struct Vertex3
{
    Vector3f position;
    Vector3f normal;
    Vector2f textureCoordinate = Vector2f(0, 0);

    Vertex3(Vector3f position, Vector3f normal)
        : position(position), normal(normal)
    {
    }

    Vertex3(Vector3f position, Vector3f normal, Vector2f textureCoordinate)
        : position(position), normal(normal), textureCoordinate(textureCoordinate)
    {
    }

    void set(Shader& shader, const std::string name)
    {
        shader.setUniform(name + ".Position", this->position);
        shader.setUniform(name + ".Normal", this->normal);
        shader.setUniform(name + ".TextureCoordinate", this->textureCoordinate);
    }
};