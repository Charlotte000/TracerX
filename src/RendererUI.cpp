#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>
#include <ImGui/imgui_stdlib.h>
#include <TracerX/RendererUI.h>

namespace TracerX
{

void InfoUI(Renderer& renderer, sf::RenderTexture& target)
{
    ImGui::Text("Window size: %dx%d", renderer.size.x, renderer.size.y);
    if (ImGui::BeginMenu("Camera"))
    {
        if (ImGui::DragFloat3("Position", (float*)&renderer.camera.position, .01f))
        {
            renderer.reset();
        }

        if (ImGui::DragFloat3("Forward", (float*)&renderer.camera.forward, .01f))
        {
            renderer.camera.forward = normalized(renderer.camera.forward);
            renderer.reset();
        }

        if (ImGui::DragFloat3("Up", (float*)&renderer.camera.up, .01f))
        {
            renderer.camera.up = normalized(renderer.camera.up);
            renderer.reset();
        }

        if (ImGui::DragFloat("Focal length", &renderer.camera.focalLength, 0.001f, 0, 1000))
        {
            renderer.shader.setUniform("FocalLength", renderer.camera.focalLength);
            renderer.reset();
        }

        if (ImGui::DragFloat("Focal strength", &renderer.camera.focusStrength, 0.0001f, 0, 1000))
        {
            renderer.shader.setUniform("FocusStrength", renderer.camera.focusStrength);
            renderer.reset();
        }

        ImGui::EndMenu();
    }
    
    if (ImGui::DragInt("Samples", &renderer.sampleCount, 0.01f, 0, 10000))
    {
        renderer.shader.setUniform("SampleCount", renderer.sampleCount);
        renderer.reset();
    }

    if (ImGui::DragInt("Max bounce", &renderer.maxBounceCount, 0.01f, 0, 10000))
    {
        renderer.shader.setUniform("MaxBouceCount", renderer.maxBounceCount);
        renderer.reset();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::SliderInt("Width subdivise", &renderer.subDivisor.x, 1, renderer.size.x, "%d", ImGuiSliderFlags_Logarithmic))
    {
        renderer.reset();
    }

    if (ImGui::SliderInt("Height subdivise", &renderer.subDivisor.y, 1, renderer.size.y, "%d", ImGuiSliderFlags_Logarithmic))
    {
        renderer.reset();
    }

    ImGui::Checkbox("Show cursor", &renderer.showCursor);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static int totalPixels = renderer.size.x * renderer.size.y;
    static int pixelDifference = totalPixels;
    static float differencePercentage = 100;
    if (ImGui::Button("Update pixel difference"))
    {
        pixelDifference = renderer.getPixelDifference();
        differencePercentage = 100.0f * pixelDifference / totalPixels;
    }
    
    ImGui::Text("Difference: %7dpx / %7dpx | %.2f %%", pixelDifference, totalPixels, differencePercentage);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Checkbox("Camera control", &renderer.isCameraControl))
    {
        sf::Mouse::setPosition(renderer.size / 2, renderer.window);
        renderer.window.setMouseCursorVisible(false);
        renderer.reset();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Checkbox("Render", &renderer.isProgressive))
    {
        renderer.subStage = 0;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static std::string fileName;
    ImGui::InputText("Filename", &fileName);
    if (ImGui::Button("Save"))
    {
        target.getTexture().copyToImage().saveToFile(fileName);
    }
}

void MaterialUI(Renderer& renderer)
{
    for (int i = 0; i < renderer.materials.size(); i++)
    {
        if (ImGui::BeginMenu(std::string("Material " + std::to_string(i)).c_str()))
        {
            Material& material = renderer.materials[i];
            
            if (ImGui::ColorEdit3("Albedo", (float*)&material.albedoColor, ImGuiColorEditFlags_Float))
            {
                renderer.updateMaterials();
                renderer.reset();
            }

            if (ImGui::BeginMenu("Albedo map"))
            {
                if (ImGui::SliderInt("Albedo map id", &material.albedoMapId, -1, renderer.textures.size() - 1))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (material.albedoMapId >= 0)
                {
                    ImGui::Image(renderer.textures[material.albedoMapId], sf::Vector2f(100, 100));
                }

                ImGui::EndMenu();
            }

            if (ImGui::SliderFloat("Roughness", &material.roughness, 0, 1))
            {
                renderer.updateMaterials();
                renderer.reset();
            }

            if (ImGui::BeginMenu("Metalness"))
            {
                if (ImGui::ColorEdit3("Metalness color", (float*)&material.metalnessColor, ImGuiColorEditFlags_Float))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (ImGui::SliderFloat("Metalness", &material.metalness, 0, 1))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Emission"))
            {
                if (ImGui::ColorEdit3("Emission color", (float*)&material.emissionColor, ImGuiColorEditFlags_Float))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Emission", &material.emissionStrength, .001f, 0, 100))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Fresnel"))
            {
                if (ImGui::ColorEdit3("Fresnel color", (float*)&material.fresnelColor, ImGuiColorEditFlags_Float))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Fresnel", &material.fresnelStrength, .0001f, 0, 100))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::EndMenu();
            }

            if (ImGui::DragFloat("Refraction", &material.refractionFactor, .001f, 0, 100))
            {
                renderer.updateMaterials();
                renderer.reset();
            }

            if (ImGui::DragFloat("Density", &material.density, .001f, 0, 100))
            {
                renderer.updateMaterials();
                renderer.reset();
            }

            if (ImGui::Button("Delete"))
            {
                renderer.materials.erase(renderer.materials.begin() + i);
                renderer.updateMaterials();
                renderer.reset();
            }

            ImGui::EndMenu();
        }
    }

    if (ImGui::Button("Create"))
    {
        renderer.materials.push_back(Material());
        renderer.updateMaterials();
        renderer.reset();
    }
}

void GeometryUI(Renderer& renderer)
{
    if (ImGui::BeginMenu(std::string("Spheres (" + std::to_string(renderer.spheres.size()) + ')').c_str()))
    {
        for (int i = 0; i < renderer.spheres.size(); i++)
        {
            if (ImGui::BeginMenu(std::string("Sphere " + std::to_string(i)).c_str()))
            {
                Sphere& sphere = renderer.spheres[i];

                if (ImGui::DragFloat3("Origin", (float*)&sphere.origin, .001f))
                {
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Radius", &sphere.radius, .001f))
                {
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::DragFloat3("Rotate", (float*)&sphere.rotation, .01f))
                {
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == sphere.materialId) &&
                            materialId != sphere.materialId)
                        {
                            sphere.materialId = materialId;
                            renderer.updateSpheres();
                            renderer.reset();
                        }
                    }

                    ImGui::EndListBox();
                }

                if (ImGui::Button("Focus camera"))
                {
                    renderer.camera.focalLength = length(renderer.camera.position - sphere.origin);
                    renderer.reset();
                }

                if (ImGui::Button("Delete"))
                {
                    renderer.spheres.erase(renderer.spheres.begin() + i);
                    renderer.updateSpheres();
                    renderer.reset();
                }

                ImGui::EndMenu();
            }
        }
        
