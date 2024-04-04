#pragma once

#include "Quad.h"
#include "Mesh.h"
#include "Image.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Texture.h"
#include "Material.h"
#include "Triangle.h"
#include "FrameBuffer.h"
#include "TextureArray.h"

#include <vector>
#include <string>
#include <glm/glm.hpp>


class Renderer
{
public:
    Camera camera;
    int maxBouceCount = 5;
    int frameCount = 1;
    float gamma = 2.2f;
    float environmentIntensity = 1.f;
    glm::mat3 environmentRotation = glm::mat3(1);
    FrameBuffer output;
    TextureArray textureArray;

    static inline const std::string shaderFolder = "../shaders/";

    void init();
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
    Quad quad;
    Shader pathTracer;
    Shader toneMapper;
    FrameBuffer accumulator;
    Texture environmentTexture;
    Buffer<Vertex> vertexBuffer;
    Buffer<Triangle> triangleBuffer;
    Buffer<Mesh> meshBuffer;
    Buffer<Material> materialBuffer;
    Buffer<glm::vec3> bvhBuffer;

    void initData();
    void updateShaders();
};
