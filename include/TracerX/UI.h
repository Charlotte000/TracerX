#pragma once

#include "Scene.h"
#include "Image.h"
#include "Camera.h"
#include "Texture.h"

#include <string>
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>


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
    Texture textureView;

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
    void materialTextureSelector(const std::string& name, float& currentTextureId);
};
