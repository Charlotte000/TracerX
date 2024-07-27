#include "Application.h"

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
    this->window = glfwCreateWindow(size.x, size.y, "TracerX", nullptr, nullptr);
    if (this->window == nullptr)
    {
        throw std::runtime_error("GLFW Create Window Error");
    }

    glfwMakeContextCurrent(this->window);
    glfwSetWindowUserPointer(this->window, this);
    glfwMaximizeWindow(this->window);
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
                    glfwSetInputMode(window, GLFW_CURSOR, glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                    break;
                case GLFW_KEY_SPACE:
                    app->enableRendering = !app->enableRendering;
                    break;
            }
        }
    });

    // Init components
    this->renderer.init(size);
    this->ui.init(this);
    this->loadAnyEnvironment();

    this->renderer.loadScene(this->scene);
}

void Application::shutdown()
{
    this->ui.shutdown();
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
        if (this->enableRendering || this->renderer.getSampleCount() == 0)
        {
            if (this->enablePreview)
            {
                unsigned int maxBounceCount = this->renderer.maxBounceCount;
                this->renderer.maxBounceCount = this->enableRendering ? maxBounceCount : 0;
                this->renderer.render(this->enableRendering ? this->samplesPerFrame : 1);
                this->renderer.maxBounceCount = maxBounceCount;
            }
            else
            {
                this->renderer.render(this->samplesPerFrame);
            }
        }

        // UI
        this->ui.render();

        // Display
        glfwSwapBuffers(this->window);
    }

    this->shutdown();
}

void Application::loadScene(const std::string& fileName)
{
    this->scene = Scene::loadGLTF(fileName);
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
            this->renderer.environment.loadFromFile(path.string());
            return;
        }
    }
}

void Application::control()
{
    static double lastTime = 0;
    static glm::vec2 mousePosition;

    double newX, newY;
    glfwGetCursorPos(this->window, &newX, &newY);
    glm::vec2 newMousePos(newX, newY);
    glm::vec2 mouseDelta = (newMousePos - mousePosition) * this->cameraRotationSpeed / 100.f;
    mousePosition = newMousePos;

    double nextTime = glfwGetTime();
    float elapsedTime = (float)(nextTime - lastTime);
    lastTime = nextTime;

    if (glfwGetInputMode(this->window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
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
    camera.forward = glm::rotate(camera.forward, -mouseDelta.y, right);
    camera.up = glm::rotate(camera.up, -mouseDelta.y, right);
    camera.forward = glm::rotate(camera.forward, -mouseDelta.x, camera.up);

    this->renderer.clear();
}
