#pragma once

#include <SFML/Graphics.hpp>

namespace TracerX
{

struct Box
{
    alignas(16) sf::Vector3f origin;
    alignas(16) sf::Vector3f size;
    int materialId;

    Box(sf::Vector3f origin, sf::Vector3f size);

    Box(sf::Vector3f origin, sf::Vector3f size, int materialId);
};

}