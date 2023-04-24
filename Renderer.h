#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "AABB.h"
#include "Sphere.h"
#include "Camera.h"
#include "Environment.h"
#include "Vertex3.h"
#include "Mesh.h"

using namespace std;

class Renderer
{
public:
    vector<Material> materials;
    vector<Sphere> spheres;
    vector<AABB> aabbs;
    vector<int> indices;
    vector<Vertex3> vertices;
    vector<Mesh> meshes;
    Camera camera;
    Vector2i size;
    Environment environment;
    Shader shader;
    int frameCount = 1;
    int subStage = 0;
    int sampleCount = 0;
    int maxBounceCount = 0;
    Vector2i subDivisor = Vector2i(1, 1);

    Renderer(Vector2i size, Camera camera, int sampleCount, int maxBounceCount);

    void loadScene();

    void run(int iterationCount, const string imagePath);

    int getPixelDifference();

    void add(Sphere sphere, const Material material);

    void add(Sphere sphere);

    void add(AABB aabb, const Material material);

    void add(AABB aabb);

    int add(const Material& material);

    void addFile(const string filePath, Vector3f offset = Vector3f(0, 0, 0), Vector3f scale = Vector3f(1, 1, 1), Vector3f rotation = Vector3f(0, 0, 0));

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource);

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward);
protected:
    const string path = "shader.frag";
    RenderTexture buffer1;
    RenderTexture buffer2;

    void loadShader();

    void clear();
};