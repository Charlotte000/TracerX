#include <imgui.h>
#include <imgui-SFML.h>

#include "RendererUI.h"

void InfoUI(Renderer& renderer, bool& isProgressive, bool& isCameraControl, sf::RenderWindow& window, sf::RenderTexture& target, char* fileName, size_t fileNameSize)
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
                isProgressive = false;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Forward"))
        {
            float forward[3]{ renderer.camera.forward.x, renderer.camera.forward.y, renderer.camera.forward.z };
            if (ImGui::DragFloat3("", forward, .01f))
            {
                renderer.camera.position = normalized(sf::Vector3f(forward[0], forward[1], forward[2]));
                isProgressive = false;
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Up"))
        {
            float up[3]{ renderer.camera.up.x, renderer.camera.up.y, renderer.camera.up.z };
            if (ImGui::DragFloat3("", up, .01f))
            {
                renderer.camera.position = normalized(sf::Vector3f(up[0], up[1], up[2]));
                isProgressive = false;
            }
            ImGui::TreePop();
        }

        if (ImGui::DragFloat("Focal length", &renderer.camera.focalLength, 0.001f, 0, 1000))
        {
            renderer.shader.setUniform("FocalLength", renderer.camera.focalLength);
            isProgressive = false;
        }

        if (ImGui::DragFloat("Focal strength", &renderer.camera.focusStrength, 0.0001f, 0, 1000))
        {
            renderer.shader.setUniform("FocusStrength", renderer.camera.focusStrength);
            isProgressive = false;
        }

        ImGui::TreePop();
    }

    if (ImGui::DragInt("Ray per frame", &renderer.rayPerFrameCount, 0.01f, 0))
    {
        renderer.shader.setUniform("RayPerFrameCount", renderer.rayPerFrameCount);
        isProgressive = false;
    }

    if (ImGui::DragInt("Max bounce", &renderer.maxBounceCount, 0.01f, 0))
    {
        renderer.shader.setUniform("MaxBouceCount", renderer.maxBounceCount);
        isProgressive = false;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Checkbox("Camera control", &isCameraControl))
    {
        sf::Mouse::setPosition(renderer.size / 2, window);
        isProgressive = false;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox("Render", &isProgressive);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::InputText("Filename", fileName, fileNameSize);
    if (ImGui::Button("Save"))
    {
        target.getTexture().copyToImage().saveToFile(fileName);
    }

    ImGui::End();
}

void MaterailUI(Renderer& renderer, bool& isProgressive)
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
                renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                isProgressive = false;
            }

            if (ImGui::DragFloat("Roughness", &material.roughness, .001f, 0, 1))
            {
                renderer.materials[i].roughness = material.roughness;
                renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                isProgressive = false;
            }

            if (ImGui::TreeNode("Metalness"))
            {
                float metalnessColor[3]{ material.metalnessColor.x, material.metalnessColor.y, material.metalnessColor.z };
                if (ImGui::ColorEdit3("Metalness color", metalnessColor, ImGuiColorEditFlags_Float))
                {
                    renderer.materials[i].metalnessColor = sf::Vector3f(metalnessColor[0], metalnessColor[1], metalnessColor[2]);
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                if (ImGui::DragFloat("Metalness", &material.metalness, .001f, 0, 1))
                {
                    renderer.materials[i].metalness = material.metalness;
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Emission"))
            {
                float emissionColor[3]{ material.emissionColor.x, material.emissionColor.y, material.emissionColor.z };
                if (ImGui::ColorEdit3("Emission color", emissionColor, ImGuiColorEditFlags_Float))
                {
                    renderer.materials[i].emissionColor = sf::Vector3f(emissionColor[0], emissionColor[1], emissionColor[2]);
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                if (ImGui::DragFloat("Emission", &material.emissionStrength, .01f, 0, 100))
                {
                    renderer.materials[i].emissionStrength = material.emissionStrength;
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Fresnel"))
            {
                float fresnelColor[3]{ material.fresnelColor.x, material.fresnelColor.y, material.fresnelColor.z };
                if (ImGui::ColorEdit3("Fresnel color", fresnelColor, ImGuiColorEditFlags_Float))
                {
                    renderer.materials[i].fresnelColor = sf::Vector3f(fresnelColor[0], fresnelColor[1], fresnelColor[2]);
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                if (ImGui::DragFloat("Fresnel", &material.fresnelStrength, .001f, 0, 100))
                {
                    renderer.materials[i].fresnelStrength = material.fresnelStrength;
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Transparency"))
            {
                if (ImGui::Checkbox("Transparent", &material.isTransparent))
                {
                    renderer.materials[i].isTransparent = material.isTransparent;
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                if (ImGui::DragFloat("Refraction", &material.refractionFactor, .01f, 0, 100))
                {
                    renderer.materials[i].refractionFactor = material.refractionFactor;
                    renderer.materials[i].set(renderer.shader, "Materials[" + std::to_string(i) + ']');
                    isProgressive = false;
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void GeometryUI(Renderer& renderer, bool& isProgressive)
{
    ImGui::Begin("Geometry");
    if (ImGui::TreeNode("Spheres", "Spheres (%d)", renderer.spheres.size()))
    {
        for (int i = 0; i < renderer.spheres.size(); i++)
        {
            Sphere sphere = renderer.spheres[i];

            if (ImGui::TreeNode((void*)(intptr_t)i, "Sphere %i", i))
            {
                float origin[3]{ sphere.origin.x, sphere.origin.y, sphere.origin.z };
                if (ImGui::DragFloat3("Origin", origin, .001f))
                {
                    renderer.spheres[i].origin = sf::Vector3f(origin[0], origin[1], origin[2]);
                    renderer.spheres[i].set(renderer.shader, "Spheres[" + to_string(i) + ']');
                    isProgressive = false;
                }

                if (ImGui::DragFloat("Radius", &sphere.radius, .001f))
                {
                    renderer.spheres[i].radius = sphere.radius;
                    renderer.spheres[i].set(renderer.shader, "Spheres[" + to_string(i) + ']');
                    isProgressive = false;
                }

                ImGui::Text("Material id: %i", sphere.materialId);

                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Boxes", "Boxes (%d)", renderer.aabbs.size()))
    {
        for (int i = 0; i < renderer.aabbs.size(); i++)
        {
            AABB aabb = renderer.aabbs[i];

            if (ImGui::TreeNode((void*)(intptr_t)i, "Box %i", i))
            {
                float origin[3]{ aabb.origin.x, aabb.origin.y, aabb.origin.z };
                if (ImGui::DragFloat3("Origin", origin, .001f))
                {
                    renderer.aabbs[i].origin = sf::Vector3f(origin[0], origin[1], origin[2]);
                    renderer.aabbs[i].set(renderer.shader, "AABBs[" + to_string(i) + ']');
                    isProgressive = false;
                }

                float size[3]{ aabb.size.x, aabb.size.y, aabb.size.z };
                if (ImGui::DragFloat3("Size", size, .001f))
                {
                    renderer.aabbs[i].size = sf::Vector3f(size[0], size[1], size[2]);
                    renderer.aabbs[i].set(renderer.shader, "AABBs[" + to_string(i) + ']');
                    isProgressive = false;
                }

                ImGui::Text("Material id: %i", aabb.materialId);

                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Triangles", "Triangles (%d)", renderer.triangles.size()))
    {
        for (int i = 0; i < renderer.triangles.size(); i++)
        {
            Triangle triangle = renderer.triangles[i];

            if (ImGui::TreeNode((void*)(intptr_t)i, "Triangle %i", i))
            {
                if (ImGui::TreeNode("Vertex 1"))
                {
                    float position[3]{ triangle.pos1.x, triangle.pos1.y, triangle.pos1.z };
                    if (ImGui::DragFloat3("Position", position, .001f))
                    {
                        renderer.triangles[i].pos1 = sf::Vector3f(position[0], position[1], position[2]);
                        renderer.triangles[i].set(renderer.shader, "Triangles[" + to_string(i) + ']');
                        isProgressive = false;
                    }

                    float normal[3]{ triangle.normal1.x, triangle.normal1.y, triangle.normal1.z };
                    if (ImGui::DragFloat3("Normal", normal, .001f))
                    {
                        renderer.triangles[i].normal1 = normalized(sf::Vector3f(normal[0], normal[1], normal[2]));
                        renderer.triangles[i].set(renderer.shader, "Triangles[" + to_string(i) + ']');
                        isProgressive = false;
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Vertex 2"))
                {
                    float position[3]{ triangle.pos2.x, triangle.pos2.y, triangle.pos2.z };
                    if (ImGui::DragFloat3("Position", position, .001f))
                    {
                        renderer.triangles[i].pos2 = sf::Vector3f(position[0], position[1], position[2]);
                        renderer.triangles[i].set(renderer.shader, "Triangles[" + to_string(i) + ']');
                        isProgressive = false;
                    }

                    float normal[3]{ triangle.normal2.x, triangle.normal2.y, triangle.normal2.z };
                    if (ImGui::DragFloat3("Normal", normal, .001f))
                    {
                        renderer.triangles[i].normal2 = normalized(sf::Vector3f(normal[0], normal[1], normal[2]));
                        renderer.triangles[i].set(renderer.shader, "Triangles[" + to_string(i) + ']');
                        isProgressive = false;
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Vertex 3"))
                {
                    float position[3]{ triangle.pos3.x, triangle.pos3.y, triangle.pos3.z };
                    if (ImGui::DragFloat3("Position", position, .001f))
                    {
                        renderer.triangles[i].pos3 = sf::Vector3f(position[0], position[1], position[2]);
                        renderer.triangles[i].set(renderer.shader, "Triangles[" + to_string(i) + ']');
                        isProgressive = false;
                    }

                    float normal[3]{ triangle.normal3.x, triangle.normal3.y, triangle.normal3.z };
                    if (ImGui::DragFloat3("Normal", normal, .001f))
                    {
                        renderer.triangles[i].normal3 = normalized(sf::Vector3f(normal[0], normal[1], normal[2]));
                        renderer.triangles[i].set(renderer.shader, "Triangles[" + to_string(i) + ']');
                        isProgressive = false;
                    }

                    ImGui::TreePop();
                }

                ImGui::Text("Material id: %i", triangle.materialId);

                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void EnvironmentUI(Renderer& renderer, bool& isProgressive)
{
    ImGui::Begin("Envorinment");
    if (ImGui::TreeNode("Sun"))
    {
        float sunColor[3]{ renderer.environment.sunColor.x, renderer.environment.sunColor.y, renderer.environment.sunColor.z };
        if (ImGui::ColorEdit3("Color", sunColor, ImGuiColorEditFlags_Float))
        {
            renderer.environment.sunColor = sf::Vector3f(sunColor[0], sunColor[1], sunColor[2]);
            renderer.environment.set(renderer.shader, "Environment");
            isProgressive = false;
        }

        float sunDirection[3]{ renderer.environment.sunDirection.x, renderer.environment.sunDirection.y, renderer.environment.sunDirection.z };
        if (ImGui::DragFloat3("Direction", sunDirection, 0.001f))
        {
            renderer.environment.sunDirection = normalized(sf::Vector3f(sunDirection[0], sunDirection[1], sunDirection[2]));
            renderer.environment.set(renderer.shader, "Environment");
            isProgressive = false;
        }

        if (ImGui::DragFloat("Focus", &renderer.environment.sunFocus, 1, 0, 10000))
        {
            renderer.environment.set(renderer.shader, "Environment");
            isProgressive = false;
        }

        if (ImGui::DragFloat("Intensity", &renderer.environment.sunIntensity, 0.001f, 0, 100000))
        {
            renderer.environment.set(renderer.shader, "Environment");
            isProgressive = false;
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
            isProgressive = false;
        }

        float zenith[3]{ renderer.environment.skyColorZenith.x, renderer.environment.skyColorZenith.y, renderer.environment.skyColorZenith.z };
        if (ImGui::ColorEdit3("Zenith", zenith, ImGuiColorEditFlags_Float))
        {
            renderer.environment.skyColorZenith = sf::Vector3f(zenith[0], zenith[1], zenith[2]);
            renderer.environment.set(renderer.shader, "Environment");
            isProgressive = false;
        }

        ImGui::TreePop();
    }

    float ground[3]{ renderer.environment.groundColor.x, renderer.environment.groundColor.y, renderer.environment.groundColor.z };
    if (ImGui::ColorEdit3("Ground", ground, ImGuiColorEditFlags_Float))
    {
        renderer.environment.groundColor = sf::Vector3f(ground[0], ground[1], ground[2]);
        renderer.environment.set(renderer.shader, "Environment");
        isProgressive = false;
    }

    if (ImGui::Checkbox("Enable", &renderer.environment.enabled))
    {
        renderer.environment.set(renderer.shader, "Environment");
        isProgressive = false;
    }

    ImGui::End();
}
