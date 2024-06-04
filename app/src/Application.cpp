#include "Application.h"

#include <iomanip>
#include <tinydir.h>
#include <glm/gtx/rotate_vector.hpp>

using namespace TracerX;

Application::Application()
{
    tinydir_dir envDir;
    tinydir_open_sorted(&envDir, Application::environmentFolder.c_str());
    for (size_t i = 2; i < envDir.n_files; i++)
    {
        tinydir_file envFile;
        tinydir_readfile_n(&envDir, &envFile, i);
        this->environmentFiles.push_back(envFile.name);
    }

    tinydir_dir sceneDir;
    tinydir_open_sorted(&sceneDir, Application::sceneFolder.c_str());
    for (size_t i = 2; i < sceneDir.n_files; i++)
    {
        tinydir_file sceneFile;
        tinydir_readfile_n(&sceneDir, &sceneFile, i);
        if (std::strcmp(sceneFile.extension, "glb") == 0 || std::strcmp(sceneFile.extension, "gltf") == 0)
        {
            this->sceneFiles.push_back(sceneFile.name);
        }
    }
}

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
                    app->isRendering = !app->isRendering;
                    break;
            }
        }
    });

    // Init components
    this->renderer.init(size);
    this->ui.init(this);
    if (this->renderer.environment.name == "None" && !this->environmentFiles.empty())
    {
        this->renderer.environment.loadFromFile(Application::environmentFolder + this->environmentFiles[0]);
    }

    this->renderer.loadScene(this->scene, true);
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
        if (this->isRendering || this->renderer.getFrameCount() == 0)
        {
            this->renderer.render(this->perFrameCount);
        }

        // UI
        this->ui.render();

        // Display
        glfwSwapBuffers(this->window);
    }

    this->shutdown();
}

void Application::save() const
{
    auto t = std::time(nullptr);
    std::stringstream name;
    name << "img" << this->renderer.getFrameCount() << '(' << std::put_time(std::localtime(&t), "%Y%m%dT%H%M%S") << ')' << ".png";
    this->renderer.getImage().saveToFile(name.str());
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
    float elapsedRotate = this->cameraRotationSpeed * 3.f * elapsedTime;

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
