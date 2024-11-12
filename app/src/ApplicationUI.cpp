#include "Application.h"

#include <iostream>
#include <stdexcept>
#include <tinyfiledialogs.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/polar_coordinates.hpp>

using namespace TracerX;

#pragma region UI Helper Functions
void SetupImGuiStyle()
{
    // Rounded Visual Studio style by RedNicStone from ImThemes
    // https://github.com/Patitotective/ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 4.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 4.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 2.5f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 11.0f;
    style.ScrollbarRounding = 2.5f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 3.5f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
}

// https://www.unknowncheats.me/forum/direct3d/601535-imgui-simple-spinner.html
void Im_Spinner(const char* label, float radius, float thickness, ImU32 color) {
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(radius * 2, radius * 2); 
 
    ImGui::Dummy(size);
 
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();
    DrawList->PathClear();
 
    int num_segments = 30;
    float start = fabsf(sinf(ImGui::GetTime() * 1.8f) * (num_segments - 5));
 
    float a_min = 3.14159265358979323846f * 2.0f * start / num_segments;
    float a_max = 3.14159265358979323846f * 2.0f * (num_segments - 3) / num_segments;
 
    
    ImVec2 centre = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
 
    for (int i = 0; i <= num_segments; i++) {
        float a = a_min + (i / (float)num_segments) * (a_max - a_min);
        DrawList->PathLineTo(ImVec2(centre.x + cosf(a + ImGui::GetTime() * 8) * radius, centre.y + sinf(a + ImGui::GetTime() * 8) * radius));
    }
 
    DrawList->PathStroke(color, false, thickness);
}

bool DragUInt(const char* label, unsigned int* v, float v_speed = 1.f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0)
{
    int vTemp = *v;
    bool changed = ImGui::DragInt(label, &vTemp, v_speed, v_min, v_max, format, flags);
    *v = vTemp;
    return changed;
}

bool DragUInt2(const char* label, unsigned int v[2], float v_speed = 1.f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0)
{
    int vTemp[2] { (int)v[0], (int)v[1] };
    bool changed = ImGui::DragInt2(label, vTemp, v_speed, v_min, v_max, format, flags);
    v[0] = (unsigned int)vTemp[0];
    v[1] = (unsigned int)vTemp[1];
    return changed;
}

void Tooltip(const std::string& content)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text("%s", content.c_str());
        ImGui::EndTooltip();
    }
}

void drawFillImage(GLint textureHandler, glm::vec2 srcSize, glm::vec2& imagePos, glm::vec2& imageSize, glm::vec3 tintColor = glm::vec3(1), glm::vec2 uvLo = glm::vec2(0), glm::vec2 uvUp = glm::vec2(1), bool flipY = false)
{
    if (flipY)
    {
        uvLo.y = 1 - uvLo.y;
        uvUp.y = 1 - uvUp.y;
    }

    float srcAspectRatio = srcSize.x / srcSize.y;

    glm::vec2 dstSize = toVec2(ImGui::GetContentRegionAvail());
    float dstAspectRatio = dstSize.x / dstSize.y;

    imageSize = dstSize * (srcAspectRatio > dstAspectRatio ? glm::vec2(1, dstAspectRatio / srcAspectRatio) : glm::vec2(srcAspectRatio / dstAspectRatio, 1));
    imagePos = (toVec2(ImGui::GetWindowSize()) - imageSize) * 0.5f;

    ImGui::SetCursorPos(toImVec2(imagePos));
    ImGui::Image(
        (void*)(intptr_t)textureHandler,
        toImVec2(imageSize - 2.f),
        toImVec2(uvLo),
        toImVec2(uvUp),
        ImVec4(tintColor.r, tintColor.g, tintColor.b, 1),
        ImGui::GetStyle().Colors[ImGuiCol_TableBorderLight]);

    imagePos += toVec2(ImGui::GetWindowPos());
}

