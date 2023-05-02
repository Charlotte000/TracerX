#include <math.h>
#include <TracerX/VectorMath.h>

namespace TracerX
{

sf::Vector3f cross(sf::Vector3f a, sf::Vector3f b)
{
    return sf::Vector3f(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

sf::Vector3f mult(sf::Vector3f a, sf::Vector3f b)
{
    return sf::Vector3f(a.x * b.x, a.y * b.y, a.z * b.z);
}

float dot(sf::Vector3f a, sf::Vector3f b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

sf::Vector3f normalized(sf::Vector3f v)
{
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return v / length;
}

sf::Vector3f rotateAroundAxis(sf::Vector3f v, sf::Vector3f axis, float angle)
{
    return v * std::cos(angle) + cross(axis, v) * std::sin(angle) + axis * dot(axis, v) * (1 - std::cos(angle));
}

sf::Vector3f rotateX(sf::Vector3f v, float angle)
{
    return sf::Vector3f(v.x, v.y * std::cos(angle) - v.z * std::sin(angle), v.y * std::sin(angle) + v.z * std::cos(angle));
}

sf::Vector3f rotateY(sf::Vector3f v, float angle)
{
    return sf::Vector3f(v.x * std::cos(angle) + v.z * std::sin(angle), v.y, -v.x * std::sin(angle) + v.z * std::cos(angle));
}

sf::Vector3f rotateZ(sf::Vector3f v, float angle)
{
    return sf::Vector3f(v.x * std::cos(angle) - v.y * std::sin(angle), v.x * std::sin(angle) + v.y * std::cos(angle), v.z);
}

float length(sf::Vector3f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

sf::Vector3f min(sf::Vector3f a, sf::Vector3f b)
{
    return sf::Vector3f(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

sf::Vector3f max(sf::Vector3f a, sf::Vector3f b)
{
    return sf::Vector3f(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

}