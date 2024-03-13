#include "TracerX/UI.h"
#include "TracerX/Application.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/polar_coordinates.hpp>


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
}

void UI::init(Application* app)
{
    this->app = app;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->app->window, true);
    ImGui_ImplOpenGL3_Init();

    SetupImGuiStyle();
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
    ImGuizmo::SetRect(this->imagePos.x, this->imagePos.y, this->imageSize.x, this->imageSize.y);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    this->barMenu();
    this->mainWindowMenu();

    if (this->editMesh)
    {
        glm::mat4 view = this->app->renderer.camera.createView();
        glm::mat4 projection = this->app->renderer.camera.createProjection();

        if (this->showBox)
        {
            ImGuizmo::DrawCubes(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(this->editMeshTransform), 1);
        }

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), this->operation, this->mode, glm::value_ptr(this->editMeshTransform));
    }

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

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Open"))
        {
            if (ImGui::BeginCombo("##fileOpen", this->app->scene.name.c_str()))
            {
                for (const std::string& name : this->app->sceneFiles)
                {
                    if (ImGui::Selectable(name.c_str(), this->app->scene.name == name))
                    {
                        Image oldEnv = this->app->scene.environment;
                        this->app->scene = Scene::loadGLTF(Application::sceneFolder + name);
                        this->app->scene.environment = oldEnv;
                        this->app->renderer.resetScene(this->app->scene);
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::EndMenu();
        }

        if (ImGui::Selectable("Save (as png)"))
        {
            this->app->save();
        }

        ImGui::EndMenu();
    }

    float elapsedTime = ImGui::GetIO().DeltaTime;
    ImGui::Separator();
    ImGui::Text("%.0fms", 1000 * elapsedTime);

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
    ImGui::BeginChild("drawingPanelMenu", ImVec2(ImGui::GetMainViewport()->WorkSize.x * .75f, -1), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar);
    ImVec2 size = ImGui::GetContentRegionAvail();
    float aspectRatio = size.x / size.y;
    float imAspectRatio = this->app->renderer.camera.aspectRatio;
    this->imageSize = imAspectRatio > aspectRatio ? ImVec2(size.x, size.y / imAspectRatio * aspectRatio) : ImVec2(size.x * imAspectRatio / aspectRatio, size.y);
    this->imagePos = ImVec2((ImGui::GetWindowSize().x - this->imageSize.x) * 0.5f, (ImGui::GetWindowSize().y - this->imageSize.y) * 0.5f);

    ImGui::SetCursorPos(this->imagePos);
    this->imagePos = ImVec2(this->imagePos.x + ImGui::GetWindowContentRegionMin().x, this->imagePos.y + ImGui::GetWindowContentRegionMin().y);
    ImGui::Image((void*)(intptr_t)this->app->renderer.output.colorTexture.getHandler(), this->imageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
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
    ImGui::BeginChild("sceneMenu", ImVec2(-1, ImGui::GetMainViewport()->WorkSize.y * .75f), ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Scene"))
    {
        bool isCamera = this->editCamera != nullptr;
        if (ImGui::Selectable("Camera", &isCamera))
        {
            this->editMesh = nullptr;
            this->editMaterial = nullptr;
            this->editCamera = isCamera ? &this->app->renderer.camera : nullptr;
            this->editEnvironment = nullptr;
        }

        bool isEnvironment = this->editEnvironment != nullptr;
        if (ImGui::Selectable("Environment", &isEnvironment))
        {
            this->editMesh = nullptr;
            this->editMaterial = nullptr;
            this->editCamera = nullptr;
            this->editEnvironment = isEnvironment ? &this->app->scene.environment : nullptr;
        }

        if (ImGui::TreeNode("Meshes"))
        {
            for (size_t i = 0; i < this->app->scene.meshes.size(); i++)
            {
                Mesh& mesh = this->app->scene.meshes[i];
                std::string name = this->app->scene.meshNames[i] + "##" + std::to_string(i);
                bool isMesh = &mesh == this->editMesh;
                if (ImGui::Selectable(name.c_str(), &isMesh))
                {
                    this->editMesh = isMesh ? &mesh : nullptr;
                    this->editMeshTransform = isMesh ? mesh.transform : glm::mat4(1);
                    this->editMaterial = nullptr;
                    this->editCamera = nullptr;
                    this->editEnvironment = nullptr;
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Materials"))
        {
            for (size_t i = 0; i < this->app->scene.materials.size(); i++)
            {
                Material& material = this->app->scene.materials[i];
                std::string name = this->app->scene.materialNames[i] + "##" + std::to_string(i);
                bool isMaterial = &material == this->editMaterial;
                if (ImGui::Selectable(name.c_str(), &isMaterial))
                {
                    this->editMesh = nullptr;
                    this->editMaterial = isMaterial ? &material : nullptr;
                    this->editCamera = nullptr;
                    this->editEnvironment = nullptr;
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
    ImGui::DragFloat3("Translation", translation, .01f);
    ImGui::DragFloat3("Rotation", rotation, .01f);
    ImGui::DragFloat3("Scale", scale, .01f, 0.f, 1000.f);

    ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale,  glm::value_ptr(this->editMeshTransform));

    ImGui::RadioButton("Translate", (int*)&this->operation, (int)ImGuizmo::OPERATION::TRANSLATE);
    ImGui::SameLine();
    ImGui::RadioButton("Rotate", (int*)&this->operation, (int)ImGuizmo::OPERATION::ROTATE);
    ImGui::SameLine();
    ImGui::RadioButton("Scale", (int*)&this->operation, (int)ImGuizmo::OPERATION::SCALE);

    ImGui::RadioButton("World", (int*)&this->mode, (int)ImGuizmo::MODE::WORLD);
    ImGui::SameLine();
    ImGui::RadioButton("Local", (int*)&this->mode, (int)ImGuizmo::MODE::LOCAL);

    ImGui::Checkbox("Show box", &this->showBox);

    if (ImGui::Button("Apply", ImVec2(-1, 0)))
    {
        this->editMesh->transform = this->editMeshTransform;
        const std::vector<glm::vec3>& bvh = scene.createBVH();

        renderer.resetAccumulator();
        renderer.resetMeshes(scene.meshes);
        renderer.resetBVH(bvh, scene.triangles);
    }
}

void UI::propertyMaterialMenu()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;
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

        if (ImGui::BeginCombo("Texture##albedo", material.albedoTextureId == -1 ? "None" : scene.textures[material.albedoTextureId].name.c_str()))
        {
            if (ImGui::Selectable("None", (int)material.albedoTextureId == -1))
            {
                material.albedoTextureId = -1.f;
                renderer.resetAccumulator();
                renderer.resetMaterials(scene.materials);
            }

            for (size_t textureId = 0; textureId < scene.textures.size(); textureId++)
            {
                Image& texture = scene.textures[textureId];
                if (ImGui::Selectable((texture.name + "##albedoTexture" + std::to_string(textureId)).c_str(), (int)material.albedoTextureId == textureId))
                {
                    material.albedoTextureId = (float)textureId;
                    renderer.resetAccumulator();
                    renderer.resetMaterials(scene.materials);
                }
            }

            ImGui::EndCombo();
        }

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Roughness"))
    {
        if (ImGui::SliderFloat("Strength##roughness", &material.roughness, 0.f, 1.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        if (ImGui::BeginCombo("Texture##roughness", material.roughnessTextureId == -1 ? "None" : scene.textures[material.roughnessTextureId].name.c_str()))
        {
            if (ImGui::Selectable("None", (int)material.roughnessTextureId == -1))
            {
                material.roughnessTextureId = -1.f;
                renderer.resetAccumulator();
                renderer.resetMaterials(scene.materials);
            }

            for (size_t textureId = 0; textureId < scene.textures.size(); textureId++)
            {
                Image& texture = scene.textures[textureId];
                if (ImGui::Selectable((texture.name + "##roughnessTexture" + std::to_string(textureId)).c_str(), (int)material.roughnessTextureId == textureId))
                {
                    material.roughnessTextureId = (float)textureId;
                    renderer.resetAccumulator();
                    renderer.resetMaterials(scene.materials);
                }
            }

            ImGui::EndCombo();
        }

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Metalness"))
    {
        if (ImGui::SliderFloat("Strength##metalness", &material.metalness, 0.f, 1.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        if (ImGui::BeginCombo("Texture##metalness", material.metalnessTextureId == -1 ? "None" : scene.textures[material.metalnessTextureId].name.c_str()))
        {
            if (ImGui::Selectable("None", (int)material.metalnessTextureId == -1))
            {
                material.metalnessTextureId = -1.f;
                renderer.resetAccumulator();
                renderer.resetMaterials(scene.materials);
            }

            for (size_t textureId = 0; textureId < scene.textures.size(); textureId++)
            {
                Image& texture = scene.textures[textureId];
                if (ImGui::Selectable((texture.name + "##metalnessTexture" + std::to_string(textureId)).c_str(), (int)material.metalnessTextureId == textureId))
                {
                    material.metalnessTextureId = (float)textureId;
                    renderer.resetAccumulator();
                    renderer.resetMaterials(scene.materials);
                }
            }

            ImGui::EndCombo();
        }

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

        if (ImGui::BeginCombo("Texture##emission", material.emissionTextureId == -1 ? "None" : scene.textures[material.emissionTextureId].name.c_str()))
        {
            if (ImGui::Selectable("None", (int)material.emissionTextureId == -1))
            {
                material.emissionTextureId = -1.f;
                renderer.resetAccumulator();
                renderer.resetMaterials(scene.materials);
            }

            for (size_t textureId = 0; textureId < scene.textures.size(); textureId++)
            {
                Image& texture = scene.textures[textureId];
                if (ImGui::Selectable((texture.name + "##emissionTexture" + std::to_string(textureId)).c_str(), (int)material.emissionTextureId == textureId))
                {
                    material.emissionTextureId = (float)textureId;
                    renderer.resetAccumulator();
                    renderer.resetMaterials(scene.materials);
                }
            }

            ImGui::EndCombo();
        }

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
        if (ImGui::BeginCombo("Texture##normal", material.normalTextureId == -1 ? "None" : scene.textures[material.normalTextureId].name.c_str()))
        {
            if (ImGui::Selectable("None", (int)material.normalTextureId == -1))
            {
                material.normalTextureId = -1.f;
                renderer.resetAccumulator();
                renderer.resetMaterials(scene.materials);
            }

            for (size_t textureId = 0; textureId < scene.textures.size(); textureId++)
            {
                Image& texture = scene.textures[textureId];
                if (ImGui::Selectable((texture.name + "##normalTexture" + std::to_string(textureId)).c_str(), (int)material.normalTextureId == textureId))
                {
                    material.normalTextureId = (float)textureId;
                    renderer.resetAccumulator();
                    renderer.resetMaterials(scene.materials);
                }
            }

            ImGui::EndCombo();
        }

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
    ImGui::DragFloat("Movement speed", &this->app->cameraSpeed, 1.f, 0.f, 10000.f, "%.3f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Rotation speed", &this->app->cameraRotationSpeed, .001f, 1.f);
}

void UI::propertyEnvironmentMenu()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;
    Image& environment = *this->editEnvironment;

    if (ImGui::DragFloat("Intensity", &renderer.environmentIntensity, .001f, .0f, 1000000.f))
    {
        renderer.resetAccumulator();
    }

    if (ImGui::BeginCombo("##environment", environment.name.c_str()))
    {
        if (ImGui::Selectable("None", environment.name == "None"))
        {
            environment = Image::empty;
            renderer.resetAccumulator();
            renderer.resetEnvironment(environment);
        }

        for (size_t i = 0; i < this->app->environmentFiles.size(); i++)
        {
            const std::string& environmentName = this->app->environmentFiles[i];
            if (ImGui::Selectable((environmentName + "##environmentTexture" + std::to_string(i)).c_str(), environmentName == environment.name))
            {
                scene.loadEnvironmentMap(Application::environmentFolder + environmentName);
                renderer.resetAccumulator();
                renderer.resetEnvironment(environment);
            }
        }

        ImGui::EndCombo();
    }
}

void UI::propertySceneMenu()
{
    Renderer& renderer = this->app->renderer;

    if (ImGui::DragInt2("Render size", glm::value_ptr(renderer.output.colorTexture.size), 10.f, 1, 10000))
    {
        renderer.resize(renderer.output.colorTexture.size);
    }

    ImGui::DragFloat("Gamma", &renderer.gamma, 0.01f, 0.f, 1000.f);
    if (ImGui::DragInt("Max bouce count", &renderer.maxBouceCount, .01f, 0, 1000))
    {
        renderer.resetAccumulator();
    }

    ImGui::Separator();
    if (ImGui::Button(this->app->isRendering ? "Stop" : "Start", ImVec2(-1, 0)))
    {
        this->app->isRendering = !this->app->isRendering;
    }

    if (ImGui::Button("Denoise", ImVec2(-1, 0)))
    {
        renderer.denoise();
    }
}
