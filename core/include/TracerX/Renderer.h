/**
 * @file Renderer.h
 */
#pragma once

#include "TracerX/Scene.h"
#include "TracerX/Environment.h"
#include "TracerX/core/GL/Shader.h"
#include "TracerX/core/GL/TextureArray.h"
#include "TracerX/core/GL/StorageBuffer.h"
#include "TracerX/core/GL/UniformBuffer.h"

namespace TracerX
{

/**
 * @brief The main class of the TracerX rendering engine.
 * 
 * The renderer uses a path tracing algorithm to render the scene.
 * 
 * The process consists of two stages:
 * 
 * 1. Accumulation: The renderer traces rays through the scene and accumulates the resulting colors.
 * 
 * 2. Tone mapping: The renderer applies tone mapping to the accumulated colors and displays the result.
 * 
 * An OpenGL compute shader is utilized to trace rays and update the image.
 * It is responsible for initializing and destroying the OpenGL and GLEW contexts.
 * 
 * The renderer can handle both full-frame and rectangular region rendering.
 * It supports gamma correction and environment settings.
 * Additionally, the renderer can apply denoising to the final image.
 * 
 * The renderer can be used in the following way:
 * @code {.cpp}
 * TracerX::Renderer renderer;
 * renderer.init(glm::uvec2(800, 600));
 * renderer.loadScene(scene);
 * renderer.render(100);
 * renderer.denoise();
 * TracerX::Image image = renderer.getImage();
 * image.saveToFile("output.png");
 * renderer.shutdown();
 * @endcode
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
     * @brief Determines how the renderer maps the colors to the display.
     * 
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
    }
    /**
     * @brief The tone mapping mode used by the renderer.
     */
    toneMapMode = ToneMapMode::Reinhard;

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
     * @brief Resizes the rendered image to the specified size.
     * @param size The new size of the image.
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
     * The sample count is incremented if updateSampleCount is true.
     * The rendered image can be accessed using Renderer::getImage().
     * 
     * @param samples The number of samples to render.
     * @param rectPosition The position of the top-left corner of the region.
     * @param rectSize The size of the region.
     * @param updateSampleCount Whether to update the sample count.
     * @see Renderer::render to render the entire image.
     */
    void renderRect(unsigned int samples, glm::uvec2 rectPosition, glm::uvec2 rectSize, bool updateSampleCount = true);

    /**
     * @brief Applies tone mapping to the rendered image.
     * 
     * This method does not path trace the scene. It only updates the tone mapped image.
     * Use this method after changing the tone mapping settings or the gamma value.
     */
    void toneMap();

#ifdef TX_DENOISE
    /**
     * @brief Applies denoising to the rendered image.
     * 
     * Use this method after rendering the scene to reduce noise.
     * 
     * @throws std::runtime_error Thrown if the denoising fails.
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
     * @see Renderer::getImage to load the image from the GPU to the CPU.
     */
    GLuint getTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the albedo image.
     * 
     * The albedo image contains the color information of the first hit of the rays.
     * The color information does not include lighting or shadows and does not depend on the viewing angle.
     * 
     * @return The texture handler.
     * @see Renderer::getAlbedoImage to load the image from the GPU to the CPU.
     */
    GLuint getAlbedoTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the normal image.
     * 
     * The normal image contains the normal information of the first hit of the rays.
     * To get the world-space normal, use the formula: normal = 2 * color - 1.
     * 
     * @return The texture handler.
     * @see Renderer::getNormalImage to load the image from the GPU to the CPU.
     */
    GLuint getNormalTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the depth image.
     * 
     * The depth image contains the depth information of the first hit of the rays.
     * The depth information is encoded non-linearly in the red channel within the range [0, 1].
     * To get a linear depth, use the formula: linearDepth = 2 * near * far / (far + near - depth * (far - near)).
     * Where near is the minimum render distance and far is the maximum render distance.
     * 
     * @return The texture handler.
     * @see Renderer::getDepthImage to load the image from the GPU to the CPU.
     */
    GLuint getDepthTextureHandler() const;

