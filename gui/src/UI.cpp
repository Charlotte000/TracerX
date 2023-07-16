#include "GUI/UI.h"
#include <TracerX/VectorMath.h>
#include <math.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>
#include <ImGui/imgui_stdlib.h>
#include <ImGui/imgui_UBO.h>

namespace GUI
{

void _Material(Application& app, size_t index)
{
    if (!ImGui::BeginMenu(std::string("Material " + std::to_string(index)).c_str()))
    {
        return;
    }

    TracerX::Material material = app.materials.get()[index];
    
    if (ImGui::ColorEdit3("Albedo", (float*)&material.albedoColor, ImGuiColorEditFlags_Float))
    {
        app.materials.set(index, material);
        app.reset();
    }

    if (ImGui::BeginMenu("Albedo map"))
    {
        if (ImGui::SliderInt("Albedo map id", &material.albedoMapId, -1, (int)app.textures.size() - 1))
        {
            app.materials.set(index, material);
            app.reset();
        }

        if (material.albedoMapId >= 0)
        {
            ImGui::Image(app.textures[material.albedoMapId], sf::Vector2f(100, 100));
        }

        ImGui::EndMenu();
    }

    if (ImGui::SliderFloat("Roughness", &material.roughness, 0, 1))
    {
        app.materials.set(index, material);
        app.reset();
    }

    if (ImGui::BeginMenu("Metalness"))
    {
        if (ImGui::ColorEdit3("Metalness color", (float*)&material.metalnessColor, ImGuiColorEditFlags_Float) |
            ImGui::SliderFloat("Metalness", &material.metalness, 0, 1))
        {
            app.materials.set(index, material);
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Emission"))
    {
        if (ImGui::ColorEdit3("Emission color", (float*)&material.emissionColor, ImGuiColorEditFlags_Float) |
            ImGui::DragFloat("Emission", &material.emissionStrength, .001f, 0, 100))
        {
            app.materials.set(index, material);
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Fresnel"))
    {
        if (ImGui::ColorEdit3("Fresnel color", (float*)&material.fresnelColor, ImGuiColorEditFlags_Float) |
            ImGui::DragFloat("Fresnel", &material.fresnelStrength, .0001f, 0, 100))
        {
            app.materials.set(index, material);
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::DragFloat("Refraction", &material.refractionFactor, .001f, 0, 100) |
        ImGui::DragFloat("Density", &material.density, .001f, 0, 100))
    {
        app.materials.set(index, material);
        app.reset();
    }

    if (ImGui::Button("Delete"))
    {
        app.materials.remove(index);
        app.reset();
    }

    ImGui::EndMenu();
}

void _Sphere(Application& app, size_t index)
{
    if (!ImGui::BeginMenu(std::string("Sphere " + std::to_string(index)).c_str()))
    {
        return;
    }

    TracerX::Sphere sphere = app.spheres.get()[index];

    if (ImGui::DragFloat3("Origin", (float*)&sphere.origin, .001f) |
        ImGui::DragFloat("Radius", &sphere.radius, .001f) |
        ImGui::DragFloat3("Rotate", (float*)&sphere.rotation, .01f))
    {
        app.spheres.set(index, sphere);
        app.reset();
    }

    if (ImGui::BeginListBox("Material id"))
    {
        for (int materialId = 0; materialId < app.materials.get().size(); materialId++)
        {
            if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == sphere.materialId) &&
                materialId != sphere.materialId)
            {
                sphere.materialId = materialId;
                app.spheres.set(index, sphere);
                app.reset();
            }
        }

        ImGui::EndListBox();
    }

    if (ImGui::Button("Focus camera"))
    {
        app.camera.lookAt(sphere.origin);
        app.reset();
    }

    if (ImGui::Button("Delete"))
    {
        app.spheres.remove(index);
        app.reset();
    }

    ImGui::EndMenu();
}

void _Box(Application& app, size_t index)
{
    if (!ImGui::BeginMenu(std::string("Box " + std::to_string(index)).c_str()))
    {
        return;
    }

    TracerX::Box box = app.boxes.get()[index];

    ImGui::Text("Bounding box min: (%f, %f, %f)", box.boxMin.x, box.boxMin.y, box.boxMin.z);
    ImGui::Text("Bounding box max: (%f, %f, %f)", box.boxMax.x, box.boxMax.y, box.boxMax.z);

    if (ImGui::DragFloat3("Origin", (float*)&box.origin, .001f) |
        ImGui::DragFloat3("Size", (float*)&box.size, .001f) |
        ImGui::DragFloat3("Rotate", (float*)&box.rotation, .01f))
    {
        app.boxes.set(index, box);
        app.reset();
    }

    if (ImGui::BeginListBox("Material id"))
    {
        for (int materialId = 0; materialId < app.materials.get().size(); materialId++)
        {
            if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == box.materialId) &&
                materialId != box.materialId)
            {
                box.materialId = materialId;
                app.boxes.set(index, box);
                app.reset();
            }
        }
    
        ImGui::EndListBox();
    }

    if (ImGui::Button("Focus camera"))
    {
        app.camera.lookAt(box.origin);
        app.reset();
    }

    if (ImGui::Button("Delete"))
    {
        app.boxes.remove(index);
        app.reset();
    }

    ImGui::EndMenu();
}

void _Mesh(Application& app, size_t index)
{
    if (!ImGui::BeginMenu(std::string("Mesh " + std::to_string(index)).c_str()))
    {
        return;
    }

    TracerX::Mesh mesh = app.meshes.get()[index];

    ImGui::Text("Indices start: %i", mesh.indicesStart);
    ImGui::Text("Indices end: %i", mesh.indicesEnd);
    ImGui::Text("Bounding box min: (%f, %f, %f)", mesh.boxMin.x, mesh.boxMin.y, mesh.boxMin.z);
    ImGui::Text("Bounding box max: (%f, %f, %f)", mesh.boxMax.x, mesh.boxMax.y, mesh.boxMax.z);

    sf::Vector3f offset = mesh.position;
    if (ImGui::DragFloat3("Offset", (float*)&offset, .01f))
    {
        mesh.offset(offset - mesh.position, app.indices, app.vertices);
        app.meshes.set(index, mesh);
        app.reset();
    }

    sf::Vector3f scale = mesh.size;
    if (ImGui::DragFloat3("Scale", (float*)&scale, .01f) && scale.x != 0 && scale.y != 0 && scale.z != 0)
    {
        mesh.scale(TracerX::div(scale, mesh.size), app.indices, app.vertices);
        app.meshes.set(index, mesh);
        app.reset();
    }

    sf::Vector3f rotation = mesh.rotation;
    if (ImGui::DragFloat3("Rotate", (float*)&rotation, .01f))
    {
        mesh.rotate(rotation - mesh.rotation, app.indices, app.vertices);
        app.meshes.set(index, mesh);
        app.reset();
    }

    if (ImGui::BeginListBox("Material id"))
    {
        for (int materialId = 0; materialId < app.materials.get().size(); materialId++)
        {
            if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == mesh.materialId) &&
                materialId != mesh.materialId)
            {
                mesh.materialId = materialId;
                app.meshes.set(index, mesh);
                app.reset();
            }
        }

        ImGui::EndListBox();
    }

