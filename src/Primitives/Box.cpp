#include <limits>
#include <TracerX/VectorMath.h>
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

void Box::updateAABB()
{
    this->boxMin = sf::Vector3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    this->boxMax = sf::Vector3f(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    
    sf::Vector3f vertex;
    vertex = this->getVertex(sf::Vector3f(.5f, .5f, .5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(.5f, .5f, -.5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(.5f, -.5f, .5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(.5f, -.5f, -.5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(-.5f, .5f, .5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(-.5f, .5f, -.5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(-.5f, -.5f, .5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);

    vertex = this->getVertex(sf::Vector3f(-.5f, -.5f, -.5f));
    this->boxMin = min(this->boxMin, vertex);
    this->boxMax = max(this->boxMax, vertex);
}

sf::Vector3f Box::getVertex(sf::Vector3f v)
{
    return this->origin + rotateZ(rotateY(rotateX(mult(this->size, v), this->rotation.x), this->rotation.y), this->rotation.z);
}

}