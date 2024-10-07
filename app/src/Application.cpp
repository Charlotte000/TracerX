#include "Application.h"

#include <TracerX/GLTFLoader.h>

#include <stdexcept>
#include <glm/gtx/rotate_vector.hpp>

using namespace TracerX;

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
                    if (app->enableCameraControl || !app->enableRendering)
                    {
                        app->enableCameraControl = !app->enableCameraControl;
                        glfwSetInputMode(window, GLFW_CURSOR, app->enableCameraControl ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                        app->viewUVCenter = glm::vec2(.5f);
                        app->viewUVSize = glm::vec2(1);
                    }

                    break;
                case GLFW_KEY_SPACE:
                    app->enableRendering = !app->enableRendering;
                    if (app->enableRendering)
                    {
                        if (app->enablePreview && app->renderer.getSampleCount() == 1)
                        {
                            app->renderer.clear();
                        }

                        app->enableCameraControl = false;
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    }

                    break;
            }
        }
    });

    glfwSetScrollCallback(this->window, [](GLFWwindow* window, double xOffset, double yOffset)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (app->viewActive && !app->enableCameraControl)
        {
            if (yOffset > 0)
            {
                app->viewUVSize *= .9f;
                app->viewUVSize = glm::max(glm::vec2(.0001f), app->viewUVSize);
                app->viewUVCenter = glm::clamp(app->viewUVCenter, app->viewUVSize * .5f, 1.f - app->viewUVSize * .5f);
            }
            else if (yOffset < 0)
            {
                app->viewUVSize *= 1.1f;
                app->viewUVSize = glm::min(glm::vec2(1), app->viewUVSize);
                app->viewUVCenter = glm::clamp(app->viewUVCenter, app->viewUVSize * .5f, 1.f - app->viewUVSize * .5f);
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
        glfwPollEvents();

        this->control();

        // Render
        if (this->enablePreview && this->renderer.getSampleCount() == 0)
        {
            unsigned int maxBounceCount = this->renderer.maxBounceCount;
            this->renderer.maxBounceCount = 0;
            this->renderer.render(1);
            this->renderer.maxBounceCount = maxBounceCount;
        }
        else if (this->enableRendering || this->renderer.getSampleCount() == 0)
        {
            this->renderer.render(this->samplesPerFrame);
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
    this->renderer.clear();
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

    if (this->viewActive && !this->enableCameraControl && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        this->viewUVCenter -= mouseDelta * this->viewUVSize / this->viewSize;
        this->viewUVCenter = glm::clamp(this->viewUVCenter, this->viewUVSize * .5f, 1.f - this->viewUVSize * .5f);
    }

    if (!this->enableCameraControl)
    {
        return;
    }

    float elapsedMove = this->cameraSpeed * elapsedTime;
    float elapsedRotate = this->cameraRotationSpeed * 3 * elapsedTime;

    Camera& camera = this->renderer.camera;
    glm::vec3 right = glm::cross(camera.forward, camera.up);

    // Keyboard
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.position += camera.forward * elapsedMove;
    }

    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.position += -camera.forward * elapsedMove;
    }

    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.position += -right * elapsedMove;
    }

    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.position += right * elapsedMove;
    }

    if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.position += camera.up * elapsedMove;
    }

    if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.position += -camera.up * elapsedMove;
    }

    if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.up = glm::rotate(camera.up, -elapsedRotate, camera.forward);
    }

    if (glfwGetKey(this->window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.up = glm::rotate(camera.up, elapsedRotate, camera.forward);
    }

    // Mouse
    mouseDelta *= this->cameraRotationSpeed / 100.f;
    camera.forward = glm::rotate(camera.forward, -mouseDelta.y, right);
    camera.up = glm::rotate(camera.up, -mouseDelta.y, right);
    camera.forward = glm::rotate(camera.forward, -mouseDelta.x, camera.up);

    this->renderer.clear();
}

float Application::getLookAtDistance()
{
    Image image = this->renderer.getDepthImage();
    size_t centerIndex = image.size.x / 2 * (1 + image.size.y) * 4;
    float nonLinear = image.pixels[centerIndex] * 2 - 1;

    float min = this->renderer.minRenderDistance;
    float max = this->renderer.maxRenderDistance;
    float linear = 2 * min * max / (max + min - nonLinear * (max - min));
    return linear;
}

glm::vec2 toVec2(const ImVec2 v)
{
    return glm::vec2(v.x, v.y);
}

ImVec2 toImVec2(const glm::vec2 v)
{
    return ImVec2(v.x, v.y);
}
