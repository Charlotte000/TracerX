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
    enum class PropertyOption
    {
        PContorls,
        PToneMapping,
        PSettings,
        PCamera,
        PEnvironment,
        PMesh,
        PMaterial,
    } property = PropertyOption::PContorls;
    void* edit = nullptr;
    int currentTextureId = -1;
    Application* app = nullptr;
    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
    TracerX::core::Texture textureView;

    void barMenu();
    void mainWindowMenu();
    void drawingPanelMenu();
    void sidePanelMenu();
    void propertySelectorMenu();
    void propertyMenu();
    void propertyMeshMenu(TracerX::Mesh& mesh);
    void propertyMaterialMenu(TracerX::Material& material);
    void propertyCameraMenu();
    void propertyEnvironmentMenu();
    void propertyControlsMenu();
    void propertyToneMappingMenu();
    void propertySettingsMenu();
    bool materialTextureSelector(const std::string& name, int& currentTextureId, glm::vec3 tintColor);
    void viewTexture(GLint textureHandler);
};
