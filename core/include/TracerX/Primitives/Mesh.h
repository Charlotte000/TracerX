#pragma once

#include <SFML/Graphics.hpp>
#include <TracerX/Vertex3.h>
#include <TracerX/VectorMath.h>
#include <TracerX/SSBO.h>

namespace TracerX
{

struct Mesh
{
    alignas(16) sf::Vector3f boxMin = sf::Vector3f(0, 0, 0);
    alignas(16) sf::Vector3f boxMax = sf::Vector3f(0, 0, 0);
    alignas(16) sf::Vector3f position = sf::Vector3f(0, 0, 0);
    alignas(16) sf::Vector3f size = sf::Vector3f(1, 1, 1);
    alignas(16) sf::Vector3f rotation = sf::Vector3f(0, 0, 0);
    int indicesStart;
    int indicesEnd;
    int materialId;

    Mesh(int indicesStart, int indicesEnd, int materialId);

    void offset(sf::Vector3f offset, const SSBO<int>& indices, SSBO<Vertex3>& verticies);

    void scale(sf::Vector3f scale, const SSBO<int>& indices, SSBO<Vertex3>& verticies);

    void rotate(sf::Vector3f rotation, const SSBO<int>& indices, SSBO<Vertex3>& verticies);

    void updateAABB(const SSBO<int>& indices, const SSBO<Vertex3>& verticies);
};

}