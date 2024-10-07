#include <TracerX/Scene.h>
#include <TracerX/Renderer.h>
#include <TracerX/GLTFLoader.h>

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
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
    cout << "Setting up" << endl << endl;
    GLFWwindow* window = createWindow();

    // Loading the scene
    Scene scene = loadGLTF("../../app/assets/scenes/Ajax.glb");

    // Move the mesh
    MeshInstance& ajaxMesh = scene.meshInstances[0];
    ajaxMesh.transform = glm::translate(ajaxMesh.transform, glm::vec3(0, -.5f, 0));

    // Setting up the renderer
    Renderer renderer;
    renderer.init(glm::uvec2(1000, 1000));
    renderer.environment.loadFromFile("../../app/assets/environments/konzerthaus_2k.hdr");
    renderer.loadScene(scene);

    // Setting up the camera
    renderer.camera.fov = glm::radians(45.f);
    renderer.camera.position = glm::normalize(glm::vec3(.5f, 0, 1)) * 1.5f;
    renderer.camera.forward = -glm::normalize(renderer.camera.position);
    renderer.camera.blur = 0.001f;

    // Render
    cout << "Ajax white" << endl;
    renderer.render(100);

#ifdef TX_DENOISE
    cout << "Denoising" << endl;
    renderer.denoise();
#endif

    cout << "Saving" << endl << endl;
    renderer.getImage().saveToFile("ajaxWhite.png");

    // Setting up the mesh material
    Material& axajMaterial = scene.materials[ajaxMesh.materialId];
    axajMaterial.albedoColor = glm::vec3(.3f, .3f, .001f);
    axajMaterial.metalness = 1;
    axajMaterial.roughness = .1f;
    renderer.updateSceneMaterials(scene);

    // Render
    renderer.clear();
    cout << "Ajax gold" << endl;
    renderer.render(100);

#ifdef TX_DENOISE
    cout << "Denoising" << endl;
    renderer.denoise();
#endif

    cout << "Saving" << endl;
    renderer.getImage().saveToFile("ajaxGold.png");

    // Shutting down
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
