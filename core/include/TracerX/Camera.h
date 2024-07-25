/**
 * @file Camera.h
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace TracerX
{

/**
 * @brief Represents a camera in a 3D scene.
 */
struct Camera
{
public:
    /**
     * @brief The position of the camera.
     */
    glm::vec3 position = glm::vec3(0, 0, 10);

    /**
     * @brief The field of view of the camera.
     */
    float fov = glm::half_pi<float>();

    /**
     * @brief The forward direction of the camera.
     */
    glm::vec3 forward = glm::vec3(0, 0, -1);

    /**
     * @brief The focal distance of the camera.
     */
    float focalDistance = 1;

    /**
     * @brief The up direction of the camera.
     */
    glm::vec3 up = glm::vec3(0, 1, 0);

    /**
     * @brief The aperture of the camera.
     */
    float aperture = 0;

    /**
     * @brief The blur amount of the camera.
     * 
     */
    float blur = 0;

    /**
     * @brief Sets the camera to look at the specified position.
     * 
     * Sets Camera::focalDistance to the distance between the camera and the position.
     * 
     * @param position The position to look at.
     */
    void lookAt(glm::vec3 position);

    /**
     * @brief Creates a view matrix for the camera.
     * @return The view matrix.
     */
    glm::mat4 createView() const;

    /**
     * @brief Creates a projection matrix for the camera.
     * @param width The width of the viewport.
     * @param height The height of the viewport.
     * @param zNear The near clipping plane.
     * @param zFar The far clipping plane.
     * @return The projection matrix.
     */
    glm::mat4 createProjection(float width, float height, float zNear, float zFar) const;
private:
    int padding1;
    int padding2;
    int padding3;
};

}
