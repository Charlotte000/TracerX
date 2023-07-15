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
    this->prevPosition.set(position);
    this->prevForward.set(forward);
    this->prevUp.set(up);
    this->focalLength.set(focalLength);
    this->focusStrength.set(focusStrength);
    this->fov.set(fov);
}

void Camera::create(sf::Shader* shader)
{
    this->position.create(shader, "Camera.NextPosition");
    this->forward.create(shader, "Camera.NextForward");
    this->up.create(shader, "Camera.NextUp");
    this->prevPosition.create(shader, "Camera.PrevPosition");
    this->prevForward.create(shader, "Camera.PrevForward");
    this->prevUp.create(shader, "Camera.PrevUp");
    this->focalLength.create(shader, "Camera.FocalLength");
    this->focusStrength.create(shader, "Camera.FocusStrength");
    this->fov.create(shader, "Camera.FOV");
}

void Camera::updateShader()
{
    this->position.updateShader();
    this->forward.updateShader();
    this->up.updateShader();
    this->prevPosition.updateShader();
    this->prevForward.updateShader();
    this->prevUp.updateShader();
    this->focalLength.updateShader();
    this->focusStrength.updateShader();
    this->fov.updateShader();
}

void Camera::move(const sf::RenderWindow& window)
{
    const float moveSensitivity = 10;

    sf::Vector3f right = this->getRight();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->position.set(this->position.get() + this->forward.get() / moveSensitivity);
        this->prevPosition.set(this->position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->position.set(this->position.get() - this->forward.get() / moveSensitivity);
        this->prevPosition.set(this->position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->position.set(this->position.get() + right / moveSensitivity);
        this->prevPosition.set(this->position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->position.set(this->position.get() - right / moveSensitivity);
        this->prevPosition.set(this->position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        this->position.set(this->position.get() + this->up.get() / moveSensitivity);
        this->prevPosition.set(this->position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        this->position.set(this->position.get() - this->up.get() / moveSensitivity);
        this->prevPosition.set(this->position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        this->up.set(rotateAroundAxis(this->up.get(), this->forward.get(), 1 / 100.f));
        this->prevUp.set(this->up.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        this->up.set(rotateAroundAxis(this->up.get(), this->forward.get(), -1 / 100.f));
        this->prevUp.set(this->up.get());
    }


    sf::Vector2i center = (sf::Vector2i)window.getSize() / 2;
    sf::Vector2f mouseDelta = ((sf::Vector2f)sf::Mouse::getPosition(window) - (sf::Vector2f)center) / 100.f;
    sf::Mouse::setPosition(center, window);

    this->forward.set(rotateAroundAxis(this->forward.get(), right, -mouseDelta.y));
    this->up.set(rotateAroundAxis(this->up.get(), right, -mouseDelta.y));

    this->forward.set(rotateAroundAxis(this->forward.get(), this->up.get(), -mouseDelta.x));

    this->prevForward.set(this->forward.get());
    this->prevUp.set(this->up.get());
}

void Camera::lookAt(sf::Vector3f position)
{
    this->forward.set(normalized(position - this->position.get()));
    this->up.set(normalized(cross(cross(this->forward.get(), this->up.get()), this->forward.get())));
    this->focalLength.set(length(this->position.get() - position));
}

sf::Vector3f Camera::getRight()
{
    return cross(this->forward.get(), this->up.get());
}

}