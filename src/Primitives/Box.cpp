#include <TracerX/Primitives/Box.h>

namespace TracerX
{

Box::Box(sf::Vector3f origin, sf::Vector3f size)
    : origin(origin), size(size), materialId(0)
{
}

Box::Box(sf::Vector3f origin, sf::Vector3f size, int materialId)
    : origin(origin), size(size), materialId(materialId)
{
}

}