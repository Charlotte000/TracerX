#include "Application.h"

#include <TracerX/GLTFLoader.h>

#include <thread>
#include <iostream>
#include <stdexcept>
#include <tinyfiledialogs.h>
#include <glm/gtx/rotate_vector.hpp>

using namespace TracerX;

#pragma region Application Helper Functions
bool Application::CameraControl::controlFree(Camera& camera)
{
    ImGuiIO io = ImGui::GetIO();
    float elapsedTime = io.DeltaTime;
    glm::vec2 mouseDelta = toVec2(io.MouseDelta) * this->rotationSpeed / 200.f;

    bool updated = false;

    // Keyboard
    float elapsedMove = this->movementSpeed * elapsedTime;
    glm::vec3 right = glm::normalize(glm::cross(camera.forward, camera.up));
    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        camera.position += camera.forward * elapsedMove;
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        camera.position += -camera.forward * elapsedMove;
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        camera.position += -right * elapsedMove;
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
        camera.position += right * elapsedMove;
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
    {
        camera.position += camera.up * elapsedMove;
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        camera.position += -camera.up * elapsedMove;
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_Q))
    {
        camera.up = glm::rotate(camera.up, -this->rotationSpeed * 2 * elapsedTime, camera.forward);
        updated = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_E))
    {
        camera.up = glm::rotate(camera.up, this->rotationSpeed * 2 * elapsedTime, camera.forward);
        updated = true;
    }

    // Mouse
    if (mouseDelta != glm::vec2(0))
    {
        camera.forward = glm::rotate(camera.forward, -mouseDelta.y, right);
        camera.up = glm::rotate(camera.up, -mouseDelta.y, right);
        camera.forward = glm::rotate(camera.forward, -mouseDelta.x, camera.up);
        updated = true;
    }

    return updated;
}

bool Application::CameraControl::controlOrbit(Camera& camera)
{
    ImGuiIO io = ImGui::GetIO();
    float elapsedTime = io.DeltaTime;
    glm::vec2 mouseDelta = toVec2(io.MouseDelta) * this->rotationSpeed / 200.f;
    if (mouseDelta == glm::vec2(0))
    {
        return false;
    }

    float orbitRadius = glm::length(camera.position - this->orbitOrigin);
    glm::vec3 right = glm::normalize(glm::cross(camera.forward, camera.up));

    // Camera rotation
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        camera.forward = glm::rotate(camera.forward, -mouseDelta.y, right);
        camera.forward = glm::rotate(camera.forward, -mouseDelta.x, camera.up);

        camera.position = this->orbitOrigin - camera.forward * orbitRadius;
        right = glm::normalize(glm::cross(camera.forward, glm::vec3(0, 1, 0)));
        camera.up = glm::normalize(glm::cross(right, camera.forward));
        return true;
    }

    // Camera zoom
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        camera.position -= camera.forward * mouseDelta.y;
        return true;
    }

    // Camera pan
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
    {
        this->orbitOrigin += camera.up * mouseDelta.y - right * mouseDelta.x;
        camera.position = this->orbitOrigin - camera.forward * orbitRadius;
        return true;
    }

    return false;
}

void Application::RenderTextureView::reset()
{
    this->uvCenter = glm::vec2(.5);
    this->uvSize = glm::vec2(1);
}

void Application::RenderTextureView::clamp()
{
    this->uvSize = glm::clamp(this->uvSize, glm::vec2(.0001f), glm::vec2(1));
    this->uvCenter = glm::clamp(this->uvCenter, this->uvSize * .5f, 1.f - this->uvSize * .5f);
}

void Application::RenderTextureView::getUV(glm::vec2& lo, glm::vec2& up) const
{
    lo = this->uvCenter - this->uvSize * .5f;
    up = this->uvCenter + this->uvSize * .5f;
}

void Application::RenderTextureView::getRectFromUV(glm::vec2& lo, glm::vec2& up) const
{
    lo = lo * this->size + this->pos;
    up = up * this->size + this->pos;
}

