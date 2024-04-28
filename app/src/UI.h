#pragma once

#include <TracerX/Mesh.h>
#include <TracerX/Scene.h>
#include <TracerX/Image.h>
#include <TracerX/Camera.h>
#include <TracerX/Texture.h>
#include <TracerX/Material.h>

#include <string>
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>

class Application;

class UI
{
public:
    void init(Application* app);
    void shutdown();
    void render();
private:
    Application* app = nullptr;
    TracerX::core::Mesh* editMesh = nullptr;
    TracerX::core::Material* editMaterial = nullptr;
    bool editCamera = false;
    bool editEnvironment = false;
    glm::mat4 editMeshTransform;
    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
    bool autoApply = false;
    TracerX::core::Texture textureView;

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
