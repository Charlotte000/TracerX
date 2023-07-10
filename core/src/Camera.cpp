#include <TracerX/VectorMath.h>
#include <TracerX/Camera.h>

namespace TracerX
{

Camera::Camera(sf::Vector3f position, sf::Vector3f forward, sf::Vector3f up, float focalLength, float focusStrength)
    : up(up), forward(forward), position(position), focalLength(focalLength), focusStrength(focusStrength)
{
}

void Camera::move(const sf::RenderWindow& window)
{
    sf::Vector3f right = cross(this->forward, this->up);

    const float moveSensitivity = 10;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->position += this->forward / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->position -= this->forward / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->position += right / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->position -= right / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        this->position += this->up / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        this->position -= this->up / moveSensitivity;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        this->up = rotateAroundAxis(this->up, this->forward, 1 / 100.f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        this->up = rotateAroundAxis(this->up, this->forward, -1 / 100.f);
    }


    sf::Vector2i center = (sf::Vector2i)window.getSize() / 2;
    sf::Vector2f mouseDelta = ((sf::Vector2f)sf::Mouse::getPosition(window) - (sf::Vector2f)center) / 100.f;
    sf::Mouse::setPosition(center, window);

    this->forward = rotateAroundAxis(this->forward, right, -mouseDelta.y);
    this->up = rotateAroundAxis(this->up, right, -mouseDelta.y);

    this->forward = rotateAroundAxis(this->forward, this->up, -mouseDelta.x);
}

void Camera::lookAt(sf::Vector3f position)
{
    this->forward = normalized(position - this->position);
    this->up = normalized(cross(cross(this->forward, this->up), this->forward));
    this->focalLength = length(this->position - position);
}

void Camera::set(sf::Shader& shader) const
{
    shader.setUniform("Camera.Position", this->position);
    shader.setUniform("Camera.Forward", this->forward);
    shader.setUniform("Camera.Up", this->up);
    shader.setUniform("Camera.Right", cross(this->forward, this->up));
    shader.setUniform("Camera.FocalLength", this->focalLength);
    shader.setUniform("Camera.FocusStrength", this->focusStrength);
    shader.setUniform("Camera.FOV", this->fov);
}

}