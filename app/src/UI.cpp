#include "UI.h"
#include "Application.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/polar_coordinates.hpp>

using namespace TracerX;
using namespace TracerX::core;

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

void UI::shutdown()
{
    if (this->app == nullptr)
    {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    this->textureView.shutdown();
}

void UI::init(Application* app)
{
    this->app = app;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->app->window, true);
    ImGui_ImplOpenGL3_Init();

    SetupImGuiStyle();

    this->textureView.init();
    this->textureView.update(Image::loadFromMemory(glm::ivec2(2048, 2048), std::vector<float>()));
}

void UI::render()
{
    if (this->app == nullptr)
    {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    this->barMenu();
    this->mainWindowMenu();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::barMenu()
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Open"))
        {
            if (ImGui::BeginCombo("##fileOpen", scene.name.c_str()))
            {
                for (const std::string& name : this->app->sceneFiles)
                {
                    if (ImGui::Selectable(name.c_str(), scene.name == name))
                    {
                        Image oldEnv = scene.environment;
                        std::string oldEnvName = scene.environmentName;
                        scene = Scene::loadGLTF(Application::sceneFolder + name);
                        scene.environment = oldEnv;
                        scene.environmentName = oldEnvName;
                        renderer.resetScene(scene);

                        this->editMesh = nullptr;
                        this->editMeshTransform = glm::mat4(1);
                        this->editMaterial = nullptr;
                        this->editCamera = false;
                        this->editEnvironment = false;
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save (as png)"))
        {
            this->app->save();
        }

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Info"))
    {
        ImGui::OpenPopup("infoMenu");
    }
    
    if (ImGui::BeginPopup("infoMenu"))
    {
        ImGui::Text(
            "C - start/stop camera control mode\n"
            "W, A, S, D - camera forward, left, backward, right movement\n"
            "LShift, LCtrl - camera up, down movement\n"
            "Mouse - camera rotation\n"
            "Q, E - camera tilt\n"
            "Space - start/stop rendering"
        );
        ImGui::EndPopup();
    }

    float elapsedTime = ImGui::GetIO().DeltaTime;
    ImGui::Separator();
    ImGui::Text("%4.0fms", 1000 * elapsedTime);

    ImGui::Separator();
    ImGui::Text("Frame count: %d", renderer.frameCount - 1);

    ImGui::EndMainMenuBar();
}

void UI::mainWindowMenu()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize, ImGuiCond_Always);
    if (!ImGui::Begin("mainWindowMenu", nullptr, ImGuiWindowFlags_NoDecoration))
    {
        return;
    }

    this->drawingPanelMenu();
    ImGui::SameLine();
    this->sidePanelMenu();

    ImGui::End();
}

void UI::drawingPanelMenu()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    ImGui::BeginChild("drawingPanelMenu", ImVec2(ImGui::GetMainViewport()->WorkSize.x * .75f, -1), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar);
    ImVec2 size = ImGui::GetContentRegionAvail();
    float aspectRatio = size.x / size.y;
    float imAspectRatio = (float)renderer.output.colorTexture.size.x / renderer.output.colorTexture.size.y;
    ImVec2 imageSize = imAspectRatio > aspectRatio ? ImVec2(size.x, size.y / imAspectRatio * aspectRatio) : ImVec2(size.x * imAspectRatio / aspectRatio, size.y);
    ImVec2 imagePos((ImGui::GetWindowSize().x - imageSize.x) * 0.5f, (ImGui::GetWindowSize().y - imageSize.y) * 0.5f);

    ImGui::SetCursorPos(imagePos);
    ImGui::Image((void*)(intptr_t)renderer.output.colorTexture.getHandler(), imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    glm::mat4 view = renderer.camera.createView();
    glm::mat4 projection = renderer.camera.createProjection(imAspectRatio);
    ImGuizmo::SetRect(imagePos.x + ImGui::GetWindowPos().x, imagePos.y + ImGui::GetWindowPos().y, imageSize.x, imageSize.y);
    ImGuizmo::SetDrawlist();

    if (this->editMesh &&
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), this->operation, this->mode, glm::value_ptr(this->editMeshTransform)) &&
        this->autoApply)
    {
            this->editMesh->transform = this->editMeshTransform;
            const std::vector<glm::vec3>& bvh = scene.createBVH();

            renderer.resetAccumulator();
            renderer.resetMeshes(scene.meshes);
            renderer.resetBVH(bvh, scene.triangles);
    }

    if (this->editEnvironment)
    {
        glm::mat4 rotation(renderer.environmentRotation);
        if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::WORLD, glm::value_ptr(rotation)))
        {
            renderer.environmentRotation = rotation;
            renderer.resetAccumulator();
        }
    }

    ImGui::EndChild();
}

