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
    struct Rendering
    {
        unsigned int sampleCountTarget = 0;
        unsigned int samplesPerFrame = 1;
        bool enable = false;
        bool isPreview = true;
        bool needClear = false;
    } rendering;
    struct CameraControl
    {
        bool enable = false;
        float speed = 5;
        float rotationSpeed = 1;
    } cameraControl;
    struct Tiling
    {
        unsigned int count = 0;
        unsigned int factor = 1;
    } tiling;
    struct RenderView
    {
        glm::vec2 uvCenter = glm::vec2(.5f);
        glm::vec2 uvSize = glm::vec2(1);
        glm::vec2 size;
        glm::vec2 pos;
        bool isHover = false;
    } renderView;
    struct Property
    {
        enum class Type
        {
            Contorls,
            ToneMapping,
            Settings,
            Camera,
            Environment,
            MeshInstance,
            Material,
        } type = Type::Contorls;
        void* target = nullptr;
    } property;
    struct MaterialTexture
    {
        int textureId = -1;
        TracerX::core::Texture texture;
    } materialTexture;
    struct Gizmo
    {
        ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
        ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
        float snap = 1;
    } gizmo;

    static inline const std::filesystem::path assetsFolder = std::filesystem::relative(ASSETS_PATH).string();
    static inline const std::filesystem::path environmentFolder = Application::assetsFolder / "environments" / "";
    static inline const std::filesystem::path sceneFolder = Application::assetsFolder / "scenes" / "";

    void loadAnyEnvironment();
    void control();
    float getLookAtDistance() const;
    void clear();
    void getCurrentTile(glm::uvec2& pos, glm::uvec2& size);
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
    void propertyMeshInstance(TracerX::MeshInstance& meshInstance);
    void propertyMaterial(TracerX::Material& material);
    bool materialTextureSelector(const std::string& name, int& currentTextureId, glm::vec3 tintColor);
    void drawFillImage(GLint textureHandler, glm::vec2 srcSize, glm::vec2& imagePos, glm::vec2& imageSize, glm::vec3 tintColor = glm::vec3(1), glm::vec2 uvLo = glm::vec2(0), glm::vec2 uvUp = glm::vec2(1), bool flipY = false);
#pragma endregion UI
};

glm::vec2 toVec2(const ImVec2 v);
ImVec2 toImVec2(const glm::vec2 v);
