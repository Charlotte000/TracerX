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

bool DragUInt(const char* label, unsigned int* v, float v_speed = 1.f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0)
{
    return ImGui::DragScalar(label, ImGuiDataType_U32, v, v_speed, &v_min, &v_max, format, flags);
}

bool DragUInt2(const char* label, unsigned int v[2], float v_speed = 1.f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0)
{
    return ImGui::DragScalarN(label, ImGuiDataType_U32, v, 2, v_speed, &v_min, &v_max, format, flags);
}

void Tooltip(const std::string& content)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text(content.c_str());
        ImGui::EndTooltip();
    }}

void Application::initUI()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init();

    SetupImGuiStyle();

    this->materialTexture.texture.init(GL_RGBA32F);
    this->materialTexture.texture.update(Image::empty);
}

void Application::shutdownUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    this->materialTexture.texture.shutdown();
}

void Application::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    this->mainMenuBar();
    this->mainWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::mainMenuBar()
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
                Application::sceneFolder.string().c_str(),
                2,
                patterns,
                nullptr,
                0);
            if (path != nullptr)
            {
                try
                {
                    this->loadScene(path);
                    this->materialTexture.textureId = -1;
                    if (this->property.target != nullptr)
                    {
                        this->property.target = nullptr;
                        this->property.type = Property::Type::Contorls;
                    }
                }
                catch (const std::runtime_error& err)
                {
                    std::cerr << err.what() << std::endl;
                    tinyfd_messageBox("Error", "Invalid scene file", "ok", "warning", 0);
                }
            }
        }

        if (ImGui::MenuItem("Open environment", "(png/hdr/jpg)"))
        {
            const char* patterns[] = { "*.png", "*.hdr", "*.jpg" };
            const char* fileName = tinyfd_openFileDialog(
                "Open environment image",
                Application::environmentFolder.string().c_str(),
                3,
                patterns,
                nullptr,
                0);
            if (fileName != nullptr)
            {
                try
                {
                    this->renderer.environment.loadFromFile(fileName);
                    this->clear();
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
                    this->renderer.getImage().saveToFile(fileName);
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
        ImGui::Text(
            "Space - start/stop rendering\n"
            "Mouse wheel - zoom in/out\n"
            "Mouse drag - move view\n"
        );

        if (ImGui::BeginMenu("Camera"))
        {
            ImGui::Text(
                "C - start/stop camera control mode\n"
                "W, A, S, D - camera forward, left, backward, right movement\n"
                "LShift, LCtrl - camera up, down movement\n"
                "Mouse - camera rotation\n"
                "Q, E - camera tilt\n"
            );

            ImGui::EndMenu();
        }

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
    ImGui::Text("Samples: %u", this->renderer.getSampleCount());

    ImGui::EndMainMenuBar();
}

void Application::mainWindow()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize, ImGuiCond_Always);
    if (!ImGui::Begin("mainWindowMenu", nullptr, ImGuiWindowFlags_NoDecoration))
    {
        return;
    }

    this->drawingPanel();
    ImGui::SameLine();
    this->sidePanel();

    ImGui::End();
}

