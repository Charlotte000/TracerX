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

void Camera::create(sf::Shader* shader, const std::string& name)
{
    this->name = name;
    this->position.create(shader, name + ".Position");
    this->forward.create(shader, name + ".Forward");
    this->up.create(shader, name + ".Up");
    this->focalLength.create(shader, name + ".FocalLength");
    this->focusStrength.create(shader, name + ".FocusStrength");
    this->fov.create(shader, name + ".FOV");
}

void Camera::updateShader()
{
    this->position.updateShader();
    this->forward.updateShader();
    this->up.updateShader();
    this->focalLength.updateShader();
    this->focusStrength.updateShader();
    this->fov.updateShader();
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