#include <SFML/System.hpp>

#include "VectorMath.h"

using namespace sf;

Vector3f cross(Vector3f a, Vector3f b)
{
    return Vector3f(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

Vector3f mult(Vector3f a, Vector3f b)
{
    return Vector3f(a.x * b.x, a.y * b.y, a.z * b.z);
}

float dot(Vector3f a, Vector3f b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3f normalized(Vector3f v)
{
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return v / length;
}

Vector3f rotateAroundAxis(Vector3f v, Vector3f axis, float angle)
{
    return v * cos(angle) + cross(axis, v) * sin(angle) + axis * dot(axis, v) * (1 - cos(angle));
}

Vector3f rotateX(Vector3f v, float angle)
{
    return Vector3f(v.x, v.y * cos(angle) - v.z * sin(angle), v.y * sin(angle) + v.z * cos(angle));
}

Vector3f rotateY(Vector3f v, float angle)
{
    return Vector3f(v.x * cos(angle) + v.z * sin(angle), v.y, -v.x * sin(angle) + v.z * cos(angle));
}

Vector3f rotateZ(Vector3f v, float angle)
{
    return Vector3f(v.x * cos(angle) - v.y * sin(angle), v.x * sin(angle) + v.y * cos(angle), v.z);
}