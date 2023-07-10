#include "GUI/UI.h"
#include <TracerX/VectorMath.h>
#include <math.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>
#include <ImGui/imgui_stdlib.h>

namespace GUI
{

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

    ImGui::Separator();

    ImGui::Text("FPS: %-4i", (int)roundf(ImGui::GetIO().Framerate));
    ImGui::Text("Frame: %-4i", app.frameCount);
    ImGui::ProgressBar((float)app.subStage / app.subDivisor.x / app.subDivisor.y);
    ImGui::EndMainMenuBar();
}

void InfoUI(Application& app)
{
    ImGui::Text("Window size: %dx%d", app.size.x, app.size.y);
    if (ImGui::BeginMenu("Camera"))
    {
        if (ImGui::SliderAngle("FOV", &app.camera.fov, 0, 180.0f))
        {
            app.reset();
        }

        if (ImGui::DragFloat3("Position", (float*)&app.camera.position, .01f))
        {
            app.reset();
        }

        if (ImGui::DragFloat3("Forward", (float*)&app.camera.forward, .01f))
        {
            app.camera.forward = TracerX::normalized(app.camera.forward);
            app.reset();
        }

        if (ImGui::DragFloat3("Up", (float*)&app.camera.up, .01f))
        {
            app.camera.up = TracerX::normalized(app.camera.up);
            app.reset();
        }

        if (ImGui::DragFloat("Focal length", &app.camera.focalLength, 0.001f, 0, 1000))
        {
            app.reset();
        }

        if (ImGui::DragFloat("Focal strength", &app.camera.focusStrength, 0.0001f, 0, 1000))
        {
            app.reset();
        }

        ImGui::EndMenu();
    }
    
    if (ImGui::DragInt("Samples", &app.sampleCount, 0.01f, 0, 10000))
    {
        app.shader.setUniform("SampleCount", app.sampleCount);
        app.reset();
    }

    if (ImGui::DragInt("Max bounce", &app.maxBounceCount, 0.01f, 0, 10000))
    {
        app.shader.setUniform("MaxBouceCount", app.maxBounceCount);
        app.reset();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::SliderInt("Width subdivise", &app.subDivisor.x, 1, app.size.x, "%d", ImGuiSliderFlags_Logarithmic))
    {
        app.reset();
    }

    if (ImGui::SliderInt("Height subdivise", &app.subDivisor.y, 1, app.size.y, "%d", ImGuiSliderFlags_Logarithmic))
    {
        app.reset();
    }

    ImGui::Checkbox("Show cursor", &app.showCursor);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static int totalPixels = app.size.x * app.size.y;
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
        sf::Mouse::setPosition(app.size / 2, app.window);
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
        app.targetTexture->getTexture().copyToImage().saveToFile(fileName);
    }
}

