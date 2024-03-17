#pragma once

#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Buffer.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "TextureArray.h"

#include <glm/glm.hpp>


class Renderer
{
public:
    Camera camera;
    int maxBouceCount = 5;
    int frameCount = 1;
    float gamma = 2.2f;
    float environmentIntensity = 1.f;
    FrameBuffer output;
    TextureArray textureArray;

    void init(glm::ivec2 size, Scene& scene);
    void resize(glm::ivec2 size);
    void shutdown();
    void render();
    void denoise();
    void resetAccumulator();
    void resetMeshes(const std::vector<Mesh>& meshes);
    void resetBVH(const std::vector<glm::vec3>& bvh, const std::vector<Triangle> triangles);
    void resetMaterials(const std::vector<Material>& materials);
    void resetEnvironment(const Image& image);
    void resetScene(Scene& scene);
private:
    Shader pathTracer;
    Shader toneMapper;
    FrameBuffer accumulator;
    Texture environmentTexture;
    Buffer<Vertex> vertexBuffer;
    Buffer<Triangle> triangleBuffer;
    Buffer<Mesh> meshBuffer;
    Buffer<Material> materialBuffer;
    Buffer<glm::vec3> bvhBuffer;

    void initData(glm::ivec2 size, Scene& scene);
    void updateShaders();
};