    if (ImGui::Button("Focus camera"))
    {
        app.camera.lookAt(mesh.position);
        app.reset();
    }

    ImGui::EndMenu();
}


void UI(Application& app)
{
    // Draw UI
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Info"))
    {
        InfoUI(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Material"))
    {
        MaterialUI(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Geometry"))
    {
        GeometryUI(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Environment"))
    {
        EnvironmentUI(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Texture"))
    {
        TextureUI(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Animation"))
    {
        AnimationUI(app);
        ImGui::EndMenu();
    }

    ImGui::Separator();

    ImGui::Text("FPS: %-4i", (int)roundf(ImGui::GetIO().Framerate));
    ImGui::Text("Frame: %-4i", app.frameCount.get());
    ImGui::ProgressBar((float)app.subStage / app.subDivisor.x / app.subDivisor.y);
    ImGui::EndMainMenuBar();
}

void InfoUI(Application& app)
{
    ImGui::Text("Window size: %dx%d", (int)app.size.get().x, (int)app.size.get().y);
    if (ImGui::BeginMenu("Camera"))
    {
        if (ImGui::DragVector3fUBO("Position", app.camera.position, .01f) |
            ImGui::DragVector3fUBONormalize("Forward", app.camera.forward, .01f) |
            ImGui::DragVector3fUBONormalize("Up", app.camera.up, .01f) |
            ImGui::DragFloatUBO("Focal length", app.camera.focalLength, .001f, 0, 1000) |
            ImGui::DragFloatUBO("Focus strength", app.camera.focusStrength, .0001f, 0, 1000) |
            ImGui::SliderAngleUBO("FOV", app.camera.fov, 0, 180))
        {
            app.prevCamera.position.set(app.camera.position.get());
            app.prevCamera.forward.set(app.camera.forward.get());
            app.prevCamera.up.set(app.camera.up.get());
            app.prevCamera.focalLength.set(app.camera.focalLength.get());
            app.prevCamera.focusStrength.set(app.camera.focusStrength.get());
            app.prevCamera.fov.set(app.camera.fov.get());
            app.reset();
        }

        if (ImGui::BeginMenu("Motion blur"))
        {
            if (ImGui::DragVector3fUBO("Prev position", app.prevCamera.position, .01f) |
                ImGui::DragVector3fUBONormalize("Prev Forward", app.prevCamera.forward, .01f) |
                ImGui::DragVector3fUBONormalize("Prev Up", app.prevCamera.up, .01f) |
                ImGui::DragFloatUBO("Prev Focal length", app.prevCamera.focalLength, .001f, 0, 1000) |
                ImGui::DragFloatUBO("Prev Focus strength", app.prevCamera.focusStrength, .0001f, 0, 1000) |
                ImGui::SliderAngleUBO("Prev FOV", app.prevCamera.fov, 0, 180))
            {
                app.reset();            
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
    
    if (ImGui::DragIntUBO("Samples", app.sampleCount, 0.01f, 0, 10000) |
        ImGui::DragIntUBO("Max bounce", app.maxBounceCount, 0.01f, 0, 10000))
    {
        app.reset();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::SliderInt("Width subdivise", &app.subDivisor.x, 1, app.size.get().x, "%d", ImGuiSliderFlags_Logarithmic) |
        ImGui::SliderInt("Height subdivise", &app.subDivisor.y, 1, app.size.get().y, "%d", ImGuiSliderFlags_Logarithmic))
    {
        app.reset();
    }

    ImGui::Checkbox("Show cursor", &app.showCursor);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static int totalPixels = app.size.get().x * app.size.get().y;
    static int pixelDifference = totalPixels;
    static float differencePercentage = 100;
    if (ImGui::Button("Update pixel difference"))
    {
        pixelDifference = app.getPixelDifference();
        differencePercentage = 100.0f * pixelDifference / totalPixels;
    }
    
    ImGui::Text("Difference: %7dpx / %7dpx | %.2f %%", pixelDifference, totalPixels, differencePercentage);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Checkbox("Camera control", &app.isCameraControl))
    {
        sf::Mouse::setPosition((sf::Vector2i)app.size.get() / 2, app.window);
        app.window.setMouseCursorVisible(false);
        app.reset();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Checkbox("Render", &app.isProgressive))
    {
        app.subStage = 0;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static std::string fileName;
    ImGui::InputText("Filename", &fileName);
    if (ImGui::Button("Save"))
    {
        app.saveToFile(fileName);
    }
}

void MaterialUI(Application& app)
{
    for (int i = 0; i < app.materials.get().size(); i++)
    {
        _Material(app, i);
    }

    if (ImGui::Button("Create"))
    {
        app.materials.add(TracerX::Material());
        app.reset();
    }
}

void GeometryUI(Application& app)
{
    if (ImGui::BeginMenu(std::string("Spheres (" + std::to_string(app.spheres.get().size()) + ')').c_str()))
    {
        for (int i = 0; i < app.spheres.get().size(); i++)
        {
            _Sphere(app, i);
        }
        
        if (ImGui::Button("Create"))
        {
            app.spheres.add(TracerX::Sphere(sf::Vector3f(0, 0, 0), 0, 0));
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(std::string("Boxes (" + std::to_string(app.boxes.get().size()) + ')').c_str()))
    {
        for (int i = 0; i < app.boxes.get().size(); i++)
        {
            _Box(app, i);
        }

        if (ImGui::Button("Create"))
        {
            app.boxes.add(TracerX::Box(sf::Vector3f(0, 0, 0), sf::Vector3f(0, 0, 0), 0));
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(std::string("Meshes (" + std::to_string(app.meshes.get().size()) + ')').c_str()))
    {
        for (int i = 0; i < app.meshes.get().size(); i++)
        {
            _Mesh(app, i);
        }

        if (ImGui::Button("Create"))
        {
            ImGui::OpenPopup("FileSelect");
        }

        if (ImGui::BeginPopupContextWindow("FileSelect"))
        {
            static std::string filePath;
            ImGui::InputText("File path", &filePath);
            if (ImGui::Button("Load"))
            {
                try
                {
                    app.addFile(filePath);
                    app.reset();
                }
                catch (std::runtime_error&) { }

                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }

        ImGui::EndMenu();
    }
}

void EnvironmentUI(Application& app)
{
    if (ImGui::BeginMenu("Sun"))
    {
        if (ImGui::ColorEdit3UBO("Color", app.environment.sunColor, ImGuiColorEditFlags_Float))
        {
            app.reset();
        }

        if (ImGui::DragVector3fUBONormalize("Direction", app.environment.sunDirection, 0.001f))
        {
            app.reset();
        }

        if (ImGui::DragFloatUBO("Focus", app.environment.sunFocus, 1, 0, 10000))
        {
            app.reset();
        }

        if (ImGui::DragFloatUBO("Intensity", app.environment.sunIntensity, 0.001f, 0, 100000))
        {
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Sky"))
    {
        if (ImGui::ColorEdit3UBO("Horizon", app.environment.skyColorHorizon, ImGuiColorEditFlags_Float))
        {
            app.reset();
        }

        if (ImGui::ColorEdit3UBO("Zenith", app.environment.skyColorZenith, ImGuiColorEditFlags_Float))
        {
            app.reset();
        }

        if (ImGui::DragFloatUBO("Intensity", app.environment.skyIntensity, 0.0001f, 0, 2))
        {
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::ColorEdit3UBO("Ground", app.environment.groundColor, ImGuiColorEditFlags_Float))
    {
        app.reset();
    }

    if (ImGui::CheckboxUBO("Enable", app.environment.enabled))
    {
        app.reset();
    }
}

void TextureUI(Application& app)
{
    for (int i = 0; i < app.textures.size(); i++)
    {
        if (ImGui::BeginMenu(std::string("Texture " + std::to_string(i)).c_str()))
        {
            ImGui::Image(app.textures[i], sf::Vector2f(100, 100));

            if (ImGui::Button("Delete"))
            {
                app.textures.erase(app.textures.begin() + i);
                app.updateTextures();
            }

            ImGui::EndMenu();
        }
    }

    if (ImGui::Button("Create"))
    {
        ImGui::OpenPopup("TextureSelect");
    }

    if (ImGui::BeginPopupContextWindow("TextureSelect"))
    {
        static std::string filePath;
        ImGui::InputText("File path", &filePath);
        if (ImGui::Button("Load"))
        {
            try
            {
                app.addTexture(filePath);
                app.updateTextures();
                app.reset();
            }
            catch (std::runtime_error&) { }

            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void AnimationUI(Application& app)
{
    if (!app.animation.hasLoaded())
    {
        static std::string filePath;
        static std::string name;
        ImGui::DragFloat("FPS", &app.animation.fps, 1.f, 0.f, 10000.f, "%g");
        ImGui::DragFloat("Duration", &app.animation.duration, 1.f, 0.f, 10000.f, "%gs");
        ImGui::DragInt("Frame iteration", &app.animation.frameIteration, 1.f, 0, 10000);
        ImGui::InputText("File path", &filePath);
        ImGui::InputText("Name", &name);

        if (ImGui::Button("Load animation"))
        {
            try
            {
                app.animation.load(filePath, name, app.prevCamera, app.camera);
            }
            catch (std::runtime_error&) { }
        }

        return;
    }

    ImGui::Text("FPS: %g", app.animation.fps);
    ImGui::Text("Duration: %g", app.animation.duration);
    ImGui::Text("Frame iteration: %d", app.animation.frameIteration);

    int currentFrame = app.animation.currentFrame + 1;
    if (ImGui::SliderInt("Current frame", &currentFrame, 1, app.animation.totalFrames))
    {
        app.animation.currentFrame = currentFrame - 2;
        app.animation.getNextFrame(app.prevCamera, app.camera);
    }
}

}