        if (ImGui::Button("Create"))
        {
            renderer.spheres.push_back(Sphere(sf::Vector3f(0, 0, 0), 0, 0));
            renderer.updateSpheres();
            renderer.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(std::string("Boxes (" + std::to_string(renderer.boxes.size()) + ')').c_str()))
    {
        for (int i = 0; i < renderer.boxes.size(); i++)
        {
            if (ImGui::BeginMenu(std::string("Box " + std::to_string(i)).c_str()))
            {
                Box& box = renderer.boxes[i];

                ImGui::Text("Bounding box min: (%f, %f, %f)", box.boxMin.x, box.boxMin.y, box.boxMin.z);
                ImGui::Text("Bounding box max: (%f, %f, %f)", box.boxMax.x, box.boxMax.y, box.boxMax.z);

                if (ImGui::DragFloat3("Origin", (float*)&box.origin, .001f))
                {
                    box.updateAABB();
                    renderer.updateBoxes();
                    renderer.reset();
                }

                if (ImGui::DragFloat3("Size", (float*)&box.size, .001f))
                {
                    box.updateAABB();
                    renderer.updateBoxes();
                    renderer.reset();
                }

                if (ImGui::DragFloat3("Rotate", (float*)&box.rotation, .01f))
                {
                    box.updateAABB();
                    renderer.updateBoxes();
                    renderer.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == box.materialId) &&
                            materialId != box.materialId)
                        {
                            box.materialId = materialId;
                            renderer.updateBoxes();
                            renderer.reset();
                        }
                    }
                
                    ImGui::EndListBox();
                }

                if (ImGui::Button("Focus camera"))
                {
                    renderer.camera.focalLength = length(renderer.camera.position - box.origin);
                    renderer.reset();
                }

                if (ImGui::Button("Delete"))
                {
                    renderer.boxes.erase(renderer.boxes.begin() + i);
                    renderer.updateBoxes();
                    renderer.reset();
                }

