#include <TracerX/Renderer.h>
#include <TracerX/GLTFLoader.h>

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

GLFWwindow* createWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    return window;
}

void renderAndSave(TracerX::Renderer& renderer, unsigned int samples, const std::filesystem::path& outputFile)
{
    renderer.clear();

    std::cout << "Rendering " << outputFile.filename() << std::endl;
    renderer.render(samples);

#if TX_DENOISE
    std::cout << "Denoising " << outputFile.filename() << std::endl;
    renderer.denoise();
#endif

    std::cout << "Saving " << outputFile.filename() << std::endl << std::endl;
    renderer.toneMapTexture.upload().saveToFile(outputFile);
}

int main(int argc, char* argv[])
{
    const std::filesystem::path homeDir = std::filesystem::path(argv[0]).parent_path() / ".." / "..";
    const std::filesystem::path environmentDir = homeDir / "app" / "assets" / "environments";
    const std::filesystem::path sceneDir = homeDir / "app" / "assets" / "scenes";

    std::cout << "Setting up" << std::endl << std::endl;
    GLFWwindow* window = createWindow();

    // Loading the scene
    TracerX::Scene scene = TracerX::loadGLTF(sceneDir / "Ajax.glb");

    // Move the mesh
    TracerX::MeshInstance& ajaxMesh = scene.meshInstances[0];
    ajaxMesh.transform = glm::translate(ajaxMesh.transform, glm::vec3(0, -.5f, 0));

    // Setting up the renderer
    TracerX::Renderer renderer;
    renderer.init(glm::uvec2(1000, 1000));
    renderer.environment.update(TracerX::Image(environmentDir / "konzerthaus_2k.hdr"));
    renderer.loadScene(scene);

    // Setting up the camera
    renderer.camera.fov = glm::radians(45.f);
    renderer.camera.position = glm::normalize(glm::vec3(.5f, 0, 1)) * 1.5f;
    renderer.camera.forward = -glm::normalize(renderer.camera.position);
    renderer.camera.blur = 0.001f;

    renderAndSave(renderer, 100, "ajaxWhite.png");

    // Setting up the mesh material
    TracerX::Material& ajaxMaterial = scene.materials[ajaxMesh.materialId];
    ajaxMaterial.albedoColor = glm::vec4(.3f, .3f, .001f, 1);
    ajaxMaterial.metalness = 1;
    ajaxMaterial.roughness = .1f;
    renderer.updateSceneMaterials(scene);

    renderAndSave(renderer, 100, "ajaxGold.png");

    // Shutting down
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
