#pragma once

#include "UI.h"
#include "Shader.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


class Application
{
public:
    glm::ivec2 size;
    UI ui;
    Renderer renderer;
    Scene scene;
    Shader viewer;
    GLFWwindow* window = nullptr;
    float cameraSpeed = 5.f;
    float cameraRotationSpeed = 1.f;
    float renderScale = 1.f;
    std::vector<std::string> environmentFiles;
    std::vector<std::string> sceneFiles;

    static inline const std::string environmentFolder = "../assets/environments/";
    static inline const std::string sceneFolder = "../assets/scenes/";

    Application();
    void init(glm::ivec2 size);
    void resize(glm::ivec2 size);
    void shutdown();
    void run();
    void save();
    void control();
private:
};
