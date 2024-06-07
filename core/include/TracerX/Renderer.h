/**
 * @file Renderer.h
 */
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

/**
 * @brief Renderer class that handles rendering of the scene.
 */
class Renderer
{
public:
    /**
     * @brief The camera used for rendering.
     */
    Camera camera;
    
    /**
     * @brief The maximum number of bounces for ray.
     */
    unsigned int maxBouceCount = 5;

    /**
     * @brief The gamma correction value for the rendered image.
     */
    float gamma = 2.2f;

    /**
     * @brief The minimum distance for rendering objects.
     */
    float minRenderDistance = .0001f;

    /**
     * @brief The maximum distance for rendering objects.
     */
    float maxRenderDistance = 1000000;

    /**
     * @brief The environment settings for the scene.
     */
    Environment environment;

    /**
     * @brief Initializes the renderer with the specified size. 
     * 
     * @param size The size of the renderer.
     * @throws std::runtime_error Thrown if GLEW fails to initialize.
     */
    void init(glm::uvec2 size);

    /**
     * @brief Resizes the renderer to the specified size.
     * 
     * @param size The new size of the renderer.
     */
    void resize(glm::uvec2 size);

    /**
     * @brief Shuts down the renderer and releases resources.
     * 
     */
    void shutdown();

    /**
     * @brief Renders the scene.
     * 
     * @param count The number of frames to render.
     */
    void render(unsigned int count = 1);

    /**
     * @brief Renders a rectangular region of the image.
     * 
     * @param count The number of frames to render.
     * @param position The position of the top-left corner of the region.
     * @param size The size of the region.
     */
    void renderRect(unsigned int count, glm::uvec2 position, glm::uvec2 size);

#ifdef TX_DENOISE
    /**
     * @brief Applies denoising to the rendered image.
     */
    void denoise();
#endif

    /**
     * @brief Clears the renderer's output.
     */
    void clear();

    /**
     * @brief Gets the OpenGL texture handler for the rendered image.
     * 
     * @return The texture handler.
     */
    GLuint getTextureHandler() const;

    /**
     * @brief Gets the rendered image.
     * 
     * @return The rendered image.
     */
    Image getImage() const;

    /**
     * @brief Gets the size of the renderer.
     * 
     * @return The size of the renderer.
     */
    glm::uvec2 getSize() const;

    /**
     * @brief Gets the frame count.
     * 
     * @return The frame count.
     */
    unsigned int getFrameCount() const;

    /**
     * @brief Loads the specified scene into the renderer.
     * 
     * @param scene The scene to load.
     */
    void loadScene(const Scene& scene);

    /**
     * @brief Updates the materials in the scene.
     * 
     * @param scene The scene containing the updated materials.
     */
    void updateSceneMaterials(const Scene& scene);

    /**
     * @brief Updates the meshes in the scene.
     * 
     * @param scene The scene containing the updated meshes.
     */
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
