#pragma once

#include <SFML/Graphics.hpp>

namespace TracerX
{

struct AABB
{
    alignas(16) sf::Vector3f origin;
    alignas(16) sf::Vector3f size;
    int materialId;

    AABB(sf::Vector3f origin, sf::Vector3f size);

    AABB(sf::Vector3f origin, sf::Vector3f size, int materialId);
};

}