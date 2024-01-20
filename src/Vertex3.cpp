#include <TracerX/Vertex3.h>

namespace TracerX
{

Vertex3::Vertex3(sf::Vector3f position, sf::Vector3f normal)
    : position(position), normal(normal)
{
}

Vertex3::Vertex3(sf::Vector3f position, sf::Vector3f normal, sf::Vector2f textureCoordinate)
    : position(position), normal(normal), textureCoordinate(textureCoordinate)
{
}

}