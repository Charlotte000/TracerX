#include "Application.h"

#include <TracerX/GLTFLoader.h>

int main()
{
    // Initialize the application with the following parameters.
    // Feel free to change them.
    // =======================================

    const std::filesystem::path homeDir = std::filesystem::canonical(TX_HOME);

    // The initial size of the rendered image.
    const glm::uvec2 initSize(900, 900);

    // The maximum size of the textures used in the scene.
    // For low-end devices. See Renderer::loadScene.
    const glm::uvec2 maxTextureArraySize(-1);

    // The directory containing the scenes.
    const std::filesystem::path sceneDir = homeDir / "app" / "assets" / "scenes" / "";

    // The directory containing the environment maps.
    const std::filesystem::path environmentDir = homeDir / "app" / "assets" / "environments" / "";

#if !TX_SPIRV
    // The path to the shader source code.
    const std::filesystem::path shaderPath = homeDir / "shaders" / "main.comp";
#endif

    // The initial scene to render.
    const TracerX::Scene initScene = TracerX::loadGLTF(sceneDir / "Box.glb");

    // The initial environment to render.
    const TracerX::Image initEnvironment = TracerX::Image::loadFromFile(environmentDir / "konzerthaus_2k.hdr");
    // =======================================

    // Create the application and run it.
    Application app(
        initSize,
        maxTextureArraySize,
        sceneDir,
        environmentDir,
#if !TX_SPIRV
        shaderPath,
#endif
        initScene,
        initEnvironment);
    app.run();
}
