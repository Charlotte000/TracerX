#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "AABB.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Camera.h"
#include "Environment.h"

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
    Environment environment;
    Shader shader;
    int frameCount = 0;
    int rayPerFrameCount = 0;
    int maxBounceCount = 0;

    Renderer(Vector2i size, Camera camera, int rayPerFrameCount, int maxBounceCount);

    void loadScene();

    void runVisual();

    void run(int iterationCount, const string imagePath);

    void add(Sphere sphere, const Material material);

    void add(Sphere sphere);

    void add(AABB aabb, const Material material);

    void add(AABB aabb);

    void add(Triangle triangle, const Material material);

    void add(Triangle triangle);

    int add(const Material& material);

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource);

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward);

private:
    const string path = "shader.frag";
    RenderTexture buffer1;
    RenderTexture buffer2;

    void loadShader();

    void clear();
};