void Application::drawingPanel()
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
        this->viewTexture(this->rendering.isPreview ? this->renderer.getAlbedoTextureHandler() : this->renderer.getTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Albedo"))
    {
        this->viewTexture(this->renderer.getAlbedoTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Normal"))
    {
        this->viewTexture(this->renderer.getNormalTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Depth"))
    {
        this->viewTexture(this->renderer.getDepthTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Accumulator"))
    {
        this->viewTexture(this->renderer.getAccumulatorTextureHandler());
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Tonemapped"))
    {
        this->viewTexture(this->renderer.getTextureHandler());
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::EndChild();
}

void Application::sidePanel()
{
    ImGui::BeginChild("sidePanelMenu", ImVec2(0, -1));
    this->propertySelector();
    this->propertyEditor();
    ImGui::EndChild();
}

void Application::viewTexture(GLint textureHandler)
{
    ImGui::BeginChild("viewTexture");

    // Draw filled image
    glm::vec2 lo = this->renderView.uvCenter - this->renderView.uvSize * .5f;
    glm::vec2 up = this->renderView.uvCenter + this->renderView.uvSize * .5f;
    this->drawFillImage(textureHandler, this->renderer.getSize(), this->renderView.pos, this->renderView.size, glm::vec3(1), lo, up, true);

    this->renderView.isHover = ImGui::IsItemHovered();

    if (this->renderView.uvSize != glm::vec2(1))
    {
        // Draw zoom rectangle
        lo = lo * this->renderView.size + this->renderView.pos;
        up = up * this->renderView.size + this->renderView.pos;
        ImGui::GetWindowDrawList()->AddRect(toImVec2(lo), toImVec2(up), ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableBorderStrong]));
        ImGui::EndChild();
        return;
    }

    // Draw tile rectangle
    glm::uvec2 pos, size;
    this->getCurrentTile(pos, size);

    lo = (glm::vec2)pos / (glm::vec2)this->renderer.getSize();
    up = lo + (glm::vec2)size / (glm::vec2)this->renderer.getSize();
    lo.y = 1 - lo.y;
    up.y = 1 - up.y;

    lo = lo * this->renderView.size + this->renderView.pos;
    up = up * this->renderView.size + this->renderView.pos;
    ImGui::GetWindowDrawList()->AddRect(toImVec2(lo), toImVec2(up), ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableBorderLight]));

    glm::mat4 view = this->renderer.camera.createView();
    glm::mat4 projection = this->renderer.camera.createProjection(
        this->renderer.getSize().x,
        this->renderer.getSize().y,
        this->renderer.minRenderDistance,
        this->renderer.maxRenderDistance);

    // Draw gizmos
    ImGuizmo::SetRect(
        this->renderView.pos.x,
        this->renderView.pos.y,
        this->renderView.size.x,
        this->renderView.size.y);
    ImGuizmo::SetDrawlist();

    glm::vec3 snap(this->gizmo.snap);
    switch (this->property.type)
    {
        case Property::Type::MeshInstance:
        {
            MeshInstance& meshInstance = *static_cast<MeshInstance*>(this->property.target);
            if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                this->gizmo.operation,
                this->gizmo.mode,
                glm::value_ptr(meshInstance.transform),
                nullptr,
                ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ? glm::value_ptr(snap) : nullptr))
            {
                this->clear();
                this->renderer.updateSceneMeshInstances(this->scene);
            }

            break;
        }
        case Property::Type::Environment:
        {
            glm::mat4 rotation(this->renderer.environment.rotation);
            if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                ImGuizmo::OPERATION::ROTATE,
                ImGuizmo::MODE::LOCAL,
                glm::value_ptr(rotation),
                nullptr,
                ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ? glm::value_ptr(snap) : nullptr))
            {
                this->renderer.environment.rotation = rotation;
                this->clear();
            }

            break;
        }
        default:
            break;
    }

    ImGui::EndChild();
}