bool materialTextureSelector(Application& app, const std::string& name, int& currentTextureId, glm::vec3 tintColor)
{
    bool changed = false;

    if (ImGui::BeginCombo(
        ("Texture##" + name).c_str(),
        currentTextureId == -1 ? "None" : app.scene.textureNames[currentTextureId].c_str()))
    {
        if (ImGui::Selectable("None", currentTextureId == -1))
        {
            currentTextureId = -1;
            changed = true;
        }

        for (size_t textureId = 0; textureId < app.scene.textures.size(); textureId++)
        {
            const Image& texture = app.scene.textures[textureId];
            const std::string& textureName = app.scene.textureNames[textureId];
            if (ImGui::Selectable(
                (textureName + "##" + name + "Texture" + std::to_string(textureId)).c_str(),
                currentTextureId == textureId))
            {
                currentTextureId = (int)textureId;
                changed = true;
            }
        }

        ImGui::EndCombo();
    }

    if (currentTextureId != -1)
    {
        if (app.materialTextureView.textureId != currentTextureId)
        {
            app.materialTextureView.texture.update(app.scene.textures[currentTextureId]);
            app.materialTextureView.textureId = currentTextureId;
        }

        glm::vec2 imagePos, imageSize;
        ImGui::BeginChild("viewMaterialTexture");
        drawFillImage(app.materialTextureView.texture.getHandler(), app.materialTextureView.texture.size, imagePos, imageSize, tintColor);
        ImGui::EndChild();
    }

    return changed;
}