void MaterialUI(Application& app)
{
    for (int i = 0; i < app.materials.size(); i++)
    {
        if (ImGui::BeginMenu(std::string("Material " + std::to_string(i)).c_str()))
        {
            TracerX::Material& material = app.materials[i];
            
            if (ImGui::ColorEdit3("Albedo", (float*)&material.albedoColor, ImGuiColorEditFlags_Float))
            {
                app.updateMaterials();
                app.reset();
            }

            if (ImGui::BeginMenu("Albedo map"))
            {
                if (ImGui::SliderInt("Albedo map id", &material.albedoMapId, -1, (int)app.textures.size() - 1))
                {
                    app.updateMaterials();
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
                app.updateMaterials();
                app.reset();
            }

            if (ImGui::BeginMenu("Metalness"))
            {
                if (ImGui::ColorEdit3("Metalness color", (float*)&material.metalnessColor, ImGuiColorEditFlags_Float))
                {
                    app.updateMaterials();
                    app.reset();
                }

                if (ImGui::SliderFloat("Metalness", &material.metalness, 0, 1))
                {
                    app.updateMaterials();
                    app.reset();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Emission"))
            {
                if (ImGui::ColorEdit3("Emission color", (float*)&material.emissionColor, ImGuiColorEditFlags_Float))
                {
                    app.updateMaterials();
                    app.reset();
                }

                if (ImGui::DragFloat("Emission", &material.emissionStrength, .001f, 0, 100))
                {
                    app.updateMaterials();
                    app.reset();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Fresnel"))
            {
                if (ImGui::ColorEdit3("Fresnel color", (float*)&material.fresnelColor, ImGuiColorEditFlags_Float))
                {
                    app.updateMaterials();
                    app.reset();
                }

                if (ImGui::DragFloat("Fresnel", &material.fresnelStrength, .0001f, 0, 100))
                {
                    app.updateMaterials();
                    app.reset();
                }

                ImGui::EndMenu();
            }

            if (ImGui::DragFloat("Refraction", &material.refractionFactor, .001f, 0, 100))
            {
                app.updateMaterials();
                app.reset();
            }

            if (ImGui::DragFloat("Density", &material.density, .001f, 0, 100))
            {
                app.updateMaterials();
                app.reset();
            }

            if (ImGui::Button("Delete"))
            {
                app.materials.erase(app.materials.begin() + i);
                app.updateMaterials();
                app.reset();
            }

            ImGui::EndMenu();
        }
    }

    if (ImGui::Button("Create"))
    {
        app.materials.push_back(TracerX::Material());
        app.updateMaterials();
        app.reset();
    }
}

void GeometryUI(Application& app)
{
    if (ImGui::BeginMenu(std::string("Spheres (" + std::to_string(app.spheres.size()) + ')').c_str()))
    {
        for (int i = 0; i < app.spheres.size(); i++)
        {
            if (ImGui::BeginMenu(std::string("Sphere " + std::to_string(i)).c_str()))
            {
                TracerX::Sphere& sphere = app.spheres[i];

                if (ImGui::DragFloat3("Origin", (float*)&sphere.origin, .001f))
                {
                    app.updateSpheres();
                    app.reset();
                }

                if (ImGui::DragFloat("Radius", &sphere.radius, .001f))
                {
                    app.updateSpheres();
                    app.reset();
                }

                if (ImGui::DragFloat3("Rotate", (float*)&sphere.rotation, .01f))
                {
                    app.updateSpheres();
                    app.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < app.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == sphere.materialId) &&
                            materialId != sphere.materialId)
                        {
                            sphere.materialId = materialId;
                            app.updateSpheres();
                            app.reset();
                        }
                    }

                    ImGui::EndListBox();
                }

                if (ImGui::Button("Focus camera"))
                {
                    app.camera.lookAt(sphere.origin);
                    app.shader.setUniform("FocalLength", app.camera.focalLength);
                    app.reset();
                }

                if (ImGui::Button("Delete"))
                {
                    app.spheres.erase(app.spheres.begin() + i);
                    app.updateSpheres();
                    app.reset();
                }

                ImGui::EndMenu();
            }
        }
        
        if (ImGui::Button("Create"))
        {
            app.spheres.push_back(TracerX::Sphere(sf::Vector3f(0, 0, 0), 0, 0));
            app.updateSpheres();
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(std::string("Boxes (" + std::to_string(app.boxes.size()) + ')').c_str()))
    {
        for (int i = 0; i < app.boxes.size(); i++)
        {
            if (ImGui::BeginMenu(std::string("Box " + std::to_string(i)).c_str()))
            {
                TracerX::Box& box = app.boxes[i];

                ImGui::Text("Bounding box min: (%f, %f, %f)", box.boxMin.x, box.boxMin.y, box.boxMin.z);
                ImGui::Text("Bounding box max: (%f, %f, %f)", box.boxMax.x, box.boxMax.y, box.boxMax.z);

                if (ImGui::DragFloat3("Origin", (float*)&box.origin, .001f))
                {
                    box.updateAABB();
                    app.updateBoxes();
                    app.reset();
                }

                if (ImGui::DragFloat3("Size", (float*)&box.size, .001f))
                {
                    box.updateAABB();
                    app.updateBoxes();
                    app.reset();
                }

                if (ImGui::DragFloat3("Rotate", (float*)&box.rotation, .01f))
                {
                    box.updateAABB();
                    app.updateBoxes();
                    app.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < app.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == box.materialId) &&
                            materialId != box.materialId)
                        {
                            box.materialId = materialId;
                            app.updateBoxes();
                            app.reset();
                        }
                    }
                
                    ImGui::EndListBox();
                }

                if (ImGui::Button("Focus camera"))
                {
                    app.camera.lookAt(box.origin);
                    app.shader.setUniform("FocalLength", app.camera.focalLength);
                    app.reset();
                }

                if (ImGui::Button("Delete"))
                {
                    app.boxes.erase(app.boxes.begin() + i);
                    app.updateBoxes();
                    app.reset();
                }

                ImGui::EndMenu();
            }
        }

        if (ImGui::Button("Create"))
        {
            app.boxes.push_back(TracerX::Box(sf::Vector3f(0, 0, 0), sf::Vector3f(0, 0, 0), 0));
            app.updateBoxes();
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(std::string("Meshes (" + std::to_string(app.meshes.size()) + ')').c_str()))
    {
        for (int i = 0; i < app.meshes.size(); i++)
        {
            if (ImGui::BeginMenu(std::string("Mesh " + std::to_string(i)).c_str()))
            {
                TracerX::Mesh& mesh = app.meshes[i];

                ImGui::Text("Indices start: %i", mesh.indicesStart);
                ImGui::Text("Indices end: %i", mesh.indicesEnd);
                ImGui::Text("Bounding box min: (%f, %f, %f)", mesh.boxMin.x, mesh.boxMin.y, mesh.boxMin.z);
                ImGui::Text("Bounding box max: (%f, %f, %f)", mesh.boxMax.x, mesh.boxMax.y, mesh.boxMax.z);

                sf::Vector3f offset = mesh.position;
                if (ImGui::DragFloat3("Offset", (float*)&offset, .01f))
                {
                    mesh.offset(offset - mesh.position, app.indices, app.vertices);
                    mesh.updateAABB(app.indices, app.vertices);
                    app.updateMeshes();
                    app.updateVertices();
                    app.reset();
                }

                sf::Vector3f scale = mesh.size;
                if (ImGui::DragFloat3("Scale", (float*)&scale, .01f) && scale.x != 0 && scale.y != 0 && scale.z != 0)
                {
                    mesh.scale(TracerX::div(scale, mesh.size), app.indices, app.vertices);
                    mesh.updateAABB(app.indices, app.vertices);
                    app.updateMeshes();
                    app.updateVertices();
                    app.reset();
                }

                sf::Vector3f rotation = mesh.rotation;
                if (ImGui::DragFloat3("Rotate", (float*)&rotation, .01f))
                {
                    mesh.rotate(rotation - mesh.rotation, app.indices, app.vertices);
                    mesh.updateAABB(app.indices, app.vertices);
                    app.updateMeshes();
                    app.updateVertices();
                    app.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < app.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == mesh.materialId) &&
                            materialId != mesh.materialId)
                        {
                            mesh.materialId = materialId;
                            app.updateMeshes();
                            app.reset();
                        }
                    }

                    ImGui::EndListBox();
                }

                if (ImGui::Button("Focus camera"))
                {
                    app.camera.lookAt(mesh.position);
                    app.shader.setUniform("FocalLength", app.camera.focalLength);
                    app.reset();
                }

                ImGui::EndMenu();
            }
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
                    app.updateIndices();
                    app.updateMaterials();
                    app.updateMeshes();
                    app.updateVertices();
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
        if (ImGui::ColorEdit3("Color", (float*)&app.environment.sunColor, ImGuiColorEditFlags_Float))
        {
            app.environment.set(app.shader);
            app.reset();
        }

        if (ImGui::DragFloat3("Direction", (float*)&app.environment.sunDirection, 0.001f))
        {
            app.environment.sunDirection = TracerX::normalized(app.environment.sunDirection);
            app.environment.set(app.shader);
            app.reset();
        }

        if (ImGui::DragFloat("Focus", &app.environment.sunFocus, 1, 0, 10000))
        {
            app.environment.set(app.shader);
            app.reset();
        }

        if (ImGui::DragFloat("Intensity", &app.environment.sunIntensity, 0.001f, 0, 100000))
        {
            app.environment.set(app.shader);
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Sky"))
    {
        if (ImGui::ColorEdit3("Horizon", (float*)&app.environment.skyColorHorizon, ImGuiColorEditFlags_Float))
        {
            app.environment.set(app.shader);
            app.reset();
        }

        float zenith[3]{ app.environment.skyColorZenith.x, app.environment.skyColorZenith.y, app.environment.skyColorZenith.z };
        if (ImGui::ColorEdit3("Zenith", zenith, ImGuiColorEditFlags_Float))
        {
            app.environment.skyColorZenith = sf::Vector3f(zenith[0], zenith[1], zenith[2]);
            app.environment.set(app.shader);
            app.reset();
        }

        if (ImGui::DragFloat("Intensity", &app.environment.skyIntensity, 0.0001f, 0, 2))
        {
            app.environment.set(app.shader);
            app.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::ColorEdit3("Ground", (float*)&app.environment.groundColor, ImGuiColorEditFlags_Float))
    {
        app.environment.set(app.shader);
        app.reset();
    }

    if (ImGui::Checkbox("Enable", &app.environment.enabled))
    {
        app.environment.set(app.shader);
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

}