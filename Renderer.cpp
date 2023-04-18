#include "Renderer.h"

#include <fstream>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "RendererUI.h"

using namespace std;

Renderer::Renderer(Vector2i size, Camera camera, int rayPerFrameCount, int maxBounceCount)
    : camera(camera), size(size), rayPerFrameCount(rayPerFrameCount), maxBounceCount(maxBounceCount)
{
    this->buffer1.create(this->size.x, this->size.y);
    this->buffer2.create(this->size.x, this->size.y);
}

void Renderer::loadScene()
{
    this->loadShader();
    this->shader.setUniform("WindowSize", (Vector2f)this->size);
    this->shader.setUniform("RayPerFrameCount", this->rayPerFrameCount);
    this->shader.setUniform("MaxBouceCount", this->maxBounceCount);
    this->shader.setUniform("FocusStrength", this->camera.focusStrength);
    this->shader.setUniform("FocalLength", this->camera.focalLength);

    for (int i = 0; i < this->materials.size(); i++)
    {
        this->materials[i].set(this->shader, "Materials[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->spheres.size(); i++)
    {
        this->spheres[i].set(this->shader, "Spheres[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->triangles.size(); i++)
    {
        this->triangles[i].set(this->shader, "Triangles[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->aabbs.size(); i++)
    {
        this->aabbs[i].set(this->shader, "AABBs[" + to_string(i) + ']');
    }

    this->environment.set(this->shader, "Environment");
}

void Renderer::runVisual()
{
    RenderWindow window(VideoMode(this->size.x, this->size.y), "Ray Tracing");
    Mouse::setPosition(this->size / 2, window);

    ImGui::SFML::Init(window);

    bool isProgressive = false;
    bool isCameraControl = false;
    window.setMouseCursorVisible(false);
    char fileName[50] = "image.png";
    size_t fileNameSize = 50 * sizeof(char);

    Clock clock;
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter && isCameraControl)
            {
                isCameraControl = false;
            }
        }

        ImGui::SFML::Update(window, clock.restart());

        InfoUI(*this, isProgressive, isCameraControl, window, this->buffer1, fileName, fileNameSize);
        MaterailUI(*this, isProgressive);
        GeometryUI(*this, isProgressive);
        EnvironmentUI(*this, isProgressive);

        if (isCameraControl)
        {
            this->camera.move(window);
        }

        if (!isProgressive)
        {
            this->clear();
        }

        this->frameCount++;
        this->shader.setUniform("FrameCount", this->frameCount);
        this->shader.setUniform("CameraPosition", this->camera.position);
        this->shader.setUniform("CameraForward", this->camera.forward);
        this->shader.setUniform("CameraUp", this->camera.up);

        RenderTexture* newBuffer = (this->frameCount & 1) == 0 ? &this->buffer1 : &this->buffer2;
        RenderTexture* oldBuffer = (this->frameCount & 1) == 0 ? &this->buffer2 : &this->buffer1;

        newBuffer->clear();
        newBuffer->draw(Sprite(oldBuffer->getTexture()), &this->shader);
        newBuffer->display();

        window.clear();
        window.draw(Sprite(newBuffer->getTexture()));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

void Renderer::run(int iterationCount, const string imagePath)
{
    this->shader.setUniform("CameraPosition", this->camera.position);
    this->shader.setUniform("CameraForward", this->camera.forward);
    this->shader.setUniform("CameraUp", this->camera.up);

    while (this->frameCount < iterationCount)
    {
        this->frameCount++;
        this->shader.setUniform("FrameCount", this->frameCount);

        RenderTexture* newBuffer = (this->frameCount & 1) == 0 ? &this->buffer1 : &this->buffer2;
        RenderTexture* oldBuffer = (this->frameCount & 1) == 0 ? &this->buffer2 : &this->buffer1;

        newBuffer->clear();
        newBuffer->draw(Sprite(oldBuffer->getTexture()), &this->shader);
        newBuffer->display();
    }

    this->buffer1.getTexture().copyToImage().saveToFile(imagePath);
}

void Renderer::add(Sphere sphere, const Material material)
{
    int materialId = this->add(material);
    sphere.materialId = materialId;
    this->spheres.push_back(sphere);
}

void Renderer::add(Sphere sphere)
{
    this->spheres.push_back(sphere);
}

void Renderer::add(AABB aabb, const Material material)
{
    int materialId = this->add(material);
    aabb.materialId = materialId;
    this->aabbs.push_back(aabb);
}

void Renderer::add(AABB aabb)
{
    this->aabbs.push_back(aabb);
}

void Renderer::add(Triangle triangle, const Material material)
{
    int materialId = this->add(material);
    triangle.materialId = materialId;
    this->triangles.push_back(triangle);
}

void Renderer::add(Triangle triangle)
{
    this->triangles.push_back(triangle);
}

int Renderer::add(const Material& material)
{
    vector<Material>::iterator it = find(this->materials.begin(), this->materials.end(), material);
    if (it != this->materials.end())
    {
        return (int)(it - this->materials.begin());
    }

    this->materials.push_back(material);
    return (int)(this->materials.size() - 1);
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource)
{
    this->addCornellBox(up, down, left, right, forward, backward);
    this->add(AABB(Vector3f(0, .998f, 0), Vector3f(.5f, .001f, .5f)), lightSource);
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward)
{
    this->add(AABB(Vector3f(0, 1.5f, -1), Vector3f(2, 1, 4)), up);
    this->add(AABB(Vector3f(0, -1.5f, -1), Vector3f(2, 1, 4)), down);

    this->add(AABB(Vector3f(1.5f, 0, -1), Vector3f(1, 2, 4)), left);
    this->add(AABB(Vector3f(-1.5f, 0, -1), Vector3f(1, 2, 4)), right);

    this->add(AABB(Vector3f(0, 0, 1.5f), Vector3f(2, 2, 1)), forward);
    this->add(AABB(Vector3f(0, 0, -3.5f), Vector3f(2, 2, 1)), backward);
}

void Renderer::loadShader()
{
    ifstream myFile(this->path);
    string content;
    content.assign(istreambuf_iterator<char>(myFile), istreambuf_iterator<char>());
    myFile.close();

    if (!this->shader.loadFromMemory(
        (!this->triangles.empty() ? "#define TriangleCount " + to_string(this->triangles.size()) + "\n" : "") +
        (!this->spheres.empty() ? "#define SphereCount " + to_string(this->spheres.size()) + "\n" : "") +
        (!this->aabbs.empty() ? "#define AABBCount " + to_string(this->aabbs.size()) + "\n" : "") +
        (!this->materials.empty() ? "#define MaterialCount " + to_string(this->materials.size()) + "\n" : "") +
        content,
        Shader::Fragment))
    {
        cout << "Error" << endl;
    }
}

void Renderer::clear()
{
    this->buffer1.clear();
    this->buffer1.display();
    this->buffer2.clear();
    this->buffer2.display();
    this->frameCount = 0;
}