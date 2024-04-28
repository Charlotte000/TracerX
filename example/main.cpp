#include <TracerX/Scene.h>
#include <TracerX/Renderer.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace TracerX;
using namespace TracerX::core;

GLFWwindow* createWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    return window;
}

int main()
{
    GLFWwindow* window = createWindow();

    // Loading the scene
    Scene scene = Scene::loadGLTF("../../app/assets/scenes/Ajax.glb");
    scene.loadEnvironmentMap("../../app/assets/environments/konzerthaus_4k.hdr");

    // Move the mesh
    Mesh& ajax = scene.meshes[0];
    ajax.transform = glm::translate(scene.meshes[0].transform, glm::vec3(0, -.5f, 0));

    // Setting up the renderer
    Renderer renderer;
    renderer.init();
    renderer.resize(glm::ivec2(1000, 1000));
    renderer.resetScene(scene);

    // Setting up the camera
    renderer.camera.fov = glm::radians(45.f);
    renderer.camera.position = glm::normalize(glm::vec3(-.5f, 0, -1)) * 1.5f;
    renderer.camera.forward = -glm::normalize(renderer.camera.position);
    renderer.camera.blur = 0.001f;

    // Render
    for (size_t i = 0; i < 100; i++)
    {
        renderer.render();
    }

#ifdef TX_DENOISE
    renderer.denoise();
#endif
    renderer.output.colorTexture.upload().saveToFile("ajaxWhite.png");

    // Setting up the mesh material
    Material& material = scene.materials[ajax.materialId];
    material.albedoColor = glm::vec3(.3f, .3f, .001f);
    material.metalness = 1;
    material.roughness = .1f;
    renderer.resetMaterials(scene.materials);

    // Render
    renderer.resetAccumulator();
    for (size_t i = 0; i < 100; i++)
    {
        renderer.render();
    }

#ifdef TX_DENOISE
    renderer.denoise();
#endif
    renderer.output.colorTexture.upload().saveToFile("ajaxGold.png");

    // Shutting down
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
