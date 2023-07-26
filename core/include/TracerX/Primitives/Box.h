#pragma once

#include <SFML/Graphics.hpp>

namespace TracerX
{

struct Box
{
    alignas(16) sf::Vector3f origin;
    alignas(16) sf::Vector3f size;
    alignas(16) sf::Vector3f rotation = sf::Vector3f(0, 0, 0);
    alignas(16) sf::Vector3f boxMin = sf::Vector3f(0, 0, 0);
    alignas(16) sf::Vector3f boxMax = sf::Vector3f(0, 0, 0);
    int materialId;

    Box(sf::Vector3f origin, sf::Vector3f size, int materialId = -1);

    void updateAABB();

private:
    sf::Vector3f getVertex(sf::Vector3f v);
};

}