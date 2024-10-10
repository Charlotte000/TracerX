#pragma once

#ifndef ASSETS_PATH
#define ASSETS_PATH std::filesystem::current_path()
#endif

#include <TracerX/Renderer.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <GLFW/glfw3.h>

class Application
{
public:
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
        glm::uvec2 factor = glm::uvec2(1);

        void tick();
        bool isLastTick() const;
        void getTile(glm::uvec2 canvasSize, glm::uvec2& pos, glm::uvec2& size);
    } tiling;
    struct RenderTextureView
    {
        glm::vec2 uvCenter = glm::vec2(.5f);
        glm::vec2 uvSize = glm::vec2(1);
        glm::vec2 size;
        glm::vec2 pos;
        bool isHover = false;

        void reset();
        void clamp();
        void getUVRect(glm::vec2& lo, glm::vec2& up) const;
        void getRectFromUV(glm::vec2& lo, glm::vec2& up) const;
    } renderTextureView;
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
    struct MaterialTextureView
    {
        int textureId = -1;
        TracerX::core::GL::Texture texture;
    } materialTextureView;
    struct Gizmo
    {
        ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
        ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
        float snap = 1;
    } gizmo;

    static inline const std::filesystem::path assetsFolder = std::filesystem::relative(ASSETS_PATH).string();
    static inline const std::filesystem::path environmentFolder = Application::assetsFolder / "environments" / "";
    static inline const std::filesystem::path sceneFolder = Application::assetsFolder / "scenes" / "";

    void init(glm::uvec2 size);
    void shutdown();
    void run();
    void loadScene(const std::filesystem::path& path);
    void loadAnyEnvironment();
    void control();
    float getLookAtDistance() const;
    void clear();
    void initUI();
    void renderUI();
    void shutdownUI();
};

glm::vec2 toVec2(const ImVec2 v);
ImVec2 toImVec2(const glm::vec2 v);
