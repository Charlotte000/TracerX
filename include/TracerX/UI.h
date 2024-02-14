#pragma once

#include "Scene.h"


class Application;

class UI
{
public:
    Application* app = nullptr;

    void init(Application* app);
    void shutdown();
    void render();
private:
    Mesh* editMesh = nullptr;
    glm::mat4 editMeshTransform;

    void mainMenu();
    void info();
    void camera();
    void materials();
    void meshes();
    void environment();
    void scene();
    void transform();
};