    /**
     * @brief Gets the OpenGL texture handler for the accumulation image.
     * 
     * The accumulation image contains the accumulated colors of the rendered image.
     * 
     * @return The texture handler.
     * @see Renderer::getAccumulatorImage to load the image from the GPU to the CPU.
     */
    GLuint getAccumulatorTextureHandler() const;

    /**
     * @brief Loads the rendered texture from the GPU to the CPU.
     * @return The rendered image.
     * @see Image::saveToFile to save the image to a file.
     * @see Renderer::getTextureHandler to get the OpenGL texture handler.
     */
    Image getImage() const;

    /**
     * @brief Loads the albedo texture from the GPU to the CPU.
     * 
     * The albedo image contains the color information of the first hit of the rays.
     * The color information does not include lighting or shadows and does not depend on the viewing angle.
     * 
     * @return The albedo image.
     * @see Image::saveToFile to save the image to a file.
     * @see Renderer::getAlbedoTextureHandler to get the OpenGL texture handler.
     */
    Image getAlbedoImage() const;

    /**
     * @brief Loads the normal texture from the GPU to the CPU.
     * 
     * The normal image contains the normal information of the first hit of the rays.
     * To get the world-space normal, use the formula: normal = 2 * color - 1.
     * 
     * @return The normal image.
     * @see Image::saveToFile to save the image to a file.
     * @see Renderer::getNormalTextureHandler to get the OpenGL texture handler.
     */
    Image getNormalImage() const;

    /**
     * @brief Loads the depth texture from the GPU to the CPU.
     * 
     * The depth image contains the depth information of the first hit of the rays.
     * The depth information is encoded non-linearly in the red channel within the range [0, 1].
     * To get a linear depth, use the formula: linearDepth = 2 * near * far / (far + near - depth * (far - near)).
     * Where near is the minimum render distance and far is the maximum render distance.
     * 
     * @return The depth image.
     * @see Image::saveToFile to save the image to a file.
     * @see Renderer::getDepthTextureHandler to get the OpenGL texture handler.
     */
    Image getDepthImage() const;

    /**
     * @brief Loads the accumulation texture from the GPU to the CPU.
     * 
     * The accumulation image contains the accumulated colors of the rendered image.
     * 
     * @return The accumulation image.
     * @see Image::saveToFile to save the image to a file.
     * @see Renderer::getAccumulatorTextureHandler to get the OpenGL texture handler.
     */
    Image getAccumulatorImage() const;

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
     * Use this method to load the entire scene into the GPU.
     * Keep in mind that this method builds the BVH tree of the scene, which may be time-consuming.
     * 
     * @param scene The scene to load.
     * @see Renderer::updateSceneMaterials to update only the materials.
     * @see Renderer::updateSceneMeshes to update only the meshes.
     */
    void loadScene(Scene& scene);

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
    void updateSceneMeshInstances(Scene& scene);
private:
    unsigned int sampleCount = 0;
    core::GL::Shader shader;
    core::GL::Texture accumulationTexture;
    core::GL::Texture albedoTexture;
    core::GL::Texture normalTexture;
    core::GL::Texture depthTexture;
    core::GL::Texture toneMapTexture;
    core::GL::TextureArray textureArray;
    core::GL::StorageBuffer vertexBuffer;
    core::GL::StorageBuffer triangleBuffer;
    core::GL::StorageBuffer meshBuffer;
    core::GL::StorageBuffer meshInstanceBuffer;
    core::GL::StorageBuffer materialBuffer;
    core::GL::StorageBuffer bvhBuffer;
    core::GL::UniformBuffer cameraBuffer;
    core::GL::UniformBuffer environmentBuffer;
    core::GL::UniformBuffer paramBuffer;

    static const std::vector<unsigned char> shaderSrc;

    void initData();
    void bindData();
    void updateUniform(glm::ivec2 rectPosition, glm::ivec2 rectSize, bool onlyToneMapping);
};

}
