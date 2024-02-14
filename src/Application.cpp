#include "TracerX/Quad.h"
#include "TracerX/Application.h"

#include <iomanip>
#include <tinydir.h>
#include <glm/gtx/rotate_vector.hpp>


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

void Application::init(glm::ivec2 size)
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
    this->size = size;
    this->window = glfwCreateWindow(size.x, size.y, "TracerX", nullptr, nullptr);
    if (this->window == nullptr)
    {
        throw std::runtime_error("GLFW Create Window Error");
    }

    glfwMakeContextCurrent(this->window);
    glfwSetWindowUserPointer(this->window, this);
    
    // Key listener
    glfwSetKeyCallback(this->window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GL_TRUE);
                    break;
                case GLFW_KEY_C:
                    glfwSetInputMode(window, GLFW_CURSOR, glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED ? GLFW_CURSOR_DISABLED :  GLFW_CURSOR_NORMAL);
                    break;
            }
        }
    });

    // Resize listener
    glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow* window, int width, int height)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->resize(glm::ivec2(width, height));
    });

    // Init components
    if (this->scene.environment.name == "None" && this->environmentFiles.size() > 0)
    {
        this->scene.loadEnvironmentMap(Application::environmentFolder + this->environmentFiles[0]);
    }

    this->renderer.init(this->size, this->scene);
    this->ui.init(this);
    this->viewer.init("../shaders/vertex/main.glsl", "../shaders/fragment/viewer/main.glsl");
}

void Application::resize(glm::ivec2 size)
{
    this->size = size;
    glfwSetWindowSize(this->window, this->size.x, this->size.y);
    this->renderer.resize(this->size);
}

void Application::shutdown()
{
    this->viewer.shutdown();
    this->ui.shutdown();
    this->renderer.shutdown();
    glfwTerminate();
}

void Application::run()
{
    while (!glfwWindowShouldClose(this->window))
    {
        glfwPollEvents();

        this->control();

        // Render
        this->renderer.render();

        // Draw
        this->viewer.use();
        glViewport(0, 0, this->size.x, this->size.y);
        Quad::draw();
        glBindTexture(GL_TEXTURE_2D, 0);
        Shader::stopUse();

        // UI
        this->ui.render();

        // Display
        glfwSwapBuffers(this->window);
    }

    this->shutdown();
}

void Application::save()
{
    auto t = std::time(nullptr);
    std::stringstream name;
    name << "img" << this->renderer.frameCount << '(' << std::put_time(std::localtime(&t), "%Y%m%dT%H%M%S") << ')' << ".png";

    int w = this->renderer.output.colorTexture.width;
    int h = this->renderer.output.colorTexture.height;
    unsigned char* pixels = new unsigned char[w * h * 3];
    this->renderer.output.colorTexture.upload(pixels);
    Image::saveToDisk(name.str(), w, h, pixels);
    delete[] pixels;
}

void Application::control()
{
    static const float RotateSpeed = 3;
    static const float MouseSensitivity = 100;

    static double lastTime = 0;
    static glm::vec2 mousePosition;

    double newX, newY;
    glfwGetCursorPos(this->window, &newX, &newY);
    glm::vec2 newMousePos(newX, newY);
    glm::vec2 mouseDelta = (newMousePos - mousePosition) / MouseSensitivity;
    mousePosition = newMousePos;

    double nextTime = glfwGetTime();
    float elapsedTime = (float)(nextTime - lastTime);
    lastTime = nextTime;

    if (glfwGetInputMode(this->window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return;
    }

    float elapsedMove = this->cameraSpeed * elapsedTime;
    float elapsedRotate = RotateSpeed * elapsedTime;

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

    this->renderer.resetAccumulator();
}
