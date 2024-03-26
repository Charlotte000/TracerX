#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


struct Camera
{
    glm::vec3 position = glm::vec3(0, 0, -10);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 forward = glm::vec3(0, 0, 1);
    float fov = glm::half_pi<float>();
    float focalDistance = 1;
    float aperture = 0;
    float aspectRatio = 1;
    float blur = 0;

    void lookAt(glm::vec3 position);
    glm::mat4 createView() const;
    glm::mat4 createProjection() const;
};
