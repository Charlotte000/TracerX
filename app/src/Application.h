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
    const glm::uvec2 maxTextureArraySize;
    bool isSceneLoaded = true;
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
    struct RenderTextureView
    {
        glm::vec2 uvCenter = glm::vec2(.5f);
        glm::vec2 uvSize = glm::vec2(1);
        glm::vec2 size;
        glm::vec2 pos;
        bool isHover = false;

        void reset();
        void clamp();
        void getUV(glm::vec2& lo, glm::vec2& up) const;
        void getRectFromUV(glm::vec2& lo, glm::vec2& up) const;
        void control();
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

    static inline std::filesystem::path environmentDir;
    static inline std::filesystem::path sceneDir;
#if !TX_SPIRV
    static inline std::filesystem::path shaderPath;
#endif

    Application(glm::uvec2 initSize, const TracerX::Scene& initScene, const TracerX::Image& initEnvironment, glm::uvec2 maxTextureArraySize);
    void shutdown();
    void run();
    void loadScene(const std::filesystem::path& path);
    void control();
    float getLookAtDistance() const;
    void setCameraMode(CameraControl::Mode mode);
    void switchRendering();
#if !TX_SPIRV
    void reloadShaders();
#endif
    void clear();
    void initUI();
    void renderUI();
    void shutdownUI();
};

glm::vec2 toVec2(const ImVec2 v);
ImVec2 toImVec2(const glm::vec2 v);