void Application::RenderTextureView::control()
{
    ImGuiIO io = ImGui::GetIO();

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && this->uvSize != glm::vec2(1))
    {
        this->uvCenter -= toVec2(io.MouseDelta) * this->uvSize / this->size;
        this->clamp();
    }

    if (io.MouseWheel > 0)
    {
        this->uvSize *= .9f;
        this->clamp();
    }
    else if (io.MouseWheel < 0)
    {
        this->uvSize *= 1.1f;
        this->clamp();
    }
}

void Application::Tiling::tick()
{
    this->count = (this->count + 1) % (this->factor.x * this->factor.y);
}

bool Application::Tiling::isLastTick() const
{
    return this->count == this->factor.x * this->factor.y - 1;
}

void Application::Tiling::getTile(glm::uvec2 canvasSize, glm::uvec2& pos, glm::uvec2& size)
{
    this->factor = glm::clamp(this->factor, glm::uvec2(1), canvasSize);
    size = (glm::uvec2)glm::ceil((glm::vec2)canvasSize / glm::vec2(this->factor));
    pos = glm::uvec2(this->count % this->factor.x, (this->count / this->factor.x) % this->factor.y) * size;
}
#pragma endregion

Application::Application(glm::uvec2 initSize, const TracerX::Scene& initScene, const TracerX::Image& initEnvironment, glm::uvec2 maxTextureArraySize)
    : maxTextureArraySize(maxTextureArraySize), scene(initScene)
{
    // Init GLFW
    glfwSetErrorCallback([](int, const char* err)
    {
        std::cerr << "GLFW Error: " << err << std::endl;
        throw std::runtime_error(err);
    });

    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "GLFW Init Error" << std::endl;
        throw std::runtime_error("GLFW Init Error");
    }

    // Create window
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    this->window = glfwCreateWindow(initSize.x, initSize.y, "TracerX", nullptr, nullptr);
    if (this->window == nullptr)
    {
        std::cerr << "GLFW Create Window Error" << std::endl;
        throw std::runtime_error("GLFW Create Window Error");
    }

    // Create thread context
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    this->threadContext = glfwCreateWindow(1, 1, "", nullptr, this->window);
    if (this->threadContext == nullptr)
    {
        std::cerr << "GLFW Create Window Error" << std::endl;
        throw std::runtime_error("GLFW Create Window Error");
    }

    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(0);

    // Init renderer
    try
    {
        this->renderer.init(initSize);
    }
    catch(const std::runtime_error& err)
    {
        std::cerr << "Renderer Init Error: " << err.what() << std::endl;
        throw err;
    }
    
    this->renderer.environment.loadFromImage(initEnvironment);
    this->renderer.loadScene(this->scene, this->maxTextureArraySize);

    // Init UI
    this->initUI();
}

