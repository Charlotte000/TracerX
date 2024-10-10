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

int main()
{
    std::cout << "Setting up" << std::endl << std::endl;
    GLFWwindow* window = createWindow();

    // Loading the scene
    TracerX::Scene scene = TracerX::loadGLTF("../../app/assets/scenes/Ajax.glb");

    // Move the mesh
    TracerX::MeshInstance& ajaxMesh = scene.meshInstances[0];
    ajaxMesh.transform = glm::translate(ajaxMesh.transform, glm::vec3(0, -.5f, 0));

    // Setting up the renderer
    TracerX::Renderer renderer;
    renderer.init(glm::uvec2(1000, 1000));
    renderer.environment.loadFromFile("../../app/assets/environments/konzerthaus_2k.hdr");
    renderer.loadScene(scene);

    // Setting up the camera
    renderer.camera.fov = glm::radians(45.f);
    renderer.camera.position = glm::normalize(glm::vec3(.5f, 0, 1)) * 1.5f;
    renderer.camera.forward = -glm::normalize(renderer.camera.position);
    renderer.camera.blur = 0.001f;

    // Render
    std::cout << "Ajax white" << std::endl;
    renderer.render(100);

#ifdef TX_DENOISE
    std::cout << "Denoising" << std::endl;
    renderer.denoise();
#endif

    std::cout << "Saving" << std::endl << std::endl;
    renderer.getImage().saveToFile("ajaxWhite.png");

    // Setting up the mesh material
    TracerX::Material& axajMaterial = scene.materials[ajaxMesh.materialId];
    axajMaterial.albedoColor = glm::vec3(.3f, .3f, .001f);
    axajMaterial.metalness = 1;
    axajMaterial.roughness = .1f;
    renderer.updateSceneMaterials(scene);

    // Render
    renderer.clear();
    std::cout << "Ajax gold" << std::endl;
    renderer.render(100);

#ifdef TX_DENOISE
    std::cout << "Denoising" << std::endl;
    renderer.denoise();
#endif

    std::cout << "Saving" << std::endl;
    renderer.getImage().saveToFile("ajaxGold.png");

    // Shutting down
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