void propertyMaterial(Application& app, Material& material)
{
    if (!ImGui::BeginTabBar("propertyMaterialMenu"))
    {
        return;
    }

    bool changed = false;

    if (ImGui::BeginTabItem("Albedo"))
    {
        glm::vec4 albedoColor = glm::vec4(glm::pow(glm::vec3(material.albedoColor), glm::vec3(1 / app.renderer.gamma)), material.albedoColor.a);
        changed |= ImGui::ColorEdit4("Color (gamma corrected)##albedo", glm::value_ptr(albedoColor), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
        material.albedoColor = glm::vec4(glm::pow(glm::vec3(albedoColor), glm::vec3(app.renderer.gamma)), albedoColor.a);

        changed |= materialTextureSelector(app, "albedo", material.albedoTextureId, material.albedoColor);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Roughness"))
    {
        changed |= ImGui::SliderFloat("Strength##roughness", &material.roughness, 0, 1);
        changed |= materialTextureSelector(app, "roughness", material.roughnessTextureId, glm::vec3(0, material.roughness, 0));
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Metalness"))
    {
        changed |= ImGui::SliderFloat("Strength##metalness", &material.metalness, 0, 1);
        changed |= materialTextureSelector(app, "metalness", material.metalnessTextureId, glm::vec3(0, 0, material.metalness));
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Emission"))
    {
        changed |= ImGui::ColorEdit3("Color##emission", glm::value_ptr(material.emissionColor), ImGuiColorEditFlags_Float);
        changed |= ImGui::DragFloat("Strength##emission", &material.emissionStrength, .001f, 0, 10000);
        changed |= materialTextureSelector(app, "emission", material.emissionTextureId, material.emissionColor * material.emissionStrength);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Fresnel"))
    {
        changed |= ImGui::ColorEdit3("Color##fresnel", glm::value_ptr(material.fresnelColor), ImGuiColorEditFlags_Float);
        changed |= ImGui::DragFloat("Strength##fresnel", &material.fresnelStrength, .0001f, 0, 100);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Normal"))
    {
        changed |= materialTextureSelector(app, "normal", material.normalTextureId, glm::vec3(1));
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Alpha"))
    {
        if (ImGui::BeginCombo(
            "Alpha mode",
            material.alphaMode == Material::AlphaMode::Opaque ?
            "Opaque" :
            material.alphaMode == Material::AlphaMode::Blend ?
            "Blend" :
            "Mask"))
        {
            if (ImGui::Selectable("Opaque", material.alphaMode == Material::AlphaMode::Opaque))
            {
                changed = true;
                material.alphaMode = Material::AlphaMode::Opaque;
            }

            if (ImGui::Selectable("Blend", material.alphaMode == Material::AlphaMode::Blend))
            {
                changed = true;
                material.alphaMode = Material::AlphaMode::Blend;
            }

            if (ImGui::Selectable("Mask", material.alphaMode == Material::AlphaMode::Mask))
            {
                changed = true;
                material.alphaMode = Material::AlphaMode::Mask;
            }

            ImGui::EndCombo();
        }

        ImGui::BeginDisabled(material.alphaMode != Material::AlphaMode::Mask);
        changed |= ImGui::SliderFloat("Alpha cutoff", &material.alphaCutoff, 0.f, 1.f);
        ImGui::EndDisabled();

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Misc"))
    {
        changed |= ImGui::DragFloat("Density", &material.density, .001f, 0, 100);
        changed |= ImGui::DragFloat("IOR", &material.ior, .001f, 0, 100);
        Tooltip("If you notice black spots, try to increase the max bounce count");

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    if (changed)
    {
        app.clear();
        app.renderer.updateSceneMaterials(app.scene);
    }
}

void propertyMeshInstance(Application& app, MeshInstance& meshInstance)
{
    if (!ImGui::BeginTabBar("propertyMeshMenu"))
    {
        return;
    }

    bool changed = false;

    if (ImGui::BeginTabItem("Info"))
    {
        ImGui::Text("%d triangles", app.scene.meshes[meshInstance.meshId].triangleSize);
        if (ImGui::Button("Focus camera", ImVec2(-1, 0)))
        {
            app.renderer.camera.lookAt(meshInstance.transform[3]);
            app.cameraControl.orbitOrigin = meshInstance.transform[3];
            app.clear();
        }

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Transform"))
    {
        float translation[3];
        float rotation[3];
        float scale[3];
        ImGuizmo::DecomposeMatrixToComponents(
            glm::value_ptr(meshInstance.transform),
            translation,
            rotation,
            scale);

        if (ImGui::DragFloat3("Translation", translation, .01f) |
            ImGui::DragFloat3("Rotation", rotation, .01f) |
            ImGui::DragFloat3("Scale", scale, .01f, 0, 1000))
        {
            ImGuizmo::RecomposeMatrixFromComponents(
                translation,
                rotation,
                scale,
                glm::value_ptr(meshInstance.transform));
            changed = true;
        }

        ImGui::DragFloat("Snap value", &app.gizmo.snap, 1, 0.f, 1000000);
        Tooltip("Hold LCtrl for snapping");

        ImGui::RadioButton("Translate", (int*)&app.gizmo.operation, (int)ImGuizmo::OPERATION::TRANSLATE);
        ImGui::SameLine();
        ImGui::RadioButton("Rotate", (int*)&app.gizmo.operation, (int)ImGuizmo::OPERATION::ROTATE);
        ImGui::SameLine();
        ImGui::RadioButton("Scale", (int*)&app.gizmo.operation, (int)ImGuizmo::OPERATION::SCALE);

        ImGui::RadioButton("World", (int*)&app.gizmo.mode, (int)ImGuizmo::MODE::WORLD);
        ImGui::SameLine();
        ImGui::RadioButton("Local", (int*)&app.gizmo.mode, (int)ImGuizmo::MODE::LOCAL);

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Material"))
    {
        if (ImGui::BeginCombo(
            "Material",
            meshInstance.materialId == -1 ? "None" : app.scene.materialNames[meshInstance.materialId].c_str()))
        {
            for (size_t index = 0; index < app.scene.materials.size(); index++)
            {
                if (ImGui::Selectable(
                    (app.scene.materialNames[index] + "##meshMaterial" + std::to_string(index)).c_str(),
                    index == meshInstance.materialId))
                {
                    meshInstance.materialId = (int)index;
                    changed = true;
                }
            }

            ImGui::EndCombo();
        }

        ImGui::BeginDisabled(meshInstance.materialId == -1);
        if (ImGui::Button("Edit", ImVec2(-1, 0)))
        {
            app.property.id = meshInstance.materialId;
            app.property.type = Application::Property::Type::Material;
        }

        ImGui::EndDisabled();

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    if (changed)
    {
        app.clear();
        app.renderer.updateSceneMeshInstances(app.scene);
    }
}

void propertyEnvironment(Application& app, Environment& environment)
{
    bool changed = false;

    changed |= ImGui::DragFloat("Intensity", &environment.intensity, .001f, 0, 1000000);

    float translation[3];
    float rotation[3];
    float scale[3];
    glm::mat4 rotationMat(environment.rotation);
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(rotationMat), translation, rotation, scale);
    if (ImGui::DragFloat3("Rotation", rotation, .01f))
    {
        ImGuizmo::RecomposeMatrixFromComponents(
            translation,
            rotation,
            scale,
            glm::value_ptr(rotationMat));
        environment.rotation = rotationMat;
        changed = true;
    }

    ImGui::DragFloat("Snap value", &app.gizmo.snap, 1, 0.f, 1000000);
    Tooltip("Hold LCtrl for snapping");

    changed |= ImGui::Checkbox("Transparent background", &environment.transparent);

    if (changed)
    {
        app.clear();
    }
}

void propertyCamera(Application& app, Camera& camera)
{
    if (!ImGui::BeginTabBar("propertyCamera"))
    {
        return;
    }

    bool changed = false;

    if (ImGui::BeginTabItem("Info"))
    {
        changed |= ImGui::DragFloat3("Position", glm::value_ptr(camera.position), .01f);
        changed |= ImGui::DragFloat3("Forward", glm::value_ptr(camera.forward), .01f);
        changed |= ImGui::DragFloat3("Up", glm::value_ptr(camera.up), .01f);

        ImGui::Separator();
        changed |= ImGui::SliderAngle("FOV", &camera.fov, 1, 179, "%.0f deg", ImGuiSliderFlags_AlwaysClamp);

        ImGui::Separator();
        changed |= ImGui::DragFloat("Min render distance", &camera.zNear, 0.01f, 0, camera.zFar, "%.4f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        changed |= ImGui::DragFloat("Max render distance", &camera.zFar, 1, camera.zNear, 10000, "%.4f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);

        ImGui::Separator();
        changed |= ImGui::DragFloat("Blur", &camera.blur, .000001f, 0, 1000, "%.5f");
        Tooltip("Can be used for anti-aliasing");

        ImGui::Separator();
        changed |= ImGui::DragFloat("Focal distance", &camera.focalDistance, .001f, 0, 1000, "%.5f");
        changed |= ImGui::DragFloat("Aperture", &camera.aperture, .0001f, 0, 1000, "%.5f");

        if (ImGui::Button("Focus on look at", ImVec2(-1, 0)))
        {
            camera.focalDistance = app.getLookAtDistance();
            changed = true;
        }

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Control"))
    {
        if (ImGui::BeginCombo("Control mode", app.cameraControl.mode == Application::CameraControl::Mode::Free ? "Free" : "Orbit"))
        {
            if (ImGui::Selectable("Free", app.cameraControl.mode == Application::CameraControl::Mode::Free))
            {
                app.setCameraMode(Application::CameraControl::Mode::Free);
            }

            if (ImGui::Selectable("Orbit", app.cameraControl.mode == Application::CameraControl::Mode::Orbit))
            {
                app.setCameraMode(Application::CameraControl::Mode::Orbit);
            }

            ImGui::EndCombo();
        }

        switch (app.cameraControl.mode)
        {
            case Application::CameraControl::Mode::Free:
                Tooltip("C - start/stop camera control mode\n"
                    "W, A, S, D - camera forward, left, backward, right movement\n"
                    "LShift, LCtrl - camera up, down movement\n"
                    "Mouse - camera rotation\n"
                    "Q, E - camera tilt\n");
                ImGui::SliderFloat("Rotation speed", &app.cameraControl.rotationSpeed, .001f, 1);
                ImGui::DragFloat(
                    "Movement speed",
                    &app.cameraControl.movementSpeed,
                    1,
                    0,
                    10000,
                    "%.3f",
                    ImGuiSliderFlags_Logarithmic);
                break;
            case Application::CameraControl::Mode::Orbit:
                Tooltip("Left mouse drag - camera rotation\n"
                    "Right mouse drag - camera zoom\n"
                    "Middle mouse drag - camera pan\n");
                
                float orbitRadius = glm::length(app.cameraControl.orbitOrigin - camera.position);
                ImGui::SliderFloat("Rotation speed", &app.cameraControl.rotationSpeed, .001f, 1);
                if (ImGui::DragFloat3("Orbit origin", glm::value_ptr(app.cameraControl.orbitOrigin), .01f) |
                    ImGui::DragFloat("Orbit radius", &orbitRadius, .01f, 0, 10000))
                {
                    changed = true;
                    camera.position = app.cameraControl.orbitOrigin - camera.forward * orbitRadius;
                }

                break;
        }

        ImGui::EndTabItem();
    }

    if (!app.scene.cameras.empty())
    {
        if (ImGui::BeginTabItem("Default cameras"))
        {
            for (size_t i = 0; i < app.scene.cameras.size(); i++)
            {
                if (ImGui::Selectable(("Camera " + std::to_string(i)).c_str()))
                {
                    camera = app.scene.cameras[i];
                    changed = true;
                }
            }

            ImGui::EndTabItem();
        }
    }

    ImGui::EndTabBar();

    if (changed)
    {
        camera.forward = glm::normalize(camera.forward);
        camera.up = glm::normalize(camera.up);
        app.clear();
    }
}

void propertySettings(Application& app)
{
    bool updated = false;

    glm::uvec2 size = app.renderer.getSize();
    if (DragUInt2("Render size", glm::value_ptr(size), 10, 1, 10000, "%d", ImGuiSliderFlags_AlwaysClamp))
    {
        app.renderer.resize(size);
        updated = true;
    }

    updated |= DragUInt2("Tiling factor", glm::value_ptr(app.tiling.factor), .01f, 1, 10000, "%d", ImGuiSliderFlags_AlwaysClamp) & (app.tiling.count != 0);
    Tooltip("Reduces lags but increases the rendering time, intended for large images");

    ImGui::Separator();
    DragUInt("Samples target", &app.rendering.sampleCountTarget, 1.f, 0, 100000, "%d", ImGuiSliderFlags_AlwaysClamp);
    Tooltip("Zero means unlimited samples");

    updated |= DragUInt("Samples per frame", &app.rendering.samplesPerFrame, .1f, 1, 10000, "%d", ImGuiSliderFlags_AlwaysClamp) & (app.tiling.count != 0);
    Tooltip("A high value can cause lags but the image quality improves faster");

    updated |= DragUInt("Max bounce count", &app.renderer.maxBounceCount, .01f, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);

    if (updated)
    {
        app.clear();
    }
}

void propertyToneMapping(Application& app, Renderer::ToneMapMode& toneMapMode)
{
    bool changed = false;

    if (ImGui::BeginCombo(
        "Tone mapping",
        toneMapMode == Renderer::ToneMapMode::Reinhard ?
        "Reinhard" :
        toneMapMode == Renderer::ToneMapMode::ACES ?
        "ACES" :
        "ACES fitted"))
    {
        if (ImGui::Selectable("Reinhard", toneMapMode == Renderer::ToneMapMode::Reinhard))
        {
            changed = true;
            toneMapMode = Renderer::ToneMapMode::Reinhard;
        }

        if (ImGui::Selectable("ACES", toneMapMode == Renderer::ToneMapMode::ACES))
        {
            changed = true;
            toneMapMode = Renderer::ToneMapMode::ACES;
        }

        if (ImGui::Selectable("ACES fitted", toneMapMode == Renderer::ToneMapMode::ACESfitted))
        {
            changed = true;
            toneMapMode = Renderer::ToneMapMode::ACESfitted;
        }

        ImGui::EndCombo();
    }

    changed |= ImGui::DragFloat("Gamma", &app.renderer.gamma, 0.01f, 0, 1000, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if (changed)
    {
        app.rendering.isPreview ? app.clear() : app.renderer.toneMap();
    }
}

void propertyControls(Application& app)
{
    if (ImGui::Button(app.rendering.enable ? "Stop rendering" : "Start rendering", ImVec2(-1, 0)))
    {
        app.switchRendering();
    }

    if (ImGui::Button("Clear", ImVec2(-1, 0)))
    {
        app.clear();
    }

#if TX_DENOISE
    if (ImGui::Button("Denoise", ImVec2(-1, 0)))
    {
        try
        {
            app.renderer.denoise();
        }
        catch (const std::runtime_error& err)
        {
            std::cerr << err.what() << std::endl;
            tinyfd_messageBox("Error", "Failed to denoise", "ok", "warning", 0);
        }
    }
#endif

#if !TX_SPIRV
    if (ImGui::Button("Reload shaders", ImVec2(-1, 0)))
    {
        app.reloadShaders();
    }
#endif
}

void propertyEditor(Application& app, Application::Property& property)
{
    ImGui::BeginChild("propertyEditorMenu", ImVec2(-1, 0), ImGuiChildFlags_Border);

    switch (property.type)
    {
        case Application::Property::Type::Contorls:
            propertyControls(app);
            break;
        case Application::Property::Type::ToneMapping:
            propertyToneMapping(app, app.renderer.toneMapMode);
            break;
        case Application::Property::Type::Settings:
            propertySettings(app);
            break;
        case Application::Property::Type::Camera:
            propertyCamera(app, app.renderer.camera);
            break;
        case Application::Property::Type::Environment:
            propertyEnvironment(app, app.renderer.environment);
            break;
        case Application::Property::Type::MeshInstance:
            propertyMeshInstance(app, app.scene.meshInstances[property.id]);
            break;
        case Application::Property::Type::Material:
            propertyMaterial(app, app.scene.materials[property.id]);
            break;
    }

    ImGui::EndChild();
}

void propertySelector(Application& app, Application::Property& property)
{
    ImGui::BeginChild(
        "sceneMenu",
        ImVec2(-1, ImGui::GetMainViewport()->WorkSize.y * .75f),
        ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Renderer"))
    {
        if (ImGui::Selectable("Controls", property.type == Application::Property::Type::Contorls))
        {
            property.type = Application::Property::Type::Contorls;
        }

        if (ImGui::Selectable("Tone mapping", property.type == Application::Property::Type::ToneMapping))
        {
            property.type = Application::Property::Type::ToneMapping;
        }

        if (ImGui::Selectable("Settings", property.type == Application::Property::Type::Settings))
        {
            property.type = Application::Property::Type::Settings;
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Scene"))
    {
        if (ImGui::Selectable("Camera", property.type == Application::Property::Type::Camera))
        {
            property.type = Application::Property::Type::Camera;
        }

        if (ImGui::Selectable("Environment", property.type == Application::Property::Type::Environment))
        {
            property.type = Application::Property::Type::Environment;
        }

        if (ImGui::TreeNode("Meshes"))
        {
            for (size_t id = 0; id < app.scene.meshInstances.size(); id++)
            {
                const MeshInstance& meshInstance = app.scene.meshInstances[id];
                std::string name = app.scene.meshNames[meshInstance.meshId] + "##" + std::to_string(id);
                if (ImGui::Selectable(name.c_str(), property.type == Application::Property::Type::MeshInstance && id == property.id))
                {
                    property.type = Application::Property::Type::MeshInstance;
                    property.id = id;
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Materials"))
        {
            for (size_t id = 0; id < app.scene.materials.size(); id++)
            {
                const std::string name = app.scene.materialNames[id] + "##" + std::to_string(id);
                if (ImGui::Selectable(name.c_str(), property.type == Application::Property::Type::Material && id == property.id))
                {
                    property.type = Application::Property::Type::Material;
                    property.id = id;
                }
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::EndChild();
}

void viewRenderTexture(Application& app, GLint textureHandler)
{
    ImGui::BeginChild("viewTexture");

    // Draw filled image
    glm::vec2 lo, up;
    app.renderTextureView.getUV(lo, up);
    drawFillImage(textureHandler, app.renderer.getSize(), app.renderTextureView.pos, app.renderTextureView.size, glm::vec3(1), lo, up, true);

    app.renderTextureView.isHover = ImGui::IsItemHovered();

    if (app.renderTextureView.uvSize != glm::vec2(1))
    {
        // Draw zoom rectangle
        app.renderTextureView.getRectFromUV(lo, up);
        ImGui::GetWindowDrawList()->AddRect(toImVec2(lo), toImVec2(up), ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableBorderStrong]));
        ImGui::EndChild();
        return;
    }

    // Draw tile rectangle
    glm::uvec2 pos, size;
    app.tiling.getTile(app.renderer.getSize(), pos, size);

    lo = (glm::vec2)pos / (glm::vec2)app.renderer.getSize();
    up = lo + (glm::vec2)size / (glm::vec2)app.renderer.getSize();
    lo.y = 1 - lo.y;
    up.y = 1 - up.y;

    app.renderTextureView.getRectFromUV(lo, up);
    ImGui::GetWindowDrawList()->AddRect(toImVec2(lo), toImVec2(up), ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableBorderLight]));

    // Draw gizmos
    glm::mat4 view = app.renderer.camera.createView();
    glm::mat4 projection = app.renderer.camera.createProjection((float)app.renderer.getSize().x, (float)app.renderer.getSize().y);
    ImGuizmo::SetRect(app.renderTextureView.pos.x, app.renderTextureView.pos.y, app.renderTextureView.size.x, app.renderTextureView.size.y);
    ImGuizmo::SetDrawlist();

    glm::vec3 snap(app.gizmo.snap);
    switch (app.property.type)
    {
        case Application::Property::Type::MeshInstance:
        {
            MeshInstance& meshInstance = app.scene.meshInstances[app.property.id];
            if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                app.gizmo.operation,
                app.gizmo.mode,
                glm::value_ptr(meshInstance.transform),
                nullptr,
                ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ? glm::value_ptr(snap) : nullptr))
            {
                app.clear();
                app.renderer.updateSceneMeshInstances(app.scene);
            }

            break;
        }
        case Application::Property::Type::Environment:
        {
            glm::mat4 rotation(app.renderer.environment.rotation);
            if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                ImGuizmo::OPERATION::ROTATE,
                ImGuizmo::MODE::LOCAL,
                glm::value_ptr(rotation),
                nullptr,
                ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ? glm::value_ptr(snap) : nullptr))
            {
                app.renderer.environment.rotation = rotation;
                app.clear();
            }

            break;
        }
        default:
            break;
    }

    ImGui::EndChild();
}

void sidePanel(Application& app)
{
    ImGui::BeginChild("sidePanelMenu", ImVec2(0, -1));
    propertySelector(app, app.property);
    propertyEditor(app, app.property);
    ImGui::EndChild();
}

void drawingPanel(Application& app)
{
    ImGui::BeginChild(
        "drawingPanelMenu",
        ImVec2(ImGui::GetMainViewport()->WorkSize.x * .75f, -1),
        ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border,
        ImGuiWindowFlags_NoScrollbar);

    if (!ImGui::BeginTabBar("viewTextureTab"))
    {
        ImGui::EndChild();
        return;
    }

    if (ImGui::BeginTabItem("View"))
    {
        viewRenderTexture(app, app.rendering.isPreview ? app.renderer.getAlbedoTextureHandler() : app.renderer.getTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Albedo"))
    {
        viewRenderTexture(app, app.renderer.getAlbedoTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Normal"))
    {
        viewRenderTexture(app, app.renderer.getNormalTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Depth"))
    {
        viewRenderTexture(app, app.renderer.getDepthTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Accumulator"))
    {
        viewRenderTexture(app, app.renderer.getAccumulatorTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Tonemapped"))
    {
        viewRenderTexture(app, app.renderer.getTextureHandler());
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::EndChild();
}

void mainWindow(Application& app)
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize, ImGuiCond_Always);
    if (!ImGui::Begin("mainWindowMenu", nullptr, ImGuiWindowFlags_NoDecoration))
    {
        return;
    }

    drawingPanel(app);
    ImGui::SameLine();
    sidePanel(app);

    ImGui::End();
}

void mainMenuBar(Application& app)
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Open scene", "(glb/gltf)"))
        {
            const char* patterns[] = { "*.glb", "*.gltf" };
            const char* path = tinyfd_openFileDialog(
                "Open scene file",
                app.sceneDir.string().c_str(),
                2,
                patterns,
                nullptr,
                0);
            if (path != nullptr)
            {
                app.loadScene(path);
            }
        }

        if (ImGui::MenuItem("Open environment", "(png/hdr/jpg)"))
        {
            const char* patterns[] = { "*.png", "*.hdr", "*.jpg" };
            const char* fileName = tinyfd_openFileDialog(
                "Open environment image",
                app.environmentDir.string().c_str(),
                3,
                patterns,
                nullptr,
                0);
            if (fileName != nullptr)
            {
                try
                {
                    app.renderer.environment.loadFromFile(fileName);
                    app.clear();
                }
                catch (const std::runtime_error& err)
                {
                    std::cerr << err.what() << std::endl;
                    tinyfd_messageBox("Error", "Invalid environment file", "ok", "warning", 0);
                }
            }
        }

        if (ImGui::MenuItem("Save", "(png)"))
        {
            const char* patterns[] = { "*.png" };
            const char* fileName = tinyfd_saveFileDialog("Save image", nullptr, 1, patterns, nullptr);
            if (fileName != nullptr)
            {
                try
                {
                    app.renderer.getImage().saveToFile(fileName);
                }
                catch (const std::runtime_error& err)
                {
                    std::cerr << err.what() << std::endl;
                    tinyfd_messageBox("Error", "Failed to save the image", "ok", "warning", 0);
                }
            }
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Controls"))
    {
        ImGui::Text("Space - start/stop rendering");
        ImGui::Separator();
        ImGui::Text("LCtrl + mouse wheel - zoom in/out");
        ImGui::Text("LCtrl + left mouse drag - move view");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("About"))
    {
        ImGui::Text("Made by: Charlotte000");
        ImGui::Text("Version: v3");
        ImGui::EndMenu();
    }

    ImGui::Separator();
    ImGui::Text("%4.0fms", 1000 * ImGui::GetIO().DeltaTime);

    ImGui::Separator();
    ImGui::Text("Samples: %u", app.renderer.getSampleCount());
    ImGui::Separator();

    if (!app.isSceneLoaded)
    {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 5);
        Im_Spinner("Spinner", 5, 1.5f, IM_COL32(255, 255, 255, 255));
    }

    ImGui::EndMainMenuBar();
}
#pragma endregion

void Application::initUI()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init();

    SetupImGuiStyle();

    this->materialTextureView.texture.init(GL_RGBA32F);
}

void Application::shutdownUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    this->materialTextureView.texture.shutdown();
}

void Application::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    this->control();

    mainMenuBar(*this);
    mainWindow(*this);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
