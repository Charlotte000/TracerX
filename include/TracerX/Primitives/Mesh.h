#pragma once

#include <SFML/Graphics.hpp>
#include <set>
#include <vector>

#include <TracerX/Vertex3.h>
#include <TracerX/VectorMath.h>

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

    void offset(sf::Vector3f offset, const std::vector<int>& indices, std::vector<Vertex3>& verticies);

    void scale(sf::Vector3f scale, const std::vector<int>& indices, std::vector<Vertex3>& verticies);

    void rotate(sf::Vector3f rotation, const std::vector<int>& indices, std::vector<Vertex3>& verticies);
};

}