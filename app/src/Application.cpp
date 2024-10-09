#include "Application.h"

#include <TracerX/GLTFLoader.h>

#include <stdexcept>
#include <glm/gtx/rotate_vector.hpp>

using namespace TracerX;

#pragma region Application Helper Functions
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

void Application::RenderTextureView::getUVRect(glm::vec2& lo, glm::vec2& up) const
{
    lo = this->uvCenter - this->uvSize * .5f;
    up = this->uvCenter + this->uvSize * .5f;
}

void Application::RenderTextureView::getRectFromUV(glm::vec2& lo, glm::vec2& up) const
{
    lo = lo * this->size + this->pos;
    up = up * this->size + this->pos;
}

void Application::Tiling::tick()
{
    this->count = (this->count + 1) % (this->factor * this->factor);
}

bool Application::Tiling::isLastTick() const
{
    return this->count == this->factor * this->factor - 1;
}

void Application::Tiling::getTile(glm::uvec2 canvasSize, glm::uvec2& pos, glm::uvec2& size)
{
    this->factor = glm::clamp(this->factor, 1u, std::min(canvasSize.x, canvasSize.y));
    size = (glm::uvec2)glm::ceil((glm::vec2)canvasSize / glm::vec2(this->factor));
    pos = glm::uvec2(this->count % this->factor, this->count / this->factor) * size;
}
#pragma endregion

void Application::init(glm::uvec2 size)
{
    // Init GLFW
    glfwSetErrorCallback([](int, const char* err)
    {
        throw std::runtime_error(err);
    });

    if (glfwInit() == GL_FALSE)
    {
        throw std::runtime_error("GLFW Init Error");
    }

    // Create window
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    this->window = glfwCreateWindow(size.x, size.y, "TracerX", nullptr, nullptr);
    if (this->window == nullptr)
    {
        throw std::runtime_error("GLFW Create Window Error");
    }

    glfwMakeContextCurrent(this->window);
    glfwSetWindowUserPointer(this->window, this);
    glfwSwapInterval(0);

    // Key listener
    glfwSetKeyCallback(this->window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_C:
                    app->cameraControl.enable = !app->cameraControl.enable;
                    glfwSetInputMode(window, GLFW_CURSOR, app->cameraControl.enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                    app->renderTextureView.reset();
                    break;
                case GLFW_KEY_SPACE:
                    app->rendering.enable = !app->rendering.enable;
                    if (app->rendering.enable)
                    {
                        if (app->rendering.isPreview)
                        {
                            app->renderer.clear();
                            app->tiling.count = 0;
                        }

                        app->rendering.isPreview = false;
                    }

                    break;
            }
        }
    });

    glfwSetScrollCallback(this->window, [](GLFWwindow* window, double xOffset, double yOffset)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (app->renderTextureView.isHover && !app->cameraControl.enable)
        {
            if (yOffset > 0)
            {
                app->renderTextureView.uvSize *= .9f;
                app->renderTextureView.clamp();
            }
            else if (yOffset < 0)
            {
                app->renderTextureView.uvSize *= 1.1f;
                app->renderTextureView.clamp();
            }
        }
    });

    // Init components
    this->renderer.init(size);
    this->initUI();
    this->loadAnyEnvironment();

    this->renderer.loadScene(this->scene);
}

void Application::shutdown()
{
    this->shutdownUI();
    this->renderer.shutdown();
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

        glfwPollEvents();

        this->control();

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

        // UI
        this->renderUI();

        // Display
        glfwSwapBuffers(this->window);
    }

    this->shutdown();
}

void Application::loadScene(const std::filesystem::path& path)
{
    this->scene = loadGLTF(path);
    this->clear();
    this->renderer.loadScene(this->scene);
}

void Application::loadAnyEnvironment()
{
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(Application::environmentFolder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::filesystem::path path = entry.path();
        std::filesystem::path ext = path.extension();
        if (ext == ".hdr" || ext == ".png" || ext == ".jpg")
        {
            this->renderer.environment.loadFromFile(path);
            return;
        }
    }
}

void Application::control()
{
    ImGuiIO io = ImGui::GetIO();
    float elapsedTime = io.DeltaTime;
    glm::vec2 mouseDelta = toVec2(io.MouseDelta);

    if (this->renderTextureView.isHover && !this->cameraControl.enable && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        this->renderTextureView.uvCenter -= mouseDelta * this->renderTextureView.uvSize / this->renderTextureView.size;
        this->renderTextureView.clamp();
    }

    if (!this->cameraControl.enable)
    {
        return;
    }

    bool updated = false;
    Camera& camera = this->renderer.camera;

    // Keyboard
    float elapsedMove = this->cameraControl.speed * elapsedTime;
    glm::vec3 right = glm::cross(camera.forward, camera.up);
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.position += camera.forward * elapsedMove;
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.position += -camera.forward * elapsedMove;
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.position += -right * elapsedMove;
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.position += right * elapsedMove;
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.position += camera.up * elapsedMove;
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.position += -camera.up * elapsedMove;
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.up = glm::rotate(camera.up, -this->cameraControl.rotationSpeed * 2 * elapsedTime, camera.forward);
        updated = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.up = glm::rotate(camera.up, this->cameraControl.rotationSpeed * 2 * elapsedTime, camera.forward);
        updated = true;
    }

    // Mouse
    if (mouseDelta != glm::vec2(0))
    {
        mouseDelta *= this->cameraControl.rotationSpeed / 200.f;
        camera.forward = glm::rotate(camera.forward, -mouseDelta.y, right);
        camera.up = glm::rotate(camera.up, -mouseDelta.y, right);
        camera.forward = glm::rotate(camera.forward, -mouseDelta.x, camera.up);
        updated = true;
    }

    if (updated)
    {
        this->clear();
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
