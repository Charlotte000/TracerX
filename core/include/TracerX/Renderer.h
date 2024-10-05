/**
 * @file Renderer.h
 */
#pragma once

#include "Scene.h"
#include "Image.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Environment.h"
#include "TextureArray.h"
#include "StorageBuffer.h"
#include "UniformBuffer.h"

#include <vector>
#include <GL/glew.h>
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
 * The renderer uses a compute shader to trace the rays and update the colors.
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
 * @see Renderer::loadScene
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
    float minRenderDistance = .1f;

    /**
     * @brief The maximum distance for rendering objects.
     */
    float maxRenderDistance = 1000;

    /**
     * @brief The environment settings for the scene.
     * @see Environment::loadFromFile to load an environment from a file.
     */
    Environment environment;

    /**
     * @brief The tone mapping mode used in rendering.
     * 
     * The tone mapping mode determines how the renderer maps the colors to the display.
     */
    enum class ToneMapMode : unsigned int
    {
        /**
         * @brief Reinhard tone mapping.
         * 
         * The Reinhard tone mapping algorithm maps the colors to the display using a simple formula: color / (color + 1).
         * This algorithm is simple and fast but may not produce the best results.
         */
        Reinhard = 0,

        /**
         * @brief ACES tone mapping.
         * 
         * The ACES tone mapping algorithm maps the colors to the display using the Academy Color Encoding System (ACES).
         * This algorithm is more complex and slower but produces better results.
         */
        ACES = 1,

        /**
         * @brief ACES fitted tone mapping.
         * 
         * The ACES fitted tone mapping algorithm maps the colors to the display using a simplified version of the ACES algorithm.
         * This algorithm is faster than the full ACES algorithm but still produces good results.
         */
        ACESfitted = 2,
    } toneMapMode = ToneMapMode::Reinhard;

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
     * @param rectPosition The position of the top-left corner of the region.
     * @param rectSize The size of the region.
     * @see Renderer::render to render the entire image.
     */
    void renderRect(unsigned int samples, glm::uvec2 rectPosition, glm::uvec2 rectSize);

    /**
     * @brief Applies tone mapping to the rendered image.
     * 
     * This method does not path trace the scene. It only updates the tone mapped image.
     */
    void toneMap();

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

    /**
     * @brief Gets the OpenGL texture handler for the normal image.
     * 
     * The normal image contains the normal information of the first hit of the rays.
     * 
     * @return The texture handler.
     */
    GLuint getNormalTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the depth image.
     * 
     * The depth image contains the depth information of the first hit of the rays.
     * 
     * @return The texture handler.
     */
    GLuint getDepthTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the accumulation image.
     * 
     * The accumulation image contains the accumulated colors of the rendered image.
     * 
     * @return The texture handler.
     */
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
     * @see Renderer::updateSceneMaterials to update only the materials.
     * @see Renderer::updateSceneMeshes to update only the meshes.
     */
    void loadScene(const Scene& scene);

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
     * @brief Updates the mesh instances in the scene.
     * 
     * Use this method to update only the mesh instances in the scene.
     * 
     * @param scene The scene containing the updated mesh instances.
     * @see Renderer::loadScene to update the entire scene.
     */
    void updateSceneMeshInstances(const Scene& scene);
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
    core::StorageBuffer meshInstanceBuffer;
    core::StorageBuffer materialBuffer;
    core::StorageBuffer bvhBuffer;
    core::UniformBuffer cameraBuffer;
    core::UniformBuffer environmentBuffer;
    core::UniformBuffer paramBuffer;

    static const std::vector<unsigned char> shaderSrc;

    void initData();
    void bindData();
    void updateUniform(glm::ivec2 rectPosition, glm::ivec2 rectSize, bool onlyToneMapping);
};

}
