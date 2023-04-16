#pragma once

#include <fstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "AABB.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Camera.h"

using namespace std;

class Renderer
{
public:
    vector<Material> materials;
    vector<Sphere> spheres;
    vector<Triangle> triangles;
    vector<AABB> aabbs;
    Camera camera;
    Vector2i size;

    Renderer(Vector2i size, Camera camera)
        : camera(camera), size(size)
    {
        this->buffer1.create(this->size.x, this->size.y);
        this->buffer2.create(this->size.x, this->size.y);
    }

    void loadScene(int rayPerFrameCount, int maxBounceCount, bool enableEnvironment)
    {
        this->loadShader();
        this->shader.setUniform("WindowSize", (Vector2f)this->size);
        this->shader.setUniform("RayPerFrameCount", rayPerFrameCount);
        this->shader.setUniform("MaxBouceCount", maxBounceCount);
        this->shader.setUniform("FocusStrength", this->camera.focusStrength);
        this->shader.setUniform("FocalLength", this->camera.focalLength);
        this->shader.setUniform("EnableEnvironment", enableEnvironment);

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
    }

    void runVisual()
    {
        RenderWindow window(VideoMode(this->size.x, this->size.y), "Ray Tracing");
        Mouse::setPosition(this->size / 2, window);

        bool isProgressive = false;
        window.setMouseCursorVisible(false);
        RenderStates r;

        Clock clock;
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
                {
                    isProgressive = !isProgressive;
                    if (!isProgressive)
                    {
                        Mouse::setPosition(this->size / 2, window);
                        window.setMouseCursorVisible(false);
                    }
                    else
                    {
                        window.setMouseCursorVisible(true);
                    }
                }

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::R)
                {
                    this->buffer1.getTexture().copyToImage().saveToFile("image.png");
                    cout << endl << "Saved" << endl;
                }
            }

            cout << '\r' << setw(10) << "Frame count: " << this->frameCount << setw(10) << "FPS: " << 1.0f / clock.restart().asSeconds() << "   ";

            if (!isProgressive)
            {
                this->camera.move(window);
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
            window.display();
        }
    }

    void run(int iterationCount, const string imagePath)
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

    void add(Sphere sphere, const Material material)
    {
        int materialId = this->add(material);
        sphere.materialId = materialId;
        this->spheres.push_back(sphere);
    }

    void add(Sphere sphere)
    {
        this->spheres.push_back(sphere);
    }

    void add(AABB aabb, const Material material)
    {
        int materialId = this->add(material);
        aabb.materialId = materialId;
        this->aabbs.push_back(aabb);
    }

    void add(AABB aabb)
    {
        this->aabbs.push_back(aabb);
    }

    void add(Triangle triangle, const Material material)
    {
        int materialId = this->add(material);
        triangle.materialId = materialId;
        this->triangles.push_back(triangle);
    }

    void add(Triangle triangle)
    {
        this->triangles.push_back(triangle);
    }

    int add(const Material& material)
    {
        vector<Material>::iterator it = find(this->materials.begin(), this->materials.end(), material);
        if (it != this->materials.end())
        {
            return (int)(it - this->materials.begin());
        }

        this->materials.push_back(material);
        return (int)(this->materials.size() - 1);
    }

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource)
    {
        this->addCornellBox(up, down, left, right, forward, backward);
        this->add(AABB(Vector3f(0, 1.24f, 0), Vector3f(.5f, .5f, .5f)), lightSource);
    }

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward)
    {
        this->add(AABB(Vector3f(0, 3, 0), Vector3f(2, 4, 2)), up);
        this->add(AABB(Vector3f(0, -3, 0), Vector3f(2, 4, 2)), down);

        this->add(AABB(Vector3f(3, 0, 0), Vector3f(4, 2, 2)), left);
        this->add(AABB(Vector3f(-3, 0, 0), Vector3f(4, 2, 2)), right);

        this->add(AABB(Vector3f(0, 0, 3), Vector3f(2, 2, 4)), forward);
        this->add(AABB(Vector3f(0, 0, -3), Vector3f(2, 2, 4)), backward);
    }

private:
    const string path = "shader.frag";

    Shader shader;
    RenderTexture buffer1;
    RenderTexture buffer2;
    int frameCount = 0;

    void loadShader()
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

    void clear()
    {
        this->buffer1.clear();
        this->buffer1.display();
        this->buffer2.clear();
        this->buffer2.display();
        this->frameCount = 0;
    }
};