#pragma once

#ifndef ASSETS_PATH
#define ASSETS_PATH std::filesystem::current_path()
#endif

#include "UI.h"

#include <TracerX/Scene.h>
#include <TracerX/Renderer.h>

#include <vector>
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
    float cameraSpeed = 5.f;
    float cameraRotationSpeed = 1.f;
    unsigned int perFrameCount = 1;
    bool isRendering = false;
    bool enablePreview = true;

    static inline const std::filesystem::path assetsFolder = std::filesystem::canonical(ASSETS_PATH).string();
    static inline const std::filesystem::path environmentFolder = Application::assetsFolder / "environments" / "";
    static inline const std::filesystem::path sceneFolder = Application::assetsFolder / "scenes" / "";

    void init(glm::uvec2 size);
    void shutdown();
    void run();
    void loadScene(const std::string& fileName);
    GLint getViewHandler() const;
private:
    void control();
};
