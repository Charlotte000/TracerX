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
        float position[3]{ renderer.camera.position.x, renderer.camera.position.y, renderer.camera.position.z };
        if (ImGui::DragFloat3("Position", position, .01f))
        {
            renderer.camera.position = sf::Vector3f(position[0], position[1], position[2]);
            renderer.reset();
        }

        float forward[3]{ renderer.camera.forward.x, renderer.camera.forward.y, renderer.camera.forward.z };
        if (ImGui::DragFloat3("Forward", forward, .01f))
        {
            renderer.camera.forward = normalized(sf::Vector3f(forward[0], forward[1], forward[2]));
            renderer.reset();
        }

        float up[3]{ renderer.camera.up.x, renderer.camera.up.y, renderer.camera.up.z };
        if (ImGui::DragFloat3("Up", up, .01f))
        {
            renderer.camera.up = normalized(sf::Vector3f(up[0], up[1], up[2]));
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
            
            float albedo[3]{ material.albedoColor.x, material.albedoColor.y, material.albedoColor.z };
            if (ImGui::ColorEdit3("Albedo", albedo, ImGuiColorEditFlags_Float))
            {
                material.albedoColor = sf::Vector3f(albedo[0], albedo[1], albedo[2]);
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
                float metalnessColor[3]{ material.metalnessColor.x, material.metalnessColor.y, material.metalnessColor.z };
                if (ImGui::ColorEdit3("Metalness color", metalnessColor, ImGuiColorEditFlags_Float))
                {
                    material.metalnessColor = sf::Vector3f(metalnessColor[0], metalnessColor[1], metalnessColor[2]);
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
                float emissionColor[3]{ material.emissionColor.x, material.emissionColor.y, material.emissionColor.z };
                if (ImGui::ColorEdit3("Emission color", emissionColor, ImGuiColorEditFlags_Float))
                {
                    material.emissionColor = sf::Vector3f(emissionColor[0], emissionColor[1], emissionColor[2]);
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
                float fresnelColor[3]{ material.fresnelColor.x, material.fresnelColor.y, material.fresnelColor.z };
                if (ImGui::ColorEdit3("Fresnel color", fresnelColor, ImGuiColorEditFlags_Float))
                {
                    material.fresnelColor = sf::Vector3f(fresnelColor[0], fresnelColor[1], fresnelColor[2]);
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

                float origin[3]{ sphere.origin.x, sphere.origin.y, sphere.origin.z };
                if (ImGui::DragFloat3("Origin", origin, .001f))
                {
                    sphere.origin = sf::Vector3f(origin[0], origin[1], origin[2]);
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Radius", &sphere.radius, .001f))
                {
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), sphere.materialId) &&
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

                float origin[3]{ box.origin.x, box.origin.y, box.origin.z };
                if (ImGui::DragFloat3("Origin", origin, .001f))
                {
                    box.origin = sf::Vector3f(origin[0], origin[1], origin[2]);
                    renderer.updateBoxes();
                    renderer.reset();
                }

                float size[3]{ box.size.x, box.size.y, box.size.z };
                if (ImGui::DragFloat3("Size", size, .001f))
                {
                    box.size = sf::Vector3f(size[0], size[1], size[2]);
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

                float offset[3] = { mesh.position.x, mesh.position.y, mesh.position.z };
                if (ImGui::DragFloat3("Offset", offset, .01f))
                {
                    sf::Vector3f offsetV = sf::Vector3f(offset[0], offset[1], offset[2]) - mesh.position;
                    renderer.reset();
                    mesh.offset(offsetV, renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                }

                float scale[3] = { mesh.size.x, mesh.size.y, mesh.size.z };
                if (ImGui::DragFloat3("Scale", scale, .01f) && scale[0] != 0 && scale[1] != 0 && scale[2] != 0)
                {
                    sf::Vector3f sizeV(scale[0] / mesh.size.x, scale[1] / mesh.size.y, scale[2] / mesh.size.z);
                    renderer.reset();
                    mesh.scale(sizeV, renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                }

                float rotation[3] = { mesh.rotation.x, mesh.rotation.y, mesh.rotation.z };
                if (ImGui::DragFloat3("Rotate", rotation, .01f))
                {
                    sf::Vector3f rotationV = sf::Vector3f(rotation[0], rotation[1], rotation[2]) - mesh.rotation;
                    renderer.reset();
                    mesh.rotate(rotationV, renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
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
        float sunColor[3]{ renderer.environment.sunColor.x, renderer.environment.sunColor.y, renderer.environment.sunColor.z };
        if (ImGui::ColorEdit3("Color", sunColor, ImGuiColorEditFlags_Float))
        {
            renderer.environment.sunColor = sf::Vector3f(sunColor[0], sunColor[1], sunColor[2]);
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        float sunDirection[3]{ renderer.environment.sunDirection.x, renderer.environment.sunDirection.y, renderer.environment.sunDirection.z };
        if (ImGui::DragFloat3("Direction", sunDirection, 0.001f))
        {
            renderer.environment.sunDirection = normalized(sf::Vector3f(sunDirection[0], sunDirection[1], sunDirection[2]));
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        if (ImGui::DragFloat("Focus", &renderer.environment.sunFocus, 1, 0, 10000))
        {
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        if (ImGui::DragFloat("Intensity", &renderer.environment.sunIntensity, 0.001f, 0, 100000))
        {
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Sky"))
    {
        float horizon[3]{ renderer.environment.skyColorHorizon.x, renderer.environment.skyColorHorizon.y, renderer.environment.skyColorHorizon.z };
        if (ImGui::ColorEdit3("Horizon", horizon, ImGuiColorEditFlags_Float))
        {
            renderer.environment.skyColorHorizon = sf::Vector3f(horizon[0], horizon[1], horizon[2]);
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        float zenith[3]{ renderer.environment.skyColorZenith.x, renderer.environment.skyColorZenith.y, renderer.environment.skyColorZenith.z };
        if (ImGui::ColorEdit3("Zenith", zenith, ImGuiColorEditFlags_Float))
        {
            renderer.environment.skyColorZenith = sf::Vector3f(zenith[0], zenith[1], zenith[2]);
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        if (ImGui::DragFloat("Intensity", &renderer.environment.skyIntensity, 0.0001f, 0, 2))
        {
            renderer.environment.set(renderer.shader, "Environment");
            renderer.reset();
        }

        ImGui::EndMenu();
    }

    float ground[3]{ renderer.environment.groundColor.x, renderer.environment.groundColor.y, renderer.environment.groundColor.z };
    if (ImGui::ColorEdit3("Ground", ground, ImGuiColorEditFlags_Float))
    {
        renderer.environment.groundColor = sf::Vector3f(ground[0], ground[1], ground[2]);
        renderer.environment.set(renderer.shader, "Environment");
        renderer.reset();
    }

    if (ImGui::Checkbox("Enable", &renderer.environment.enabled))
    {
        renderer.environment.set(renderer.shader, "Environment");
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