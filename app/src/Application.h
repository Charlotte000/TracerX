#pragma once

#ifndef ASSETS_PATH
#define ASSETS_PATH std::filesystem::current_path()
#endif

#include "UI.h"

#include <TracerX/Scene.h>
#include <TracerX/Renderer.h>

#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Application
{
public:
    UI ui;
    TracerX::Renderer renderer;
    TracerX::Scene scene;
    GLFWwindow* window = nullptr;
    float cameraSpeed = 5;
    float cameraRotationSpeed = 1;
    unsigned int samplesPerFrame = 1;
    bool enableRendering = false;
    bool enablePreview = true;

    static inline const std::filesystem::path assetsFolder = std::filesystem::relative(ASSETS_PATH).string();
    static inline const std::filesystem::path environmentFolder = Application::assetsFolder / "environments" / "";
    static inline const std::filesystem::path sceneFolder = Application::assetsFolder / "scenes" / "";

    void init(glm::uvec2 size);
    void shutdown();
    void run();
    void loadScene(const std::string& fileName);
private:
    void loadAnyEnvironment();
    void control();
};
