#pragma once

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
    GLFWwindow* threadContext = nullptr;
    bool isSceneLoaded = true;
    bool isHoverTexture = false;
    const glm::uvec2 maxTextureArraySize;
#if !NDEBUG
    const std::filesystem::path shaderPath;
#endif
    const std::filesystem::path sceneDir;
    const std::filesystem::path environmentDir;

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
        bool enableFree = false;
        float movementSpeed = 5, rotationSpeed = 1;
        enum class Mode
        {
            Free,
            Orbit,
        } mode = Mode::Free;
        glm::vec3 orbitOrigin = glm::vec3(0);

        bool controlFree(TracerX::Camera& camera);
        bool controlOrbit(TracerX::Camera& camera);
    } cameraControl;
    struct Tiling
    {
        unsigned int count = 0;
        glm::uvec2 factor = glm::uvec2(1);

        void tick();
        bool isLastTick() const;
        void getTile(glm::uvec2 canvasSize, glm::uvec2& pos, glm::uvec2& size);
    } tiling;
    struct ZoomTexture
    {
        float zoom = .125;
        glm::vec2 uvCenter = glm::vec2(.5);
        bool enable = false;

        void control();
        void getUV(float aspectRatio, glm::vec2& lo, glm::vec2& up);
    } zoomTexture;
    struct Property
    {
        enum class Type
        {
            Controls,
            ToneMapping,
            Settings,
            Camera,
            Environment,
            MeshInstance,
            Material,
        } type = Type::Controls;
        size_t id = 0;
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

    Application(
        glm::uvec2 initSize,
        glm::uvec2 maxTextureArraySize,
        const std::filesystem::path sceneDir,
        const std::filesystem::path environmentDir,
#if !NDEBUG
        const std::filesystem::path shaderPath,
#endif
        const TracerX::Scene& initScene,
        const TracerX::Image& initEnvironment);
    void shutdown();
    void run();
    void loadScene(const std::filesystem::path& path);
    void control();
    float getLookAtDistance() const;
    void setCameraMode(CameraControl::Mode mode);
    void switchRendering();
#if !NDEBUG
    void reloadShaders();
#endif
    void clear();
    void initUI();
    void renderUI();
    void shutdownUI();
};

glm::vec2 toVec2(const ImVec2 v);
ImVec2 toImVec2(const glm::vec2 v);
