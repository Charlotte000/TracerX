#include "TracerX/UI.h"
#include "TracerX/Application.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/polar_coordinates.hpp>


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
    ImGuizmo::SetRect(0, 0, this->app->size.x, this->app->size.y);

    this->mainMenu();
    this->transform();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::mainMenu()
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    Renderer& renderer = this->app->renderer;

    this->info();
    this->camera();
    this->materials();
    this->meshes();
    this->scene();
    this->environment();

    ImGui::Separator();
    if (ImGui::Button("Save"))
    {
        this->app->save();
    }

    ImGui::Separator();
    if (ImGui::Button("Denoise"))
    {
        renderer.denoise();
    }

    float elapsedTime = ImGui::GetIO().DeltaTime;
    ImGui::Separator();
    ImGui::Text("%.0fms", 1000 * elapsedTime);

    ImGui::Separator();
    ImGui::Text("%.0ffps", 1 / elapsedTime);

    ImGui::Separator();
    ImGui::Text("Frame count: %d", renderer.frameCount - 1);

    ImGui::EndMainMenuBar();
}

void UI::info()
{
    if (!ImGui::BeginMenu("Info"))
    {
        return;
    }

    Renderer& renderer = this->app->renderer;

    if (ImGui::DragInt2("Window size", glm::value_ptr(this->app->size), 10.f, 1, 10000))
    {
        this->app->resize(this->app->size);
    }

    ImGui::DragFloat("Gamma", &renderer.gamma, 0.01f, 0.f, 1000.f);
    if (ImGui::DragInt("Max bouce count", &renderer.maxBouceCount, .01f, 0, 1000))
    {
        renderer.resetAccumulator();
    }

    ImGui::DragFloat("Camera speed", &this->app->cameraSpeed, 1.f, 1.f, 10000.f);

    ImGui::EndMenu();
}

void UI::camera()
{
    if (!ImGui::BeginMenu("Camera"))
    {
        return;
    }

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

    ImGui::EndMenu();
}

void UI::materials()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    if (!ImGui::BeginMenu("Materials", scene.materials.size() != 0))
    {
        return;
    }

    for (size_t i = 0; i < scene.materials.size(); i++)
    {
        Material& material = scene.materials[i];
        std::string name = scene.materialNames[i] + "##" + std::to_string(i);
        if (!ImGui::BeginMenu(name.c_str()))
        {
            continue;
        }

        ImGui::SeparatorText("Albedo");
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

        ImGui::SeparatorText("Roughness");
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

        ImGui::SeparatorText("Metalness");
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

        ImGui::SeparatorText("Emission");
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

        ImGui::SeparatorText("Fresnel");
        if (ImGui::ColorEdit3("Color##fresnel", glm::value_ptr(material.fresnelColor), ImGuiColorEditFlags_Float) |
            ImGui::DragFloat("Strength##fresnel", &material.fresnelStrength, .0001f, 0.f, 100.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        ImGui::SeparatorText("Normal");
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

        ImGui::Separator();
        if (ImGui::DragFloat("Density", &material.density, .001f, 0.f, 100.f) |
            ImGui::DragFloat("IOR", &material.ior, .001f, 0.f, 100.f))
        {
            renderer.resetAccumulator();
            renderer.resetMaterials(scene.materials);
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenu();
}

void UI::meshes()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    if (!ImGui::BeginMenu("Meshes", scene.meshes.size() != 0))
    {
        return;
    }

    for (size_t i = 0; i < scene.meshes.size(); i++)
    {
        Mesh& mesh = scene.meshes[i];
        std::string name = scene.meshNames[i] + "##" + std::to_string(i);
        if (!ImGui::BeginMenu(name.c_str()))
        {
            continue;
        }

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

        if (ImGui::Button("Transform", ImVec2(-1, 0)))
        {
            this->editMesh = &mesh;
            this->editMeshTransform = this->editMesh->transform;
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenu();
}

void UI::environment()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    if (!ImGui::BeginMenu("Environment"))
    {
        return;
    }

    if (ImGui::DragFloat("Intensity", &renderer.environmentIntensity, .001f, .0f, 1000000.f))
    {
        renderer.resetAccumulator();
    }

    if (ImGui::BeginCombo("##environment", scene.environment.name.c_str()))
    {
        if (ImGui::Selectable("None", scene.environment.name == "None"))
        {
            scene.environment = Image::empty;
            renderer.resetAccumulator();
            renderer.resetEnvironment(scene.environment);
        }

        for (size_t i = 0; i < this->app->environmentFiles.size(); i++)
        {
            const std::string& environmentName = this->app->environmentFiles[i];
            if (ImGui::Selectable((environmentName + "##environmentTexture" + std::to_string(i)).c_str(), environmentName == scene.environment.name))
            {
                scene.loadEnvironmentMap(Application::environmentFolder + environmentName);
                renderer.resetAccumulator();
                renderer.resetEnvironment(scene.environment);
            }
        }

        ImGui::EndCombo();
    }

    ImGui::EndMenu();
}

void UI::scene()
{
    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    if (!ImGui::BeginMenu("Scene", this->app->sceneFiles.size() != 0))
    {
        return;
    }

    if (ImGui::BeginCombo("##scene", this->app->scene.name.c_str()))
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

void UI::transform()
{
    if (this->editMesh == nullptr)
    {
        return;
    }

    bool is_open = true;
    ImGui::Begin("Transform", &is_open);
    if (!is_open)
    {
        this->editMesh = nullptr;
        ImGui::End();
        return;
    }

    Renderer& renderer = this->app->renderer;
    Scene& scene = this->app->scene;

    static float translation[3];
    static float rotation[3];
    static float scale[3];
    ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr(this->editMeshTransform), translation, rotation, scale);
    ImGui::DragFloat3("Translation", translation, .01f);
    ImGui::DragFloat3("Rotation", rotation, .01f);
    ImGui::DragFloat3("Scale", scale, .01f, 0.f, 1000.f);

    ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale,  glm::value_ptr(this->editMeshTransform));

    static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    static ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
    ImGui::RadioButton("Translate", (int*)&operation, (int)ImGuizmo::OPERATION::TRANSLATE);
    ImGui::SameLine();
    ImGui::RadioButton("Rotate", (int*)&operation, (int)ImGuizmo::OPERATION::ROTATE);
    ImGui::SameLine();
    ImGui::RadioButton("Scale", (int*)&operation, (int)ImGuizmo::OPERATION::SCALE);

    ImGui::RadioButton("World", (int*)&mode, (int)ImGuizmo::MODE::WORLD);
    ImGui::SameLine();
    ImGui::RadioButton("Local", (int*)&mode, (int)ImGuizmo::MODE::LOCAL);

    static bool showGrid = true;
    static bool showBox = true;
    ImGui::Checkbox("Show grid", &showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Show box", &showBox);

    glm::mat4 view = renderer.camera.createView();
    glm::mat4 projection = renderer.camera.createProjection();

    if (showBox)
    {
        ImGuizmo::DrawCubes(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(this->editMeshTransform), 1);
    }

    if (showGrid)
    {
        static glm::mat4 grid(1);
        ImGuizmo::DrawGrid(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(grid), 10.f);
    }

    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, mode, glm::value_ptr(this->editMeshTransform));

    if (ImGui::Button("Apply", ImVec2(-1, 0)))
    {
        this->editMesh->transform = this->editMeshTransform;
        const std::vector<glm::vec3>& bvh = scene.createBVH();

        renderer.resetAccumulator();
        renderer.resetMeshes(scene.meshes);
        renderer.resetBVH(bvh, scene.triangles);
    }
    
    ImGui::End();
}
