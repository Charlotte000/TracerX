#pragma once

#include <SFML/System.hpp>
#include <fstream>
#include <TracerX/Material.h>
#include <TracerX/Primitives/Sphere.h>
#include <TracerX/Primitives/Box.h>
#include <TracerX/Primitives/Mesh.h>
#include <TracerX/Vertex3.h>
#include <TracerX/Camera.h>
#include <TracerX/Environment.h>

#define SERIALIZE(type) \
std::ifstream& operator >>(std::ifstream& input, type& value); \
std::ofstream& operator <<(std::ofstream& output, const type& value);

namespace TracerX
{

SERIALIZE(sf::Vector2f);

SERIALIZE(sf::Vector3f);

SERIALIZE(Material);

SERIALIZE(Sphere);

SERIALIZE(Box);

SERIALIZE(Mesh);

SERIALIZE(Vertex3);

SERIALIZE(Camera);

SERIALIZE(Environment);

}