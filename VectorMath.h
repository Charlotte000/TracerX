#pragma once

#include <SFML/System.hpp>

using namespace sf;

Vector3f cross(Vector3f a, Vector3f b);

Vector3f mult(Vector3f a, Vector3f b);

float dot(Vector3f a, Vector3f b);

Vector3f normalized(Vector3f v);

Vector3f rotateAroundAxis(Vector3f v, Vector3f axis, float angle);

Vector3f rotateX(Vector3f v, float angle);

Vector3f rotateY(Vector3f v, float angle);

Vector3f rotateZ(Vector3f v, float angle);

float length(Vector3f v);
