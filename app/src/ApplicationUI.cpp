#include "Application.h"

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

void Application::initUI()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init();

    SetupImGuiStyle();

    this->textureView.init(GL_RGBA32F);
    this->textureView.update(Image::empty);
}

void Application::shutdownUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    this->textureView.shutdown();
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
        if (ImGui::MenuItem("Open scene"))
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
                    this->currentTextureId = -1;
                    if (this->edit != nullptr)
                    {
                        this->edit = nullptr;
                        this->property = PropertyOption::PContorls;
                    }
                }
                catch (const std::runtime_error&)
                {
                    tinyfd_messageBox("Error", "Invalid scene file", "ok", "warning", 0);
                }
            }
        }

        if (ImGui::MenuItem("Open environment"))
        {
            const char* patterns[] = { "*.png", "*.hdr", ".jpg" };
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
                    this->renderer.clear();
                }
                catch (const std::runtime_error&)
                {
                    tinyfd_messageBox("Error", "Invalid environment file", "ok", "warning", 0);
                }
            }
        }

        if (ImGui::MenuItem("Save (as png)"))
        {
            const char* patterns[] = { "*.png" };
            const char* fileName = tinyfd_saveFileDialog("Save image", nullptr, 1, patterns, nullptr);
            if (fileName != nullptr)
            {
                this->renderer.getImage().saveToFile(fileName);
            }
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

    if (ImGui::BeginTabItem("Output"))
    {
        this->viewTexture(
            !this->enablePreview || this->enableRendering || this->renderer.getSampleCount() > 1 ?
            this->renderer.getTextureHandler() : this->renderer.getAlbedoTextureHandler());
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

    glm::vec2 imagePos, imageSize;
    this->drawFillImage(textureHandler, this->renderer.getSize(), glm::vec3(1, 1, 1), true, imagePos, imageSize);

    glm::mat4 view = this->renderer.camera.createView();
    glm::mat4 projection = this->renderer.camera.createProjection(
        this->renderer.getSize().x,
        this->renderer.getSize().y,
        this->renderer.minRenderDistance,
        this->renderer.maxRenderDistance);

    ImGuizmo::SetRect(
        imagePos.x + ImGui::GetWindowPos().x,
        imagePos.y + ImGui::GetWindowPos().y,
        imageSize.x,
        imageSize.y);
    ImGuizmo::SetDrawlist();

    switch (this->property)
    {
        case PropertyOption::PMeshInstance:
        {
            MeshInstance& meshInstance = *static_cast<MeshInstance*>(this->edit);
            if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                this->operation,
                this->mode,
                glm::value_ptr(meshInstance.transform)))
            {
                meshInstance.transformInv = glm::inverse(meshInstance.transform);
                this->renderer.clear();
                this->renderer.updateSceneMeshInstances(this->scene);
            }

            break;
        }
        case PropertyOption::PEnvironment:
        {
            glm::mat4 rotation(this->renderer.environment.rotation);
            if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                ImGuizmo::OPERATION::ROTATE,
                ImGuizmo::MODE::WORLD,
                glm::value_ptr(rotation)))
            {
                this->renderer.environment.rotation = rotation;
                this->renderer.clear();
            }

            break;
        }
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
        if (ImGui::Selectable("Controls", this->property == PropertyOption::PContorls))
        {
            this->property = PropertyOption::PContorls;
        }

        if (ImGui::Selectable("Tone mapping", this->property == PropertyOption::PToneMapping))
        {
            this->property = PropertyOption::PToneMapping;
        }

        if (ImGui::Selectable("Settings", this->property == PropertyOption::PSettings))
        {
            this->property = PropertyOption::PSettings;
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Scene"))
    {
        if (ImGui::Selectable("Camera", this->property == PropertyOption::PCamera))
        {
            this->property = PropertyOption::PCamera;
        }

        if (ImGui::Selectable("Environment", this->property == PropertyOption::PEnvironment))
        {
            this->property = PropertyOption::PEnvironment;
        }

        if (ImGui::TreeNode("Meshes"))
        {
            for (size_t i = 0; i < this->scene.meshInstances.size(); i++)
            {
                MeshInstance& meshInstance = this->scene.meshInstances[i];
                std::string name = this->scene.meshNames[meshInstance.meshId] + "##" + std::to_string(i);
                if (ImGui::Selectable(name.c_str(), this->property == PropertyOption::PMeshInstance && &meshInstance == this->edit))
                {
                    this->property = PropertyOption::PMeshInstance;
                    this->edit = &meshInstance;
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
                if (ImGui::Selectable(name.c_str(), this->property == PropertyOption::PMaterial && &material == this->edit))
                {
                    this->property = PropertyOption::PMaterial;
                    this->edit = &material;
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

    switch (this->property)
    {
        case PropertyOption::PContorls:
            this->propertyControls();
            break;
        case PropertyOption::PToneMapping:
            this->propertyToneMapping();
            break;
        case PropertyOption::PSettings:
            this->propertySettings();
            break;
        case PropertyOption::PCamera:
            this->propertyCamera();
            break;
        case PropertyOption::PEnvironment:
            this->propertyEnvironment();
            break;
        case PropertyOption::PMeshInstance:
            this->propertyMeshInstance(*static_cast<MeshInstance*>(this->edit));
            break;
        case PropertyOption::PMaterial:
            this->propertyMaterial(*static_cast<Material*>(this->edit));
            break;
    }

    ImGui::EndChild();
}

void Application::propertyControls()
{
    if (ImGui::Button(this->enableRendering ? "Stop" : "Start", ImVec2(-1, 0)))
    {
        this->enableRendering = !this->enableRendering;
    }

    if (ImGui::Button("Clear", ImVec2(-1, 0)))
    {
        this->renderer.clear();
    }

#ifdef TX_DENOISE
    if (ImGui::Button("Denoise", ImVec2(-1, 0)))
    {
        this->renderer.denoise();
    }
#endif

    if (ImGui::Checkbox("Enable preview", &this->enablePreview) & this->renderer.getSampleCount() == 1)
    {
        this->renderer.clear();
    }
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
        this->renderer.getSampleCount() == 1 ? this->renderer.clear() : this->renderer.toneMap();
    }
}

void Application::propertySettings()
{
    glm::ivec2 size = this->renderer.getSize();
    if (ImGui::DragInt2("Render size", glm::value_ptr(size), 10, 1, 10000))
    {
        this->renderer.resize(size);
    }

    int maxBounceCount = this->renderer.maxBounceCount;
    if (ImGui::DragInt("Max bounce count", &maxBounceCount, .01f, 0, 1000))
    {
        this->renderer.maxBounceCount = maxBounceCount;
        this->renderer.clear();
    }

    int samplesPerFrame = this->samplesPerFrame;
    if (ImGui::DragInt("Samples per frame", &samplesPerFrame, .1f, 1, 10000))
    {
        this->samplesPerFrame = samplesPerFrame;
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text("A high value can cause lags but the image quality improves faster");
        ImGui::EndTooltip();
    }

    if (ImGui::DragFloat("Min render distance", &this->renderer.minRenderDistance, 0.01f, 0, this->renderer.maxRenderDistance, "%.4f") |
        ImGui::DragFloat("Max render distance", &this->renderer.maxRenderDistance, 10, this->renderer.minRenderDistance, 10000))
    {
        this->renderer.clear();
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

    changed |= ImGui::SliderAngle("FOV", &camera.fov, 1, 180);
    ImGui::Separator();

    changed |= ImGui::DragFloat("Focal distance", &camera.focalDistance, .001f, 0, 1000);
    changed |= ImGui::DragFloat("Aperture", &camera.aperture, .0001f, 0, 1000);
    ImGui::Separator();

    changed |= ImGui::DragFloat("Blur", &camera.blur, .000001f, 0, 1000, "%.5f");
    ImGui::Separator();

    ImGui::DragFloat(
        "Movement speed",
        &this->cameraSpeed,
        1,
        0,
        10000,
        "%.3f",
        ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Rotation speed", &this->cameraRotationSpeed, .001f, 1);

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
        this->renderer.clear();
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

    changed |= ImGui::Checkbox("Transparent background", &this->renderer.environment.transparent);

    if (changed)
    {
        this->renderer.clear();
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
            this->renderer.clear();
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
            meshInstance.transformInv = glm::inverse(meshInstance.transform);
            changed = true;
        }

        ImGui::RadioButton("Translate", (int*)&this->operation, (int)ImGuizmo::OPERATION::TRANSLATE);
        ImGui::SameLine();
        ImGui::RadioButton("Rotate", (int*)&this->operation, (int)ImGuizmo::OPERATION::ROTATE);
        ImGui::SameLine();
        ImGui::RadioButton("Scale", (int*)&this->operation, (int)ImGuizmo::OPERATION::SCALE);

        ImGui::RadioButton("World", (int*)&this->mode, (int)ImGuizmo::MODE::WORLD);
        ImGui::SameLine();
        ImGui::RadioButton("Local", (int*)&this->mode, (int)ImGuizmo::MODE::LOCAL);

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
            this->edit = &this->scene.materials[meshInstance.materialId];
            this->property = PropertyOption::PMaterial;
        }

        ImGui::EndDisabled();

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    if (changed)
    {
        this->renderer.clear();
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
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    if (changed)
    {
        this->renderer.clear();
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
        if (this->currentTextureId != currentTextureId)
        {
            this->textureView.update(this->scene.textures[currentTextureId]);
            this->currentTextureId = currentTextureId;
        }

        glm::vec2 imagePos, imageSize;
        ImGui::BeginChild("viewMaterialTexture");
        this->drawFillImage(this->textureView.getHandler(), this->textureView.size, tintColor, false, imagePos, imageSize);
        ImGui::EndChild();
    }

    return changed;
}

void Application::drawFillImage(GLint textureHandler, glm::vec2 srcSize, glm::vec3 tintColor, bool flipY, glm::vec2& imagePos, glm::vec2& imageSize)
{
    float srcAspectRatio = srcSize.x / srcSize.y;

    glm::vec2 dstSize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    float dstAspectRatio = dstSize.x / dstSize.y;

    imageSize = dstSize * (srcAspectRatio > dstAspectRatio ? glm::vec2(1, dstAspectRatio / srcAspectRatio) : glm::vec2(srcAspectRatio / dstAspectRatio, 1));
    imagePos = (glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y) - imageSize) * 0.5f;

    ImGui::SetCursorPos(ImVec2(imagePos.x, imagePos.y));
    ImGui::Image(
        (void*)(intptr_t)textureHandler,
        ImVec2(imageSize.x - 2, imageSize.y - 2),
        flipY ? ImVec2(0, 1) : ImVec2(0, 0),
        flipY ? ImVec2(1, 0) : ImVec2(1, 1),
        ImVec4(tintColor.r, tintColor.g, tintColor.b, 1),
        ImGui::GetStyle().Colors[ImGuiCol_TableBorderLight]);
}
