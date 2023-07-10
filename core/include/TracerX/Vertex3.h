#pragma once

#include <SFML/System.hpp>

namespace TracerX
{

struct Vertex3
{
    alignas(16) sf::Vector3f position;
    alignas(16) sf::Vector3f normal;
    alignas(8) sf::Vector2f textureCoordinate = sf::Vector2f(0, 0);

    Vertex3(sf::Vector3f position, sf::Vector3f normal);

    Vertex3(sf::Vector3f position, sf::Vector3f normal, sf::Vector2f textureCoordinate);
};

}