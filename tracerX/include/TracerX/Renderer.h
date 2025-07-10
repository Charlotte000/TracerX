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
 * 1. Accumulation: The renderer traces rays through the scene and accumulates the resulting colors using Renderer::accumulate method.
 * 
 * 2. Tone mapping: The renderer applies tone mapping to the accumulated colors and displays the result using Renderer::toneMap method.
 * 
 * The renderer provides the Renderer::accumulationTexture as the intermediate result of the accumulation stage and the Renderer::toneMapTexture as the final result of the tone mapping stage.
 * Also, there are Renderer::albedoTexture, Renderer::normalTexture, and Renderer::depthTexture for additional information about the render.
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
 * TracerX::Scene scene = TracerX::loadGLTF("scene.glb");
 * 
 * TracerX::Renderer renderer;
 * renderer.init(glm::uvec2(800, 600));
 * renderer.loadScene(scene);
 * renderer.render(100);
 * renderer.denoise();
 * const TracerX::Image image = renderer.toneMapTexture.upload();
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
    unsigned int maxDepth = 5;

    /**
     * @brief The maximum number of times a ray can bounce in the scene before Russian roulette is applied.
     * 
     * If a ray bounces more than the specified depth, it has a chance to terminate early.
     * If the value is 0, Russian roulette is disabled.
     */
    unsigned int russianRouletteDepth = 0;

    /**
     * @brief The gamma correction value used in tone mapping.
     */
    float gamma = 2.2f;

    /**
     * @brief The environment settings for the scene.
     * @see Environment::update to load an environment image from a file.
     */
    Environment environment;

    /**
     * @brief Determines how the renderer maps the colors to the display.
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
     * @brief The texture contains the information about the albedo (diffuse) colors of the scene.
     * 
     * It represents the base color of the surface at each pixel and contains no lighting information.
     */
    core::GL::Texture albedoTexture;

    /**
     * @brief The texture contains the information about the normals of the scene.
     * 
     * It represents the direction of the surface normals at each pixel.
     * To get the world-space normal, use the formula: normal = 2 * color - 1.
     */
    core::GL::Texture normalTexture;

    /**
     * @brief The texture contains the information about the depth of the scene.
     * 
     * It represents the distance from the camera to the closest object in the scene.
     * The depth information is encoded non-linearly in the red channel within the range [0, 1].
     * To get a linear depth, use the formula: linearDepth = 2 * near * far / (far + near - depth * (far - near)).
     * Where near is the minimum render distance and far is the maximum render distance.
     */
    core::GL::Texture depthTexture;

    /**
     * @brief The texture contains the information about the accumulated colors of the scene.
     * 
     * It represents the accumulated colors of the scene over multiple samples.
     */
    core::GL::Texture accumulationTexture;

    /**
     * @brief The texture contains the information about the tone mapped colors and hence the final output image.
     * 
     * It represents the final image that is the result of the rendering process.
     */
    core::GL::Texture toneMapTexture;

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
     * The rendered texture can be accessed using Renderer::toneMapTexture.
     * 
     * @param samples The number of samples per pixel to accumulate.
     * @see Renderer::renderRect to render only a rectangular region of the image.
     */
    void render(unsigned int samples = 1);

    /**
     * @brief Renders a rectangular region of the image.
     * 
     * Renders the specified region of the image using accumulation and tone mapping.
     * The sample count is incremented if updateSampleCount is true.
     * The rendered texture can be accessed using Renderer::toneMapTexture.
     * 
     * @param samples The number of samples per pixel to accumulate.
     * @param pos The position of the top-left corner of the region of the image to render.
     * @param size The size of the region of the image to render.
     * @param updateSampleCount Whether to update the sample count.
     * @see Renderer::render to render the entire image.
     */
    void renderRect(unsigned int samples, glm::uvec2 pos, glm::uvec2 size, bool updateSampleCount = true);

    /**
     * @brief Accumulates the rendered image.
     * 
     * This method performs path tracing of the scene and accumulates the resulting colors in the Renderer::accumulationTexture.
     * The sample count is incremented by the specified number of samples.
     * 
     * @param samples The number of samples per pixel to accumulate.
     * @param pos The position of the top-left corner of the region of the image to accumulate.
     * @param size The size of the region of the image to accumulate.
     * @see Renderer::toneMap to apply tone mapping to the accumulated image.
     */
    void accumulate(unsigned int samples, glm::uvec2 pos, glm::uvec2 size);

    /**
     * @brief Applies tone mapping to the rendered image.
     * 
     * This method applies tone mapping to the accumulated colors of the Renderer::accumulationTexture and stores the result in the Renderer::toneMapTexture.
     * 
     * @see Renderer::toneMapMode for the available tone mapping modes.
     * @see Renderer::gamma for the gamma correction value used in tone mapping.
     */
    void toneMap();

#if TX_DENOISE
    /**
     * @brief Applies denoising to the rendered image.
     * 
     * Use this method after rendering the scene to reduce noise.
     * 
     * @throws std::runtime_error Thrown if the denoising fails.
     */
    void denoise();
#endif

#if !NDEBUG
    /**
     * @brief Reloads the shaders.
     * 
     * Use this method after changing the shader source code.
     * For debugging purposes only.
     * 
     * @param shaderPath The path to the shader source code directory.
     * @throws std::runtime_error Thrown if the shaders fail to reload.
     */
    void reloadShaders(const std::filesystem::path& shaderPath);
#endif

    /**
     * @brief Clears the renderer.
     * 
     * Clears the accumulated colors and resets the sample count.
     * Should be called after any changes to the scene or environment.
     */
    void clear();

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
     * Builds the BVH tree of the scene (BLAS and TLAS), which may be time-consuming.
     * 
     * @param scene The scene to load.
     * @param maxTextureArraySize The maximum size of the textures used in the scene. For low-end devices.
     * @see Renderer::updateSceneMaterials to update only the materials.
     * @see Renderer::updateSceneMeshInstances to update only the mesh instances.
     */
    void loadScene(Scene& scene, glm::uvec2 maxTextureArraySize = glm::uvec2(-1));

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
     * Rebuilds the BVH tree of the scene (only TLAS).
     * 
     * @param scene The scene containing the updated mesh instances.
     * @see Renderer::loadScene to update the entire scene.
     */
    void updateSceneMeshInstances(Scene& scene);
private:
    unsigned int sampleCount = 0;
    core::GL::Shader accumShader;
    core::GL::Shader toneMapShader;
    core::GL::TextureArray textureArray;
    core::GL::StorageBuffer vertexBuffer;
    core::GL::StorageBuffer triangleBuffer;
    core::GL::StorageBuffer meshBuffer;
    core::GL::StorageBuffer meshInstanceBuffer;
    core::GL::StorageBuffer materialBuffer;
    core::GL::StorageBuffer blasBuffer;
    core::GL::StorageBuffer tlasBuffer;
    core::GL::UniformBuffer cameraBuffer;
    core::GL::UniformBuffer environmentBuffer;
    core::GL::UniformBuffer paramBuffer;

#if TX_SPIRV
    static const unsigned char accumShaderSrc[];
    static const size_t accumShaderSrcSize;
    static const unsigned char toneMapShaderSrc[];
    static const size_t toneMapShaderSrcSize;
#else
    static const char accumShaderSrc[];
    static const char toneMapShaderSrc[];
#endif
    void initData();
    void bindData();
    void updateUniform(glm::ivec2 rectPosition, glm::ivec2 rectSize);
};

}
