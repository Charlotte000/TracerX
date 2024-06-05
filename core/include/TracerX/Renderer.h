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
    Camera camera;
    unsigned int maxBouceCount = 5;
    float gamma = 2.2f;
    float minRenderDistance = .0001f;
    float maxRenderDistance = 1000000;
    Environment environment;

    void init(glm::uvec2 size);
    void resize(glm::uvec2 size);
    void shutdown();
    void render(unsigned int count = 1);
    void renderRect(unsigned int count, glm::uvec2 position, glm::uvec2 size);
#ifdef TX_DENOISE
    void denoise();
#endif
    void clear();
    GLuint getTextureHandler() const;
    Image getImage() const;
    glm::uvec2 getSize() const;
    unsigned int getFrameCount() const;
    void loadScene(const Scene& scene);
    void updateSceneMaterials(const Scene& scene);
    void updateSceneMeshes(const Scene& scene);
private:
    unsigned int frameCount = 0;
    core::Quad quad;
    core::Shader pathTracer;
    core::Shader toneMapper;
    core::FrameBuffer accumulator;
    core::FrameBuffer output;
    core::TextureArray textureArray;
    core::Buffer<core::Vertex> vertexBuffer;
    core::Buffer<core::Triangle> triangleBuffer;
    core::Buffer<core::Mesh> meshBuffer;
    core::Buffer<core::Material> materialBuffer;
    core::Buffer<glm::vec3> bvhBuffer;

    static const char* pathTracerShaderSrc;
    static const char* toneMapperShaderSrc;
    static const char* vertexShaderSrc;

    void initData();
};

}