                ImGui::EndMenu();
            }
        }

        if (ImGui::Button("Create"))
        {
            renderer.boxes.push_back(Box(sf::Vector3f(0, 0, 0), sf::Vector3f(0, 0, 0), 0));
            renderer.updateBoxes();
            renderer.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(std::string("Meshes (" + std::to_string(renderer.meshes.size()) + ')').c_str()))
    {
        for (int i = 0; i < renderer.meshes.size(); i++)
        {
            if (ImGui::BeginMenu(std::string("Mesh " + std::to_string(i)).c_str()))
            {
                Mesh& mesh = renderer.meshes[i];

                ImGui::Text("Indices start: %i", mesh.indicesStart);
                ImGui::Text("Indices end: %i", mesh.indicesEnd);
                ImGui::Text("Bounding box min: (%f, %f, %f)", mesh.boxMin.x, mesh.boxMin.y, mesh.boxMin.z);
                ImGui::Text("Bounding box max: (%f, %f, %f)", mesh.boxMax.x, mesh.boxMax.y, mesh.boxMax.z);

                sf::Vector3f offset = mesh.position;
                if (ImGui::DragFloat3("Offset", (float*)&offset, .01f))
                {
                    mesh.offset(offset - mesh.position, renderer.indices, renderer.vertices);
                    mesh.updateAABB(renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                    renderer.reset();
                }

                sf::Vector3f scale = mesh.size;
                if (ImGui::DragFloat3("Scale", (float*)&scale, .01f) && scale.x != 0 && scale.y != 0 && scale.z != 0)
                {
                    mesh.scale(div(scale, mesh.size), renderer.indices, renderer.vertices);
                    mesh.updateAABB(renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                    renderer.reset();
                }

                sf::Vector3f rotation = mesh.rotation;
                if (ImGui::DragFloat3("Rotate", (float*)&rotation, .01f))
                {
                    mesh.rotate(rotation - mesh.rotation, renderer.indices, renderer.vertices);
                    mesh.updateAABB(renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                    renderer.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == mesh.materialId) &&
                            materialId != mesh.materialId)
                        {
                            mesh.materialId = materialId;
                            renderer.updateMeshes();
                            renderer.reset();
                        }
                    }

                    ImGui::EndListBox();
                }

                if (ImGui::Button("Focus camera"))
                {
                    renderer.camera.focalLength = length(renderer.camera.position - mesh.position);
                    renderer.reset();
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
                    renderer.addFile(filePath);
                    renderer.updateIndices();
                    renderer.updateMaterials();
                    renderer.updateMeshes();
                    renderer.updateVertices();
                    renderer.reset();
                }
                catch (std::runtime_error&) { }

                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }

        ImGui::EndMenu();
    }
}

void EnvironmentUI(Renderer& renderer)
{
    if (ImGui::BeginMenu("Sun"))
    {
        if (ImGui::ColorEdit3("Color", (float*)&renderer.environment.sunColor, ImGuiColorEditFlags_Float))
        {
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        if (ImGui::DragFloat3("Direction", (float*)&renderer.environment.sunDirection, 0.001f))
        {
            renderer.environment.sunDirection = normalized(renderer.environment.sunDirection);
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        if (ImGui::DragFloat("Focus", &renderer.environment.sunFocus, 1, 0, 10000))
        {
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        if (ImGui::DragFloat("Intensity", &renderer.environment.sunIntensity, 0.001f, 0, 100000))
        {
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Sky"))
    {
        if (ImGui::ColorEdit3("Horizon", (float*)&renderer.environment.skyColorHorizon, ImGuiColorEditFlags_Float))
        {
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        float zenith[3]{ renderer.environment.skyColorZenith.x, renderer.environment.skyColorZenith.y, renderer.environment.skyColorZenith.z };
        if (ImGui::ColorEdit3("Zenith", zenith, ImGuiColorEditFlags_Float))
        {
            renderer.environment.skyColorZenith = sf::Vector3f(zenith[0], zenith[1], zenith[2]);
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        if (ImGui::DragFloat("Intensity", &renderer.environment.skyIntensity, 0.0001f, 0, 2))
        {
            renderer.environment.set(renderer.shader);
            renderer.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::ColorEdit3("Ground", (float*)&renderer.environment.groundColor, ImGuiColorEditFlags_Float))
    {
        renderer.environment.set(renderer.shader);
        renderer.reset();
    }

    if (ImGui::Checkbox("Enable", &renderer.environment.enabled))
    {
        renderer.environment.set(renderer.shader);
        renderer.reset();
    }
}

void TextureUI(Renderer& renderer)
{
    for (int i = 0; i < renderer.textures.size(); i++)
    {
        if (ImGui::BeginMenu(std::string("Texture " + std::to_string(i)).c_str()))
        {
            ImGui::Image(renderer.textures[i], sf::Vector2f(100, 100));

            if (ImGui::Button("Delete"))
            {
                renderer.textures.erase(renderer.textures.begin() + i);
                renderer.updateTextures();
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
                renderer.addTexture(filePath);
                renderer.updateTextures();
                renderer.reset();
            }
            catch (std::runtime_error&) { }

            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

}