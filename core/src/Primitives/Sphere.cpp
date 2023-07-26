#include <TracerX/Primitives/Sphere.h>

namespace TracerX
{

Sphere::Sphere(sf::Vector3f origin, float radius, int materialId)
    : origin(origin), radius(radius), materialId(materialId)
{
}

}