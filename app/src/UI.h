#pragma once

#include <TracerX/Mesh.h>
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
    TracerX::Mesh* editMesh = nullptr;
    TracerX::Material* editMaterial = nullptr;
    int currentTextureId = -1;
    bool editCamera = false;
    bool editEnvironment = false;
    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
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
    bool materialTextureSelector(const std::string& name, int& currentTextureId, glm::vec3 tintColor);
    void viewTexture(GLint textureHandler);
};
