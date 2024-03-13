#pragma once

#include "UI.h"
#include "Shader.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


class Application
{
public:
    UI ui;
    Renderer renderer;
    Scene scene;
    GLFWwindow* window = nullptr;
    float cameraSpeed = 5.f;
    float cameraRotationSpeed = 1.f;
    std::vector<std::string> environmentFiles;
    std::vector<std::string> sceneFiles;
    bool isRendering = false;

    static inline const std::string environmentFolder = "../assets/environments/";
    static inline const std::string sceneFolder = "../assets/scenes/";

    Application();
    void init(glm::ivec2 size);
    void shutdown();
    void run();
    void save();
    void control();
private:
};
