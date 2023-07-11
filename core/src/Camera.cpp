#include <TracerX/VectorMath.h>
#include <TracerX/Camera.h>

namespace TracerX
{

Camera::Camera()
{
}

Camera::Camera(sf::Vector3f position, sf::Vector3f forward, sf::Vector3f up, float focalLength, float focusStrength, float fov)
{
    this->position.value = position;
    this->forward.value = forward;
    this->up.value = up;
    this->focalLength.value = focalLength;
    this->focusStrength.value = focusStrength;
    this->fov.value = fov;
}

void Camera::create(sf::Shader* shader)
{
    this->position.create(shader, "Camera.Position");
    this->forward.create(shader, "Camera.Forward");
    this->up.create(shader, "Camera.Up");
    this->right.create(shader, "Camera.Right");
    this->focalLength.create(shader, "Camera.FocalLength");
    this->focusStrength.create(shader, "Camera.FocusStrength");
    this->fov.create(shader, "Camera.FOV");
}

void Camera::updateShader()
{
    if (this->forward.hasChanged() && this->up.hasChanged())
    {
        this->right.value = cross(this->forward.value, this->up.value);
    }

    this->position.updateShader();
    this->forward.updateShader();
    this->up.updateShader();
    this->right.updateShader();
    this->focalLength.updateShader();
    this->focusStrength.updateShader();
    this->fov.updateShader();
}

void Camera::move(const sf::RenderWindow& window)
{
    sf::Vector3f right = cross(this->forward.value, this->up.value);

    const float moveSensitivity = 10;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->position.value += this->forward.value / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->position.value -= this->forward.value / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->position.value += right / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->position.value -= right / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        this->position.value += this->up.value / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        this->position.value -= this->up.value / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        this->up.value = rotateAroundAxis(this->up.value, this->forward.value, 1 / 100.f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        this->up.value = rotateAroundAxis(this->up.value, this->forward.value, -1 / 100.f);
    }


    sf::Vector2i center = (sf::Vector2i)window.getSize() / 2;
    sf::Vector2f mouseDelta = ((sf::Vector2f)sf::Mouse::getPosition(window) - (sf::Vector2f)center) / 100.f;
    sf::Mouse::setPosition(center, window);

    this->forward.value = rotateAroundAxis(this->forward.value, right, -mouseDelta.y);
    this->up.value = rotateAroundAxis(this->up.value, right, -mouseDelta.y);

    this->forward.value = rotateAroundAxis(this->forward.value, this->up.value, -mouseDelta.x);
}

void Camera::lookAt(sf::Vector3f position)
{
    this->forward.value = normalized(position - this->position.value);
    this->up.value = normalized(cross(cross(this->forward.value, this->up.value), this->forward.value));
    this->focalLength.value = length(this->position.value - position);
}

}