void UI::sidePanelMenu()
{
    ImGui::BeginChild("sidePanelMenu", ImVec2(0, -1));
    this->sceneMenu();
    this->propertyMenu();
    ImGui::EndChild();
}

void UI::sceneMenu()
{
    Scene& scene = this->app->scene;

    ImGui::BeginChild("sceneMenu", ImVec2(-1, ImGui::GetMainViewport()->WorkSize.y * .75f), ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Scene"))
    {
        if (ImGui::Selectable("Camera", &this->editCamera))
        {
            this->editMesh = nullptr;
            this->editMaterial = nullptr;
            this->editEnvironment = false;
        }

        if (ImGui::Selectable("Environment", &this->editEnvironment))
        {
            this->editMesh = nullptr;
            this->editMaterial = nullptr;
            this->editCamera = false;
        }

        if (ImGui::TreeNode("Meshes"))
        {
            for (size_t i = 0; i < scene.meshes.size(); i++)
            {
                Mesh& mesh = scene.meshes[i];
                std::string name = scene.meshNames[i] + "##" + std::to_string(i);
                bool isMesh = &mesh == this->editMesh;
                if (ImGui::Selectable(name.c_str(), &isMesh))
                {
                    this->editMesh = isMesh ? &mesh : nullptr;
                    this->editMeshTransform = isMesh ? mesh.transform : glm::mat4(1);
                    this->editMaterial = nullptr;
                    this->editCamera = false;
                    this->editEnvironment = false;
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Materials"))
        {
            for (size_t i = 0; i < scene.materials.size(); i++)
            {
                Material& material = scene.materials[i];
                std::string name = scene.materialNames[i] + "##" + std::to_string(i);
                bool isMaterial = &material == this->editMaterial;
                if (ImGui::Selectable(name.c_str(), &isMaterial))
                {
                    this->editMesh = nullptr;
                    this->editMaterial = isMaterial ? &material : nullptr;
                    this->editCamera = false;
                    this->editEnvironment = false;
                }

                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("material", &i, sizeof(size_t));
                    ImGui::Text(scene.materialNames[i].c_str());
                    ImGui::EndDragDropSource();
                }
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::EndChild();
}

void UI::propertyMenu()
{
    ImGui::BeginChild("propertyMenu", ImVec2(-1, 0), ImGuiChildFlags_Border);

    if (this->editMesh)
    {
        this->propertyMeshMenu();
    }
    else if (this->editMaterial)
    {
        this->propertyMaterialMenu();
    }
    else if (this->editCamera)
    {
        this->propertyCameraMenu();
    }
    else if (this->editEnvironment)
    {
        this->propertyEnvironmentMenu();
    }
    else
    {
        this->propertySceneMenu();
    }

    ImGui::EndChild();
}

void UI::propertyMeshMenu()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;
    Mesh& mesh = *this->editMesh;

    ImGui::Text("%d triangles", (int)mesh.triangleSize);

    if (ImGui::BeginCombo("Material", (int)mesh.materialId == -1 ? "None" : scene.materialNames[(int)mesh.materialId].c_str()))
    {
        for (size_t index = 0; index < scene.materials.size(); index++)
        {
            if (ImGui::Selectable((scene.materialNames[index] + "##meshMaterial" + std::to_string(index)).c_str(), index == (int)mesh.materialId))
            {
                mesh.materialId = (float)index;
                renderer.resetAccumulator();
                renderer.resetMeshes(scene.meshes);
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("material");
        if (payload)
        {
            size_t index = *(size_t*)payload->Data;
            mesh.materialId = (float)index;
            renderer.resetAccumulator();
            renderer.resetMeshes(scene.meshes);
        }

        ImGui::EndDragDropTarget();
    }

    if (ImGui::Button("Focus camera", ImVec2(-1, 0)))
    {
        static glm::vec3 translation;
        static glm::vec3 rotation;
        static glm::vec3 scale;
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(mesh.transform), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
        renderer.camera.lookAt(translation);
        renderer.resetAccumulator();
    }

    static float translation[3];
    static float rotation[3];
    static float scale[3];
    ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr(this->editMeshTransform), translation, rotation, scale);

    bool modified = false;
    modified |= ImGui::DragFloat3("Translation", translation, .01f);
    modified |= ImGui::DragFloat3("Rotation", rotation, .01f);
    modified |= ImGui::DragFloat3("Scale", scale, .01f, 0.f, 1000.f);

    ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale,  glm::value_ptr(this->editMeshTransform));

    ImGui::RadioButton("Translate", (int*)&this->operation, (int)ImGuizmo::OPERATION::TRANSLATE);
    ImGui::SameLine();
    ImGui::RadioButton("Rotate", (int*)&this->operation, (int)ImGuizmo::OPERATION::ROTATE);
    ImGui::SameLine();
    ImGui::RadioButton("Scale", (int*)&this->operation, (int)ImGuizmo::OPERATION::SCALE);

    ImGui::RadioButton("World", (int*)&this->mode, (int)ImGuizmo::MODE::WORLD);
    ImGui::SameLine();
    ImGui::RadioButton("Local", (int*)&this->mode, (int)ImGuizmo::MODE::LOCAL);

    ImGui::Separator();
    ImGui::Checkbox("Auto apply", &this->autoApply);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text("May cause lags due to bvh recalculation");
        ImGui::EndTooltip();
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(this->autoApply);
    if (ImGui::Button("Apply", ImVec2(-1, 0)) || modified && this->autoApply)
    {
        this->editMesh->transform = this->editMeshTransform;
        const std::vector<glm::vec3>& bvh = scene.createBVH();

        renderer.resetAccumulator();
        renderer.resetMeshes(scene.meshes);
        renderer.resetBVH(bvh, scene.triangles);
    }

    ImGui::EndDisabled();
}

void UI::propertyMaterialMenu()
{
    Renderer& renderer = this->app->renderer;
    const Scene& scene = this->app->scene;
    Material& material = *this->editMaterial;

    if (!ImGui::BeginTabBar("propertyMaterialMenu"))
    {
        return;
    }

    if (ImGui::BeginTabItem("Albedo"))
    {
        if (ImGui::ColorEdit3("Color##albedo", glm::value_ptr(material.albedoColor), ImGuiColorEditFlags_Float))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        this->materialTextureSelector("albedo", material.albedoTextureId);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Roughness"))
    {
        if (ImGui::SliderFloat("Strength##roughness", &material.roughness, 0.f, 1.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        this->materialTextureSelector("roughness", material.roughnessTextureId);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Metalness"))
    {
        if (ImGui::SliderFloat("Strength##metalness", &material.metalness, 0.f, 1.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        this->materialTextureSelector("metalness", material.metalnessTextureId);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Emission"))
    {
        if (ImGui::ColorEdit3("Color##emission", glm::value_ptr(material.emissionColor), ImGuiColorEditFlags_Float) |
            ImGui::DragFloat("Strength##emission", &material.emissionStrength, .001f, .0f, 10000.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        this->materialTextureSelector("emission", material.emissionTextureId);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Fresnel"))
    {
        if (ImGui::ColorEdit3("Color##fresnel", glm::value_ptr(material.fresnelColor), ImGuiColorEditFlags_Float) |
            ImGui::DragFloat("Strength##fresnel", &material.fresnelStrength, .0001f, 0.f, 100.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Normal"))
    {
        this->materialTextureSelector("normal", material.normalTextureId);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Misc"))
    {
        if (ImGui::DragFloat("Density", &material.density, .001f, 0.f, 100.f) |
            ImGui::DragFloat("IOR", &material.ior, .001f, 0.f, 100.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
}

void UI::propertyCameraMenu()
{
    Renderer& renderer = this->app->renderer;
    const Scene& scene = this->app->scene;
    Camera& camera = renderer.camera;

    if (ImGui::DragFloat3("Position", glm::value_ptr(camera.position), .01f) |
        ImGui::DragFloat3("Forward", glm::value_ptr(camera.forward), .01f) |
        ImGui::DragFloat3("Up", glm::value_ptr(camera.up), .01f))
    {
        camera.forward = glm::normalize(camera.forward);
        camera.up = glm::normalize(camera.up);
        renderer.resetAccumulator();
    }

    ImGui::Separator();
    if (ImGui::SliderAngle("FOV", &camera.fov, 0.f, 180.f))
    {
        renderer.resetAccumulator();
    }

    ImGui::Separator();
    if (ImGui::DragFloat("Focal distance", &camera.focalDistance, .001f, 0.f, 1000.f) |
        ImGui::DragFloat("Aperture", &camera.aperture, .0001f, 0.f, 1000.f))
    {
        renderer.resetAccumulator();
    }

    ImGui::Separator();
    if (ImGui::DragFloat("Blur", &camera.blur, .000001f, 0.f, 1000.f, "%.5f"))
    {
        renderer.resetAccumulator();
    }

    ImGui::Separator();
    ImGui::DragFloat("Movement speed", &this->app->cameraSpeed, 1.f, 0.f, 10000.f, "%.3f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Rotation speed", &this->app->cameraRotationSpeed, .001f, 1.f);

    if (!scene.cameras.empty())
    {
        ImGui::Separator();
        if (ImGui::TreeNode("Default cameras"))
        {
            for (size_t i = 0; i < scene.cameras.size(); i++)
            {
                if (ImGui::Selectable(("Camera " + std::to_string(i)).c_str()))
                {
                    camera = scene.cameras[i];
                    renderer.resetAccumulator();
                }
            }

            ImGui::TreePop();
        }
    }
}

void UI::propertyEnvironmentMenu()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    if (ImGui::DragFloat("Intensity", &renderer.environmentIntensity, .001f, .0f, 1000000.f))
    {
        renderer.resetAccumulator();
    }

    if (ImGui::BeginCombo("##environment", scene.environmentName.c_str()))
    {
        if (ImGui::Selectable("None", scene.environmentName == "None"))
        {
            scene.environment = Image::empty;
            scene.environmentName = "None";
            renderer.resetAccumulator();
            renderer.resetEnvironment(scene.environment);
        }

        for (size_t i = 0; i < this->app->environmentFiles.size(); i++)
        {
            const std::string& environmentName = this->app->environmentFiles[i];
            if (ImGui::Selectable((environmentName + "##environmentTexture" + std::to_string(i)).c_str(), environmentName == scene.environmentName))
            {
                scene.loadEnvironmentMap(Application::environmentFolder + environmentName);
                renderer.resetAccumulator();
                renderer.resetEnvironment(scene.environment);
            }
        }

        ImGui::EndCombo();
    }

    static float translation[3];
    static float rotation[3];
    static float scale[3];
    glm::mat4 rotationMat(renderer.environmentRotation);
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(rotationMat), translation, rotation, scale);
    if (ImGui::DragFloat3("Rotation", rotation, .01f))
    {
        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale,  glm::value_ptr(rotationMat));
        renderer.environmentRotation = rotationMat;
        renderer.resetAccumulator();
    }

    if (ImGui::Checkbox("Transparent background", &renderer.transparentBackground))
    {
        renderer.resetAccumulator();
    }
}

void UI::propertySceneMenu()
{
    Renderer& renderer = this->app->renderer;

    glm::ivec2 size = renderer.output.colorTexture.size;
    if (ImGui::DragInt2("Render size", glm::value_ptr(size), 10.f, 1, 10000))
    {
        renderer.resize(size);
    }

    if (ImGui::DragFloat("Gamma", &renderer.gamma, 0.01f, 0.f, 1000.f) |
        ImGui::DragInt("Max bouce count", &renderer.maxBouceCount, .01f, 0, 1000))
    {
        renderer.resetAccumulator();
    }

    ImGui::Separator();
    if (ImGui::Button(this->app->isRendering ? "Stop" : "Start", ImVec2(-1, 0)))
    {
        this->app->isRendering = !this->app->isRendering;
    }

#ifdef TX_DENOISE
    if (ImGui::Button("Denoise", ImVec2(-1, 0)))
    {
        renderer.denoise();
    }
#endif
}

void UI::materialTextureSelector(const std::string& name, float& currentTextureId)
{
    Renderer& renderer = this->app->renderer;
    const Scene& scene = this->app->scene;

    if (ImGui::BeginCombo(("Texture##" + name).c_str(), currentTextureId == -1 ? "None" : scene.textureNames[currentTextureId].c_str()))
    {
        if (ImGui::Selectable("None", (int)currentTextureId == -1))
        {
            currentTextureId = -1.f;
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        for (size_t textureId = 0; textureId < scene.textures.size(); textureId++)
        {
            const Image& texture = scene.textures[textureId];
            const std::string& textureName = scene.textureNames[textureId];
            if (ImGui::Selectable((textureName + "##" + name + "Texture" + std::to_string(textureId)).c_str(), (int)currentTextureId == textureId))
            {
                currentTextureId = (float)textureId;
                renderer.resetAccumulator();
                renderer.resetMaterials(scene.materials);
            }
        }

        ImGui::EndCombo();
    }

    if (currentTextureId != -1)
    {
        renderer.textureArray.copy(this->textureView, currentTextureId);
        ImGui::Image((void*)(intptr_t)this->textureView.getHandler(), ImVec2(200, 200));
    }
}
