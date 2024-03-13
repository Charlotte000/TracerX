#pragma once

#include "Scene.h"
#include "Image.h"
#include "Camera.h"

#include <imgui.h>
#include <ImGuizmo.h>


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
    Material* editMaterial = nullptr;
    Camera* editCamera = nullptr;
    Image* editEnvironment = nullptr;
    glm::mat4 editMeshTransform;
    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
    bool autoApply = false;

    void barMenu();
    void mainWindowMenu();
    void drawingPanelMenu();
    void sidePanelMenu();
    void sceneMenu();
    void propertyMenu();
    void propertyMeshMenu();
    void propertyMaterialMenu();
    void propertyCameraMenu();
    void propertyEnvironmentMenu();
    void propertySceneMenu();
};
