#pragma once

#ifndef ASSETS_PATH
#define ASSETS_PATH std::filesystem::current_path()
#endif

#include <TracerX/Mesh.h>
#include <TracerX/Scene.h>
#include <TracerX/Texture.h>
#include <TracerX/Material.h>
#include <TracerX/Renderer.h>

#include <string>
#include <imgui.h>
#include <ImGuizmo.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Application
{
public:
    void init(glm::uvec2 size);
    void shutdown();
    void run();
    void loadScene(const std::filesystem::path& path);
private:
    TracerX::Renderer renderer;
    TracerX::Scene scene;
    GLFWwindow* window = nullptr;
    float cameraSpeed = 5;
    float cameraRotationSpeed = 1;
    unsigned int samplesPerFrame = 1;
    bool enableRendering = false;
    bool enablePreview = true;
    enum class PropertyOption
    {
        PContorls,
        PToneMapping,
        PSettings,
        PCamera,
        PEnvironment,
        PMesh,
        PMaterial,
    } property = PropertyOption::PContorls;
    void* edit = nullptr;
    int currentTextureId = -1;
    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
    TracerX::core::Texture textureView;

    static inline const std::filesystem::path assetsFolder = std::filesystem::relative(ASSETS_PATH).string();
    static inline const std::filesystem::path environmentFolder = Application::assetsFolder / "environments" / "";
    static inline const std::filesystem::path sceneFolder = Application::assetsFolder / "scenes" / "";

    void loadAnyEnvironment();
    void control();
#pragma region UI
    void initUI();
    void renderUI();
    void shutdownUI();
    void mainMenuBar();
    void mainWindow();
    void drawingPanel();
    void sidePanel();
    void viewTexture(GLint textureHandler);
    void propertySelector();
    void propertyEditor();
    void propertyControls();
    void propertyToneMapping();
    void propertySettings();
    void propertyCamera();
    void propertyEnvironment();
    void propertyMesh(TracerX::Mesh& mesh);
    void propertyMaterial(TracerX::Material& material);
    bool materialTextureSelector(const std::string& name, int& currentTextureId, glm::vec3 tintColor);
#pragma endregion UI
};
