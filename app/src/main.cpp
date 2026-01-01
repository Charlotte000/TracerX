#include <OGL/Image2D.h>

#include <TracerX/GLTFLoader.h>

#include "Application.h"

int main(int argc, char* argv[])
{
    // Initialize the application with the following parameters.
    // Feel free to change them.
    // =======================================

    const std::filesystem::path homeDir = std::filesystem::path(argv[0]).parent_path() / ".." / "..";

    // The initial size of the rendered image.
    const glm::uvec2 initSize(900, 900);

    // The maximum size of the textures used in the scene.
    // For low-end devices. See Renderer::loadScene.
    const glm::uvec2 maxTextureArraySize(-1);

    // The directory containing the scenes.
    const std::filesystem::path sceneDir = homeDir / "app" / "assets" / "scenes";

    // The directory containing the environment maps.
    const std::filesystem::path environmentDir = homeDir / "app" / "assets" / "environments";

#if !NDEBUG
    // The path to the shader source code.
    const std::filesystem::path shaderPath = homeDir / "tracerX" / "shaders";
#endif

    // The initial scene to render.
    const TracerX::Scene initScene = TracerX::loadGLTF(sceneDir / "Box.glb");

    // The initial environment to render.
    const OGL::Image2D initEnvironment(environmentDir / "konzerthaus_2k.hdr");
    // =======================================

    // Create the application and run it.
    GLFWwindow* window = Application::initGLFW();
    Application app(
        window,
        initSize,
        maxTextureArraySize,
        sceneDir,
        environmentDir,
#if !NDEBUG
        shaderPath,
#endif
        initScene,
        initEnvironment);
    app.run();
}