void Application::propertySelector()
{
    ImGui::BeginChild(
        "sceneMenu",
        ImVec2(-1, ImGui::GetMainViewport()->WorkSize.y * .75f),
        ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Renderer"))
    {
        if (ImGui::Selectable("Controls", this->property.type == Property::Type::Contorls))
        {
            this->property.type = Property::Type::Contorls;
        }

        if (ImGui::Selectable("Tone mapping", this->property.type == Property::Type::ToneMapping))
        {
            this->property.type = Property::Type::ToneMapping;
        }

        if (ImGui::Selectable("Settings", this->property.type == Property::Type::Settings))
        {
            this->property.type = Property::Type::Settings;
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Scene"))
    {
        if (ImGui::Selectable("Camera", this->property.type == Property::Type::Camera))
        {
            this->property.type = Property::Type::Camera;
        }

        if (ImGui::Selectable("Environment", this->property.type == Property::Type::Environment))
        {
            this->property.type = Property::Type::Environment;
        }

        if (ImGui::TreeNode("Meshes"))
        {
            for (size_t i = 0; i < this->scene.meshInstances.size(); i++)
            {
                MeshInstance& meshInstance = this->scene.meshInstances[i];
                std::string name = this->scene.meshNames[meshInstance.meshId] + "##" + std::to_string(i);
                if (ImGui::Selectable(name.c_str(), this->property.type == Property::Type::MeshInstance && &meshInstance == this->property.target))
                {
                    this->property.type = Property::Type::MeshInstance;
                    this->property.target = &meshInstance;
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Materials"))
        {
            for (size_t i = 0; i < this->scene.materials.size(); i++)
            {
                Material& material = this->scene.materials[i];
                std::string name = this->scene.materialNames[i] + "##" + std::to_string(i);
                if (ImGui::Selectable(name.c_str(), this->property.type == Property::Type::Material && &material == this->property.target))
                {
                    this->property.type = Property::Type::Material;
                    this->property.target = &material;
                }
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::EndChild();
}

void Application::propertyEditor()
{
    ImGui::BeginChild("propertyEditorMenu", ImVec2(-1, 0), ImGuiChildFlags_Border);

    switch (this->property.type)
    {
        case Property::Type::Contorls:
            this->propertyControls();
            break;
        case Property::Type::ToneMapping:
            this->propertyToneMapping();
            break;
        case Property::Type::Settings:
            this->propertySettings();
            break;
        case Property::Type::Camera:
            this->propertyCamera();
            break;
        case Property::Type::Environment:
            this->propertyEnvironment();
            break;
        case Property::Type::MeshInstance:
            this->propertyMeshInstance(*static_cast<MeshInstance*>(this->property.target));
            break;
        case Property::Type::Material:
            this->propertyMaterial(*static_cast<Material*>(this->property.target));
            break;
    }

    ImGui::EndChild();
}

void Application::propertyControls()
{
    if (ImGui::Button(this->rendering.enable ? "Stop" : "Start", ImVec2(-1, 0)))
    {
        this->rendering.enable = !this->rendering.enable;
    }

    if (ImGui::Button("Clear", ImVec2(-1, 0)))
    {
        this->clear();
    }

#ifdef TX_DENOISE
    if (ImGui::Button("Denoise", ImVec2(-1, 0)))
    {
        try
        {
            this->renderer.denoise();
        }
        catch (const std::runtime_error& err)
        {
            std::cerr << err.what() << std::endl;
            tinyfd_messageBox("Error", "Failed to denoise", "ok", "warning", 0);
        }
    }
#endif
}

void Application::propertyToneMapping()
{
    bool changed = false;

    if (ImGui::BeginCombo(
        "Tone mapping",
        this->renderer.toneMapMode == Renderer::ToneMapMode::Reinhard ?
        "Reinhard" :
        this->renderer.toneMapMode == Renderer::ToneMapMode::ACES ?
        "ACES" :
        "ACES fitted"))
    {
        if (ImGui::Selectable("Reinhard", this->renderer.toneMapMode == Renderer::ToneMapMode::Reinhard))
        {
            changed = true;
            this->renderer.toneMapMode = Renderer::ToneMapMode::Reinhard;
        }

        if (ImGui::Selectable("ACES", this->renderer.toneMapMode == Renderer::ToneMapMode::ACES))
        {
            changed = true;
            this->renderer.toneMapMode = Renderer::ToneMapMode::ACES;
        }

        if (ImGui::Selectable("ACES fitted", this->renderer.toneMapMode == Renderer::ToneMapMode::ACESfitted))
        {
            changed = true;
            this->renderer.toneMapMode = Renderer::ToneMapMode::ACESfitted;
        }

        ImGui::EndCombo();
    }

    changed |= ImGui::DragFloat("Gamma", &this->renderer.gamma, 0.01f, 0, 1000);

    if (changed)
    {
        this->rendering.isPreview ? this->clear() : this->renderer.toneMap();
    }
}

void Application::propertySettings()
{
    bool updated = false;

    glm::uvec2 size = this->renderer.getSize();
    if (DragUInt2("Render size", glm::value_ptr(size), 10, 1, 10000))
    {
        size = glm::max(glm::uvec2(1), size);
        this->renderer.resize(size);
        updated = true;
    }

    updated |= DragUInt("Tiling factor", &this->tiling.factor, .01f, 1, std::min(this->renderer.getSize().x, this->renderer.getSize().y)) & this->tiling.count != 0;
    Tooltip("Reduces lags but increases the rendering time, intended for large images");

    ImGui::Separator();
    DragUInt("Samples target", &this->rendering.sampleCountTarget, 1.f, 0, 100000);
    Tooltip("Zero means unlimited samples");

    updated |= DragUInt("Samples per frame", &this->rendering.samplesPerFrame, .1f, 1, 10000) & this->tiling.count != 0;
    Tooltip("A high value can cause lags but the image quality improves faster");

    updated |= DragUInt("Max bounce count", &this->renderer.maxBounceCount, .01f, 0, 1000);

    ImGui::Separator();
    updated |= ImGui::DragFloat("Min render distance", &this->renderer.minRenderDistance, 0.01f, 0, this->renderer.maxRenderDistance, "%.4f", ImGuiSliderFlags_Logarithmic) |
        ImGui::DragFloat("Max render distance", &this->renderer.maxRenderDistance, 1, this->renderer.minRenderDistance, 10000, "%.4f", ImGuiSliderFlags_Logarithmic);

    if (updated)
    {
        this->clear();
    }
}

void Application::propertyCamera()
{
    Camera& camera = this->renderer.camera;
    bool changed = false;

    changed |= ImGui::DragFloat3("Position", glm::value_ptr(camera.position), .01f);
    changed |= ImGui::DragFloat3("Forward", glm::value_ptr(camera.forward), .01f);
    changed |= ImGui::DragFloat3("Up", glm::value_ptr(camera.up), .01f);

    ImGui::Separator();
    changed |= ImGui::SliderAngle("FOV", &camera.fov, 1, 179);

    ImGui::Separator();
    changed |= ImGui::DragFloat("Focal distance", &camera.focalDistance, .001f, 0, 1000, "%.5f");
    changed |= ImGui::DragFloat("Aperture", &camera.aperture, .0001f, 0, 1000, "%.5f");

    if (ImGui::Button("Focus on look at", ImVec2(-1, 0)))
    {
        camera.focalDistance = this->getLookAtDistance();
        changed = true;
    }

    ImGui::Separator();
    changed |= ImGui::DragFloat("Blur", &camera.blur, .000001f, 0, 1000, "%.5f");
    Tooltip("Can be used for anti-aliasing");

    ImGui::Separator();
    ImGui::DragFloat(
        "Movement speed",
        &this->cameraControl.speed,
        1,
        0,
        10000,
        "%.3f",
        ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Rotation speed", &this->cameraControl.rotationSpeed, .001f, 1);

    if (!this->scene.cameras.empty())
    {
        ImGui::Separator();
        if (ImGui::TreeNode("Default cameras"))
        {
            for (size_t i = 0; i < this->scene.cameras.size(); i++)
            {
                if (ImGui::Selectable(("Camera " + std::to_string(i)).c_str()))
                {
                    camera = this->scene.cameras[i];
                    changed = true;
                }
            }

            ImGui::TreePop();
        }
    }

    if (changed)
    {
        camera.forward = glm::normalize(camera.forward);
        camera.up = glm::normalize(camera.up);
        this->clear();
    }
}

void Application::propertyEnvironment()
{
    bool changed = false;

    changed |= ImGui::DragFloat("Intensity", &this->renderer.environment.intensity, .001f, 0, 1000000);

    float translation[3];
    float rotation[3];
    float scale[3];
    glm::mat4 rotationMat(this->renderer.environment.rotation);
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(rotationMat), translation, rotation, scale);
    if (ImGui::DragFloat3("Rotation", rotation, .01f))
    {
        ImGuizmo::RecomposeMatrixFromComponents(
            translation,
            rotation,
            scale,
            glm::value_ptr(rotationMat));
        this->renderer.environment.rotation = rotationMat;
        changed = true;
    }

    ImGui::DragFloat("Snap value", &this->gizmo.snap, 1, 0.f, 1000000);
    Tooltip("Hold LCtrl for snapping");

    changed |= ImGui::Checkbox("Transparent background", &this->renderer.environment.transparent);

    if (changed)
    {
        this->clear();
    }
}

void Application::propertyMeshInstance(MeshInstance& meshInstance)
{
    if (!ImGui::BeginTabBar("propertyMeshMenu"))
    {
        return;
    }

    bool changed = false;

    if (ImGui::BeginTabItem("Info"))
    {
        ImGui::Text("%d triangles", this->scene.meshes[meshInstance.meshId].triangleSize);
        if (ImGui::Button("Focus camera", ImVec2(-1, 0)))
        {
            this->renderer.camera.lookAt(meshInstance.transform[3]);
            this->clear();
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

        ImGui::DragFloat("Snap value", &this->gizmo.snap, 1, 0.f, 1000000);
        Tooltip("Hold LCtrl for snapping");

        ImGui::RadioButton("Translate", (int*)&this->gizmo.operation, (int)ImGuizmo::OPERATION::TRANSLATE);
        ImGui::SameLine();
        ImGui::RadioButton("Rotate", (int*)&this->gizmo.operation, (int)ImGuizmo::OPERATION::ROTATE);
        ImGui::SameLine();
        ImGui::RadioButton("Scale", (int*)&this->gizmo.operation, (int)ImGuizmo::OPERATION::SCALE);

        ImGui::RadioButton("World", (int*)&this->gizmo.mode, (int)ImGuizmo::MODE::WORLD);
        ImGui::SameLine();
        ImGui::RadioButton("Local", (int*)&this->gizmo.mode, (int)ImGuizmo::MODE::LOCAL);

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Material"))
    {
        if (ImGui::BeginCombo(
            "Material",
            meshInstance.materialId == -1 ? "None" : this->scene.materialNames[meshInstance.materialId].c_str()))
        {
            for (size_t index = 0; index < this->scene.materials.size(); index++)
            {
                if (ImGui::Selectable(
                    (this->scene.materialNames[index] + "##meshMaterial" + std::to_string(index)).c_str(),
                    index == meshInstance.materialId))
                {
                    meshInstance.materialId = index;
                    changed = true;
                }
            }

            ImGui::EndCombo();
        }

        ImGui::BeginDisabled(meshInstance.materialId == -1);
        if (ImGui::Button("Edit", ImVec2(-1, 0)))
        {
            this->property.target = &this->scene.materials[meshInstance.materialId];
            this->property.type = Property::Type::Material;
        }

        ImGui::EndDisabled();

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    if (changed)
    {
        this->clear();
        this->renderer.updateSceneMeshInstances(this->scene);
    }
}

void Application::propertyMaterial(Material& material)
{
    if (!ImGui::BeginTabBar("propertyMaterialMenu"))
    {
        return;
    }

    bool changed = false;

    if (ImGui::BeginTabItem("Albedo"))
    {
        glm::vec3 albedoColor = glm::pow(material.albedoColor, glm::vec3(1 / this->renderer.gamma));
        changed |= ImGui::ColorEdit3("Color (gamma corrected)##albedo", glm::value_ptr(albedoColor), ImGuiColorEditFlags_Float);
        material.albedoColor = glm::pow(albedoColor, glm::vec3(this->renderer.gamma));

        changed |= this->materialTextureSelector("albedo", material.albedoTextureId, material.albedoColor);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Roughness"))
    {
        changed |= ImGui::SliderFloat("Strength##roughness", &material.roughness, 0, 1);
        changed |= this->materialTextureSelector("roughness", material.roughnessTextureId, glm::vec3(0, material.roughness, 0));
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Metalness"))
    {
        changed |= ImGui::SliderFloat("Strength##metalness", &material.metalness, 0, 1);
        changed |= this->materialTextureSelector("metalness", material.metalnessTextureId, glm::vec3(0, 0, material.metalness));
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Emission"))
    {
        changed |= ImGui::ColorEdit3("Color##emission", glm::value_ptr(material.emissionColor), ImGuiColorEditFlags_Float);
        changed |= ImGui::DragFloat("Strength##emission", &material.emissionStrength, .001f, 0, 10000);
        changed |= this->materialTextureSelector("emission", material.emissionTextureId, material.emissionColor * material.emissionStrength);
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
        changed |= this->materialTextureSelector("normal", material.normalTextureId, glm::vec3(1));
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
        this->clear();
        this->renderer.updateSceneMaterials(this->scene);
    }
}

bool Application::materialTextureSelector(const std::string& name, int& currentTextureId, glm::vec3 tintColor)
{
    bool changed = false;

    if (ImGui::BeginCombo(
        ("Texture##" + name).c_str(),
        currentTextureId == -1 ? "None" : this->scene.textureNames[currentTextureId].c_str()))
    {
        if (ImGui::Selectable("None", currentTextureId == -1))
        {
            currentTextureId = -1;
            changed = true;
        }

        for (size_t textureId = 0; textureId < this->scene.textures.size(); textureId++)
        {
            const Image& texture = this->scene.textures[textureId];
            const std::string& textureName = this->scene.textureNames[textureId];
            if (ImGui::Selectable(
                (textureName + "##" + name + "Texture" + std::to_string(textureId)).c_str(),
                currentTextureId == textureId))
            {
                currentTextureId = textureId;
                changed = true;
            }
        }

        ImGui::EndCombo();
    }

    if (currentTextureId != -1)
    {
        if (this->materialTexture.textureId != currentTextureId)
        {
            this->materialTexture.texture.update(this->scene.textures[currentTextureId]);
            this->materialTexture.textureId = currentTextureId;
        }

        glm::vec2 imagePos, imageSize;
        ImGui::BeginChild("viewMaterialTexture");
        this->drawFillImage(this->materialTexture.texture.getHandler(), this->materialTexture.texture.size, imagePos, imageSize, tintColor);
        ImGui::EndChild();
    }

    return changed;
}

void Application::drawFillImage(GLint textureHandler, glm::vec2 srcSize, glm::vec2& imagePos, glm::vec2& imageSize, glm::vec3 tintColor, glm::vec2 uvLo, glm::vec2 uvUp, bool flipY)
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
