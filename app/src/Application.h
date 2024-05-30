#pragma once

#include "UI.h"

#include <TracerX/Scene.h>
#include <TracerX/Renderer.h>

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Application
{
public:
    UI ui;
    TracerX::Renderer renderer;
    TracerX::Scene scene;
    GLFWwindow* window = nullptr;
    float cameraSpeed = 5.f;
    float cameraRotationSpeed = 1.f;
    int perFrameCount = 1;
    std::vector<std::string> environmentFiles;
    std::vector<std::string> sceneFiles;
    bool isRendering = false;

    static inline const std::string assetsFolder = ASSETS_PATH;
    static inline const std::string environmentFolder =  Application::assetsFolder + "/environments/";
    static inline const std::string sceneFolder = Application::assetsFolder + "/scenes/";

    Application();
    void init(glm::ivec2 size);
    void shutdown();
    void run();
    void save() const;
private:
    void control();
};
