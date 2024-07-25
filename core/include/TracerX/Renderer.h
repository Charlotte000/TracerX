/**
 * @file Renderer.h
 */
#pragma once

#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Environment.h"
#include "TextureArray.h"
#include "StorageBuffer.h"
#include "UniformBuffer.h"

#include <vector>
#include <glm/glm.hpp>

namespace TracerX
{

/**
 * @brief Represents a renderer in the TracerX rendering engine.
 * 
 * The renderer uses a path tracing algorithm to render the scene.
 * The process consists of two stages:
 * 1. Accumulation: The renderer traces rays through the scene and accumulates the resulting colors.
 * 2. Tone mapping: The renderer applies tone mapping to the accumulated colors and displays the result.
 * 
 * The renderer uses a framebuffer to accumulate the colors and a texture array to store the scene data.
 * The scene data consists of the vertices, triangles, materials, and meshes of the scene.
 * 
 * The renderer can render a full frame or a rectangular region of the frame.
 * The renderer supports gamma correction and environment settings.
 * 
 * The renderer can also apply denoising to the rendered image.
 * 
 * The renderer is responsible for initializing OpenGL and GLEW.
 * 
 * @see Renderer::init
 * @see Renderer::render
 * @see Renderer::shutdown
 */
class Renderer
{
public:
    /**
     * @brief The camera used for rendering.
     */
    Camera camera;

    /**
     * @brief The maximum number of times a ray can bounce in the scene.
     */
    unsigned int maxBounceCount = 5;

    /**
     * @brief The gamma correction value used in tone mapping.
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
     * @see Environment::loadFromFile to load an environment from a file.
     */
    Environment environment;

    /**
     * @brief Initializes the renderer with the specified size.
     * 
     * Must be called before any other method. Initializes GLEW and OpenGL.
     * 
     * @param size The size of the renderer.
     * @throws std::runtime_error Thrown if GLEW fails to initialize.
     */
    void init(glm::uvec2 size);

    /**
     * @brief Resizes the renderer to the specified size.
     * @param size The new size of the renderer.
     */
    void resize(glm::uvec2 size);

    /**
     * @brief Shuts down the renderer and releases resources.
     * 
     * Must be called after all other methods. Releases all resources and shuts down GLEW and OpenGL.
     */
    void shutdown();

    /**
     * @brief Renders the scene.
     * 
     * Renders the scene using accumulation and tone mapping.
     * The sample count is incremented.
     * The rendered image can be accessed using Renderer::getImage().
     * 
     * @param samples The number of samples to render.
     * @see Renderer::renderRect to render only a rectangular region of the image.
     */
    void render(unsigned int samples = 1);

    /**
     * @brief Renders a rectangular region of the image.
     * 
     * Renders the specified region of the image using accumulation and tone mapping.
     * The sample count is incremented.
     * The rendered image can be accessed using Renderer::getImage().
     * 
     * @param samples The number of samples to render.
     * @param position The position of the top-left corner of the region.
     * @param size The size of the region.
     * @see Renderer::render to render the entire image.
     */
    void renderRect(unsigned int samples, glm::uvec2 position, glm::uvec2 size);

#ifdef TX_DENOISE
    /**
     * @brief Applies denoising to the rendered image.
     */
    void denoise();
#endif

    /**
     * @brief Clears the renderer.
     * 
     * Clears the accumulated colors and resets the sample count.
     * Should be called after any changes to the scene or environment.
     */
    void clear();

    /**
     * @brief Gets the OpenGL texture handler for the rendered image.
     * @return The texture handler.
     */
    GLuint getTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the albedo image.
     * 
     * The albedo image contains the color information of the first hit of the rays.
     * The color information does not include lighting or shadows and does not depend on the viewing angle.
     * 
     * @return The texture handler.
     */
    GLuint getAlbedoTextureHandler() const;

    // ToDo: documentation
    GLuint getNormalTextureHandler() const;

    // ToDo: documentation
    GLuint getDepthTextureHandler() const;

    // ToDo: documentation
    GLuint getAccumulatorTextureHandler() const;

    /**
     * @brief Loads the rendered texture from the GPU to the CPU.
     * @return The rendered image.
     * @see Image::saveToFile to save the image to a file.
     */
    Image getImage() const;

    /**
     * @brief Gets the size of the renderer.
     * 
     * The size is the size of the rendered image.
     * 
     * @return The size of the renderer.
     */
    glm::uvec2 getSize() const;

    /**
     * @brief Gets the sample count.
     * 
     * The sample count is the number of accumulated frames.
     * 
     * @return The sample count.
     */
    unsigned int getSampleCount() const;

    /**
     * @brief Loads the specified scene into the renderer.
     * 
     * Use this method to load the entire scene into the renderer.
     * 
     * @param scene The scene to load.
     * @param texturesSize The size of textures in the scene.
     * @see Renderer::updateSceneMaterials to update only the materials.
     * @see Renderer::updateSceneMeshes to update only the meshes.
     */
    void loadScene(const Scene& scene, glm::uvec2 texturesSize = glm::uvec2(2048, 2048));

    /**
     * @brief Updates the materials in the scene.
     * 
     * Use this method to update only the materials in the scene.
     * 
     * @param scene The scene containing the updated materials.
     * @see Renderer::loadScene to update the entire scene.
     */
    void updateSceneMaterials(const Scene& scene);

    /**
     * @brief Updates the meshes in the scene.
     * 
     * Use this method to update only the meshes in the scene.
     * 
     * @param scene The scene containing the updated meshes.
     * @see Renderer::loadScene to update the entire scene.
     */
    void updateSceneMeshes(const Scene& scene);
private:
    unsigned int sampleCount = 0;
    core::Shader shader;
    core::Texture accumulationTexture;
    core::Texture albedoTexture;
    core::Texture normalTexture;
    core::Texture depthTexture;
    core::Texture toneMapTexture;
    core::TextureArray textureArray;
    core::StorageBuffer vertexBuffer;
    core::StorageBuffer triangleBuffer;
    core::StorageBuffer meshBuffer;
    core::StorageBuffer materialBuffer;
    core::StorageBuffer bvhBuffer;
    core::UniformBuffer cameraBuffer;
    core::UniformBuffer environmentBuffer;
    core::UniformBuffer paramBuffer;

    static const std::vector<unsigned char> shaderSrc;

    void initData();
    void bindData();
    void updateUniform(glm::ivec2 uvLow, glm::ivec2 uvUp, bool onlyToneMapping);
};

}
