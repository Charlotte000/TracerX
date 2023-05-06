#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>
#include <ImGui/imgui_stdlib.h>

#include <TracerX/RendererUI.h>

namespace TracerX
{

void InfoUI(RendererVisual& renderer, sf::RenderTexture& target)
{
    ImGui::Begin("Info");
    ImGui::Text("Frame count: %d", renderer.frameCount);
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::Text("Window size: %dx%d", renderer.size.x, renderer.size.y);

    if (ImGui::TreeNode("Camera"))
    {
        if (ImGui::TreeNode("Position"))
        {
            float position[3]{ renderer.camera.position.x, renderer.camera.position.y, renderer.camera.position.z };
            if (ImGui::DragFloat3("", position, .01f))
            {
                renderer.camera.position = sf::Vector3f(position[0], position[1], position[2]);
                renderer.reset();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Forward"))
        {
            float forward[3]{ renderer.camera.forward.x, renderer.camera.forward.y, renderer.camera.forward.z };
            if (ImGui::DragFloat3("", forward, .01f))
            {
                renderer.camera.position = normalized(sf::Vector3f(forward[0], forward[1], forward[2]));
                renderer.reset();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Up"))
        {
            float up[3]{ renderer.camera.up.x, renderer.camera.up.y, renderer.camera.up.z };
            if (ImGui::DragFloat3("", up, .01f))
            {
                renderer.camera.position = normalized(sf::Vector3f(up[0], up[1], up[2]));
                renderer.reset();
            }

            ImGui::TreePop();
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

        ImGui::TreePop();
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

    if (ImGui::SliderInt("Width subdivise", &renderer.subDivisor.x, 1, renderer.size.x))
    {
        renderer.reset();
    }

    if (ImGui::SliderInt("Height subdivise", &renderer.subDivisor.y, 1, renderer.size.y))
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

    ImGui::End();
}

void MaterailUI(RendererVisual& renderer)
{
    ImGui::Begin("Materials");
    for (int i = 0; i < renderer.materials.size(); i++)
    {
        Material material = renderer.materials[i];

        if (ImGui::TreeNode((void*)(intptr_t)i, "Material %i", i))
        {
            float albedo[3]{ material.albedoColor.x, material.albedoColor.y, material.albedoColor.z };
            if (ImGui::ColorEdit3("Albedo", albedo, ImGuiColorEditFlags_Float))
            {
                renderer.materials[i].albedoColor = sf::Vector3f(albedo[0], albedo[1], albedo[2]);
                renderer.updateMaterials();
                renderer.reset();
            }

            if (ImGui::TreeNode("Albedo map"))
            {
                if (ImGui::SliderInt("Albedo map id", &renderer.materials[i].albedoMapId, -1, renderer.albedoMaps.size() - 1))
                {
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (renderer.materials[i].albedoMapId >= 0)
                {
                    ImGui::Image(renderer.albedoMaps[renderer.materials[i].albedoMapId]);
                }

                ImGui::TreePop();
            }

            if (ImGui::SliderFloat("Roughness", &material.roughness, 0, 1))
            {
                renderer.materials[i].roughness = material.roughness;
                renderer.updateMaterials();
                renderer.reset();
            }

            if (ImGui::TreeNode("Metalness"))
            {
                float metalnessColor[3]{ material.metalnessColor.x, material.metalnessColor.y, material.metalnessColor.z };
                if (ImGui::ColorEdit3("Metalness color", metalnessColor, ImGuiColorEditFlags_Float))
                {
                    renderer.materials[i].metalnessColor = sf::Vector3f(metalnessColor[0], metalnessColor[1], metalnessColor[2]);
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (ImGui::SliderFloat("Metalness", &material.metalness, 0, 1))
                {
                    renderer.materials[i].metalness = material.metalness;
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Emission"))
            {
                float emissionColor[3]{ material.emissionColor.x, material.emissionColor.y, material.emissionColor.z };
                if (ImGui::ColorEdit3("Emission color", emissionColor, ImGuiColorEditFlags_Float))
                {
                    renderer.materials[i].emissionColor = sf::Vector3f(emissionColor[0], emissionColor[1], emissionColor[2]);
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Emission", &material.emissionStrength, .001f, 0, 100))
                {
                    renderer.materials[i].emissionStrength = material.emissionStrength;
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Fresnel"))
            {
                float fresnelColor[3]{ material.fresnelColor.x, material.fresnelColor.y, material.fresnelColor.z };
                if (ImGui::ColorEdit3("Fresnel color", fresnelColor, ImGuiColorEditFlags_Float))
                {
                    renderer.materials[i].fresnelColor = sf::Vector3f(fresnelColor[0], fresnelColor[1], fresnelColor[2]);
                    renderer.updateMaterials();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Fresnel", &material.fresnelStrength, .0001f, 0, 100))
                {
                    renderer.materials[i].fresnelStrength = material.fresnelStrength;
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Transparency"))
            {
                if (ImGui::DragFloat("Refraction", &material.refractionFactor, .001f, 0, 100))
                {
                    renderer.materials[i].refractionFactor = material.refractionFactor;
                    renderer.updateMaterials();
                    renderer.reset();
                }

                ImGui::TreePop();
            }

            if (ImGui::Button("Delete"))
            {
                renderer.materials.erase(renderer.materials.begin() + i);
                renderer.updateMaterials();
                renderer.reset();
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::Button("Create"))
    {
        renderer.materials.push_back(Material());
        renderer.updateMaterials();
        renderer.reset();
    }

    ImGui::End();
}

void GeometryUI(RendererVisual& renderer)
{
    ImGui::Begin("Geometry");
    if (ImGui::TreeNode("Spheres", "Spheres (%d)", (int)renderer.spheres.size()))
    {
        for (int i = 0; i < renderer.spheres.size(); i++)
        {
            if (ImGui::TreeNode((void*)(intptr_t)i, "Sphere %i", i))
            {
                float origin[3]{ renderer.spheres[i].origin.x, renderer.spheres[i].origin.y, renderer.spheres[i].origin.z };
                if (ImGui::DragFloat3("Origin", origin, .001f))
                {
                    renderer.spheres[i].origin = sf::Vector3f(origin[0], origin[1], origin[2]);
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::DragFloat("Radius", &renderer.spheres[i].radius, .001f))
                {
                    renderer.updateSpheres();
                    renderer.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == renderer.spheres[i].materialId) &&
                            materialId != renderer.spheres[i].materialId)
                        {
                            renderer.spheres[i].materialId = materialId;
                            renderer.updateSpheres();
                            renderer.reset();
                        }
                    }

                    ImGui::EndListBox();
                }

                if (ImGui::Button("Delete"))
                {
                    renderer.spheres.erase(renderer.spheres.begin() + i);
                    renderer.updateSpheres();
                    renderer.reset();
                }

                ImGui::TreePop();
            }
        }
        
        if (ImGui::Button("Create"))
        {
            renderer.spheres.push_back(Sphere(sf::Vector3f(0, 0, 0), 0, 0));
            renderer.updateSpheres();
            renderer.reset();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Boxes", "Boxes (%d)", (int)renderer.boxes.size()))
    {
        for (int i = 0; i < renderer.boxes.size(); i++)
        {
            if (ImGui::TreeNode((void*)(intptr_t)i, "Box %i", i))
            {
                float origin[3]{ renderer.boxes[i].origin.x, renderer.boxes[i].origin.y, renderer.boxes[i].origin.z };
                if (ImGui::DragFloat3("Origin", origin, .001f))
                {
                    renderer.boxes[i].origin = sf::Vector3f(origin[0], origin[1], origin[2]);
                    renderer.updateBoxes();
                    renderer.reset();
                }

                float size[3]{ renderer.boxes[i].size.x, renderer.boxes[i].size.y, renderer.boxes[i].size.z };
                if (ImGui::DragFloat3("Size", size, .001f))
                {
                    renderer.boxes[i].size = sf::Vector3f(size[0], size[1], size[2]);
                    renderer.updateBoxes();
                    renderer.reset();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == renderer.boxes[i].materialId) &&
                            materialId != renderer.boxes[i].materialId)
                        {
                            renderer.boxes[i].materialId = materialId;
                            renderer.updateBoxes();
                            renderer.reset();
                        }
                    }
                
                    ImGui::EndListBox();
                }

                if (ImGui::Button("Delete"))
                {
                    renderer.boxes.erase(renderer.boxes.begin() + i);
                    renderer.updateBoxes();
                    renderer.reset();
                }

                ImGui::TreePop();
            }
        }

        if (ImGui::Button("Create"))
        {
            renderer.boxes.push_back(Box(sf::Vector3f(0, 0, 0), sf::Vector3f(0, 0, 0), 0));
            renderer.updateBoxes();
            renderer.reset();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Meshes", "Meshes (%d)", (int)renderer.meshes.size()))
    {
        for (int i = 0; i < renderer.meshes.size(); i++)
        {
            if (ImGui::TreeNode((void*)(intptr_t)i, "Mesh %i", i))
            {
                ImGui::Text("Indices start: %i", renderer.meshes[i].indicesStart);
                ImGui::Text("Indices end: %i", renderer.meshes[i].indicesEnd);

                float offset[3] = { renderer.meshes[i].position.x, renderer.meshes[i].position.y, renderer.meshes[i].position.z };
                if (ImGui::DragFloat3("Offset", offset, .01f))
                {
                    sf::Vector3f offsetV(offset[0] - renderer.meshes[i].position.x, offset[1] - renderer.meshes[i].position.y, offset[2] - renderer.meshes[i].position.z);
                    renderer.reset();
                    renderer.meshes[i].offset(offsetV, renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                }

                float scale[3] = { renderer.meshes[i].size.x, renderer.meshes[i].size.y, renderer.meshes[i].size.z };
                if (ImGui::DragFloat3("Scale", scale, .01f))
                {
                    sf::Vector3f sizeV(scale[0] / renderer.meshes[i].size.x, scale[1] / renderer.meshes[i].size.y, scale[2] / renderer.meshes[i].size.z);
                    renderer.reset();
                    renderer.meshes[i].scale(sizeV, renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                }

                float rotation[3] = { renderer.meshes[i].rotation.x, renderer.meshes[i].rotation.y, renderer.meshes[i].rotation.z };
                if (ImGui::DragFloat3("Rotate", rotation, .01f))
                {
                    sf::Vector3f rotationV(rotation[0] - renderer.meshes[i].rotation.x, rotation[1] - renderer.meshes[i].rotation.y, rotation[2] - renderer.meshes[i].rotation.z);
                    renderer.reset();
                    renderer.meshes[i].rotate(rotationV, renderer.indices, renderer.vertices);
                    renderer.updateMeshes();
                    renderer.updateVertices();
                }

                if (ImGui::BeginListBox("Material id"))
                {
                    for (int materialId = 0; materialId < renderer.materials.size(); materialId++)
                    {
                        if (ImGui::Selectable(("Material " + std::to_string(materialId)).c_str(), materialId == renderer.meshes[i].materialId) &&
                            materialId != renderer.meshes[i].materialId)
                        {
                            renderer.meshes[i].materialId = materialId;
                            renderer.updateMeshes();
                            renderer.reset();
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::TreePop();
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

        ImGui::TreePop();
    }

    ImGui::End();
}

void EnvironmentUI(RendererVisual& renderer)
{
    ImGui::Begin("Envorinment");
    if (ImGui::TreeNode("Sun"))
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

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sky"))
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

        ImGui::TreePop();
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

    ImGui::End();
}

}