void Application::shutdown()
{
    this->shutdownUI();
    this->renderer.shutdown();
    glfwDestroyWindow(this->threadContext);
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Application::run()
{
    while (!glfwWindowShouldClose(this->window))
    {
        if (this->rendering.needClear)
        {
            this->renderer.clear();
            this->tiling.count = 0;
            this->rendering.isPreview = !this->rendering.enable;
            this->rendering.needClear = false;
        }

        // Render
        if (this->rendering.isPreview)
        {
            if (this->renderer.getSampleCount() == 0)
            {
                unsigned int maxBounceCount = this->renderer.maxBounceCount;
                this->renderer.maxBounceCount = 0;
                this->renderer.render(1);
                this->renderer.maxBounceCount = maxBounceCount;
            }
        }
        else if (this->rendering.enable)
        {
            if (this->rendering.sampleCountTarget > 0 && this->rendering.sampleCountTarget <= this->renderer.getSampleCount())
            {
                this->rendering.enable = false;
            }
            else
            {
                glm::uvec2 pos, size;
                this->tiling.getTile(this->renderer.getSize(), pos, size);
                this->renderer.renderRect(this->rendering.samplesPerFrame, pos, size, this->tiling.isLastTick());
                this->tiling.tick();
            }
        }

        // Render UI
        glfwPollEvents();
        this->renderUI();
        glfwSwapBuffers(this->window);
    }

    this->shutdown();
}

void Application::loadScene(const std::filesystem::path& path)
{
    std::thread([&, path]()
    {
        this->isSceneLoaded = false;
        glfwMakeContextCurrent(this->threadContext);

        try
        {
            this->scene = loadGLTF(path);
        }
        catch (const std::runtime_error& err)
        {
            glfwMakeContextCurrent(nullptr);
            this->isSceneLoaded = true;

            std::cerr << err.what() << std::endl;
            tinyfd_messageBox("Error", "Invalid scene file", "ok", "warning", 0);
            return;
        }

        this->materialTextureView.textureId = -1;
        if (this->property.type == Application::Property::Type::MeshInstance ||
            this->property.type == Application::Property::Type::Material)
        {
            this->property.type = Application::Property::Type::Contorls;
        }

        this->renderer.loadScene(this->scene, this->maxTextureArraySize);
        this->clear();
        glfwMakeContextCurrent(nullptr);
        this->isSceneLoaded = true;
    }).detach();
}

void Application::control()
{
    if (ImGui::IsKeyPressed(ImGuiKey_Space, false))
    {
        this->switchRendering();
    }

#if !TX_SPIRV
    if (ImGui::IsKeyPressed(ImGuiKey_R, false))
    {
        this->reloadShaders();
    }
#endif

    if (this->renderTextureView.isHover && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        this->renderTextureView.control();
    }

    switch (this->cameraControl.mode)
    {
        case Application::CameraControl::Mode::Free:
            if (this->cameraControl.enableFree && this->cameraControl.controlFree(this->renderer.camera))
            {
                this->clear();
                this->renderTextureView.reset();
            }

            if (ImGui::IsKeyPressed(ImGuiKey_C, false))
            {
                this->cameraControl.enableFree = !this->cameraControl.enableFree;
                glfwSetInputMode(window, GLFW_CURSOR, this->cameraControl.enableFree ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                this->renderTextureView.reset();
            }

            break;
        case Application::CameraControl::Mode::Orbit:
            if (this->renderTextureView.isHover && !ImGuizmo::IsOver() && !ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && this->cameraControl.controlOrbit(this->renderer.camera))
            {
                this->clear();
                this->renderTextureView.reset();
            }

            break;
    }
}

float Application::getLookAtDistance() const
{
    Image image = this->renderer.getDepthImage();
    size_t centerIndex = image.size.x / 2 * (1 + image.size.y) * 4;
    float nonLinear = image.pixels[centerIndex] * 2 - 1;

    float min = this->renderer.minRenderDistance;
    float max = this->renderer.maxRenderDistance;
    float linear = 2 * min * max / (max + min - nonLinear * (max - min));
    return linear;
}

void Application::setCameraMode(CameraControl::Mode mode)
{
    this->cameraControl.mode = mode;
    this->cameraControl.enableFree = false;
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    this->renderTextureView.reset();
}

void Application::switchRendering()
{
    this->rendering.enable = !this->rendering.enable;
    if (this->rendering.enable)
    {
        if (this->rendering.isPreview)
        {
            this->renderer.clear();
            this->tiling.count = 0;
        }

        this->rendering.isPreview = false;
    }
}

#if !TX_SPIRV
void Application::reloadShaders()
{
    try
    {
        this->renderer.reloadShaders();
        this->clear();
    }
    catch(const std::runtime_error& err)
    {
        std::cerr << err.what() << std::endl;
    }
}
#endif

void Application::clear()
{
    this->rendering.needClear = true;
}

glm::vec2 toVec2(const ImVec2 v)
{
    return glm::vec2(v.x, v.y);
}

ImVec2 toImVec2(const glm::vec2 v)
{
    return ImVec2(v.x, v.y);
}
