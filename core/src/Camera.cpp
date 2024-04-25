#include "TracerX/Camera.h"

#include <glm/gtc/matrix_transform.hpp>


void Camera::lookAt(glm::vec3 position)
{
    this->forward = glm::normalize(position - this->position);
    this->up = glm::normalize(glm::cross(glm::cross(this->forward, this->up), this->forward));
    this->focalDistance = glm::length(this->position - position);
}

glm::mat4 Camera::createView() const
{
    return glm::lookAt(this->position, this->position + this->forward, this->up);
}

glm::mat4 Camera::createProjection(float aspectRatio) const
{
    float fovY = atanf(tanf(this->fov / 2) / aspectRatio) * 2;
    return glm::infinitePerspective(fovY, aspectRatio, .0001f);
}
