#include <TracerX/VectorMath.h>
#include <TracerX/Camera.h>

namespace TracerX
{

Camera::Camera()
{
}

Camera::Camera(sf::Vector3f position, sf::Vector3f forward, sf::Vector3f up, float focalLength, float focusStrength, float fov)
{
    this->position.set(position);
    this->forward.set(forward);
    this->up.set(up);
    this->focalLength.set(focalLength);
    this->focusStrength.set(focusStrength);
    this->fov.set(fov);
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
    if (this->forward.hasChanged && this->up.hasChanged)
    {
        this->right.set(cross(this->forward.get(), this->up.get()));
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
    const float moveSensitivity = 10;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->position.set(this->position.get() + this->forward.get() / moveSensitivity);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->position.set(this->position.get() - this->forward.get() / moveSensitivity);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->position.set(this->position.get() + this->right.get() / moveSensitivity);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->position.set(this->position.get() - this->right.get() / moveSensitivity);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        this->position.set(this->position.get() + this->up.get() / moveSensitivity);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        this->position.set(this->position.get() - this->up.get() / moveSensitivity);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        this->up.set(rotateAroundAxis(this->up.get(), this->forward.get(), 1 / 100.f));
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        this->up.set(rotateAroundAxis(this->up.get(), this->forward.get(), -1 / 100.f));
    }


    sf::Vector2i center = (sf::Vector2i)window.getSize() / 2;
    sf::Vector2f mouseDelta = ((sf::Vector2f)sf::Mouse::getPosition(window) - (sf::Vector2f)center) / 100.f;
    sf::Mouse::setPosition(center, window);

    this->forward.set(rotateAroundAxis(this->forward.get(), this->right.get(), -mouseDelta.y));
    this->up.set(rotateAroundAxis(this->up.get(), this->right.get(), -mouseDelta.y));

    this->forward.set(rotateAroundAxis(this->forward.get(), this->up.get(), -mouseDelta.x));
}

void Camera::lookAt(sf::Vector3f position)
{
    this->forward.set(normalized(position - this->position.get()));
    this->up.set(normalized(cross(cross(this->forward.get(), this->up.get()), this->forward.get())));
    this->focalLength.set(length(this->position.get() - position));
}

}