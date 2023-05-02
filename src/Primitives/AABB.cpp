#include <TracerX/Primitives/AABB.h>

namespace TracerX
{

AABB::AABB(sf::Vector3f origin, sf::Vector3f size)
    : origin(origin), size(size), materialId(0)
{
}

AABB::AABB(sf::Vector3f origin, sf::Vector3f size, int materialId)
    : origin(origin), size(size), materialId(materialId)
{
}

}