#pragma once

#include "Quad.h"
#include "Mesh.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Material.h"
#include "Triangle.h"
#include "Environment.h"
#include "FrameBuffer.h"
#include "TextureArray.h"

#include <vector>
#include <glm/glm.hpp>

namespace TracerX
{

class Renderer
{
public:
    core::Camera camera;
    int maxBouceCount = 5;
    int frameCount = 0;
    float gamma = 2.2f;
    core::Environment environment;
    core::FrameBuffer output;
    core::TextureArray textureArray;

    void init();
    void resize(glm::ivec2 size);
    void shutdown();
    void render(size_t count = 1);
#ifdef TX_DENOISE
    void denoise();
#endif
    void resetAccumulator();
    void resetMeshes(const std::vector<core::Mesh>& meshes);
    void resetBVH(const std::vector<glm::vec3>& bvh, const std::vector<core::Triangle> triangles);
    void resetMaterials(const std::vector<core::Material>& materials);
    void resetScene(Scene& scene);
private:
    core::Quad quad;
    core::Shader pathTracer;
    core::Shader toneMapper;
    core::FrameBuffer accumulator;
    core::Buffer<core::Vertex> vertexBuffer;
    core::Buffer<core::Triangle> triangleBuffer;
    core::Buffer<core::Mesh> meshBuffer;
    core::Buffer<core::Material> materialBuffer;
    core::Buffer<glm::vec3> bvhBuffer;

    static const char* pathTracerShaderSrc;
    static const char* toneMapperShaderSrc;
    static const char* vertexShaderSrc;

    void initData();
    void updatePathTracerShader();
    void updateToneMapperShader();
};

}
