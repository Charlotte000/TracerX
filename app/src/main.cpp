#include "Application.h"

#include <TracerX/GLTFLoader.h>

int main()
{
    // Initialize the application with the following parameters.
    // Feel free to change them.
    // =======================================

    const std::filesystem::path homeDir = std::filesystem::canonical(TX_HOME);

    // The directory containing the scenes.
    Application::sceneDir = homeDir / "app" / "assets" / "scenes" / "";

    // The directory containing the environment maps.
    Application::environmentDir = homeDir / "app" / "assets" / "environments" / "";

#if !TX_SPIRV
    // The path to the shader source code.
    Application::shaderPath = homeDir / "shaders" / "main.comp";
#endif

    // The initial size of the rendered image.
    const glm::uvec2 initSize(900, 900);

    // The initial scene to render.
    const TracerX::Scene initScene = TracerX::loadGLTF(Application::sceneDir / "Box.glb");

    // The initial environment to render.
    const TracerX::Image initEnvironment = TracerX::Image::loadFromFile(Application::environmentDir / "konzerthaus_2k.hdr");

    // The maximum size of the textures used in the scene.
    // For low-end devices. See Renderer::loadScene.
    const glm::uvec2 maxTextureArraySize(-1);
    // =======================================

    // Create the application and run it.
    Application app(initSize, initScene, initEnvironment, maxTextureArraySize);
    app.run();
}
