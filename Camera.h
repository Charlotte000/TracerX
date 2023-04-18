#pragma once

#include <SFML/System.hpp>
#include "VectorMath.h"

struct Camera
{
    Vector3f up;
    Vector3f forward;
    Vector3f position;
    float focalLength;
    float focusStrength;

    Camera(Vector3f position, Vector3f forward, Vector3f up, float focalLength, float focusStrength)
        : up(up), forward(forward), position(position), focalLength(focalLength), focusStrength(focusStrength)
    {
    }

    void move(RenderWindow& window)
    {
        Vector3f right = cross(this->forward, this->up);

        const float moveSensitivity = 10;
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            this->position += this->forward / moveSensitivity;
        }

        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            this->position -= this->forward / moveSensitivity;
        }

        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            this->position += right / moveSensitivity;
        }

        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            this->position -= right / moveSensitivity;
        }

        if (Keyboard::isKeyPressed(Keyboard::LShift))
        {
            this->position += this->up / moveSensitivity;
        }

        if (Keyboard::isKeyPressed(Keyboard::LControl))
        {
            this->position -= this->up / moveSensitivity;
        }

        if (Keyboard::isKeyPressed(Keyboard::E))
        {
            this->up = rotateAroundAxis(this->up, this->forward, 1 / 100.f);
        }

        if (Keyboard::isKeyPressed(Keyboard::Q))
        {
            this->up = rotateAroundAxis(this->up, this->forward, -1 / 100.f);
        }


        Vector2i center = (Vector2i)window.getSize() / 2;
        Vector2f mouseDelta = ((Vector2f)Mouse::getPosition(window) - (Vector2f)center) / 100.f;
        Mouse::setPosition(center, window);

        this->forward = rotateAroundAxis(this->forward, right, -mouseDelta.y);
        this->up = rotateAroundAxis(this->up, right, -mouseDelta.y);

        this->forward = rotateAroundAxis(this->forward, this->up, -mouseDelta.x);
    }
};