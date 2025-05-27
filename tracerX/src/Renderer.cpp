/**
 * @file Renderer.cpp
 */
#include "TracerX/Renderer.h"

#include <iostream>
#include <stdexcept>
#include <GL/glew.h>
#if TX_DENOISE
#include <OpenImageDenoise/oidn.hpp>
#endif

using namespace TracerX;
using namespace TracerX::core;
using namespace TracerX::core::GL;

#if !NDEBUG
// https://learnopengl.com/In-Practice/Debugging
void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
#endif

void Renderer::init(glm::uvec2 size)
{
    // Init GLEW
    if (const GLenum status = glewInit(); status != GLEW_OK && status != GLEW_ERROR_NO_GLX_DISPLAY)
    {
        throw std::runtime_error((const char*)glewGetErrorString(status));
    }

#if !NDEBUG
    // Debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

    // Init GPU data
    this->initData();

    // Set size
    this->resize(size);
}

void Renderer::resize(glm::uvec2 size)
{
    // Resize textures
    this->accumulationTexture.resize(size);
    this->albedoTexture.resize(size);
    this->normalTexture.resize(size);
    this->depthTexture.resize(size);
    this->toneMapTexture.resize(size);

    // Clear for safety
    this->clear();
}

void Renderer::shutdown()
{
    // Textures
    this->accumulationTexture.shutdown();
    this->albedoTexture.shutdown();
    this->normalTexture.shutdown();
    this->depthTexture.shutdown();
    this->toneMapTexture.shutdown();
    this->environment.texture.shutdown();
    this->environment.cdfTexture.shutdown();
    this->textureArray.shutdown();

    // SSBOs
    this->vertexBuffer.shutdown();
    this->triangleBuffer.shutdown();
    this->meshBuffer.shutdown();
    this->meshInstanceBuffer.shutdown();
    this->materialBuffer.shutdown();
    this->blasBuffer.shutdown();
    this->tlasBuffer.shutdown();

    // UBOs
    this->cameraBuffer.shutdown();
    this->environmentBuffer.shutdown();
    this->paramBuffer.shutdown();

    // Shader
    this->shader.shutdown();
}

void Renderer::render(unsigned int samples)
{
    this->renderRect(samples, glm::uvec2(0, 0), this->getSize());
}

void Renderer::renderRect(unsigned int samples, glm::uvec2 rectPosition, glm::uvec2 rectSize, bool updateSampleCount)
{
    // Update UBOs
    this->bindData();
    this->updateUniform(rectPosition, rectSize, false);

    // Render
    this->shader.use();
    for (unsigned int i = 0; i < samples; i++)
    {
        this->paramBuffer.updateSub(&this->sampleCount, sizeof(this->sampleCount), sizeof(int) * 4);
        Shader::dispatchCompute(Shader::getGroups(rectSize));
        this->sampleCount++;
    }

    Shader::stopUse();

    // Revert sampleCount if needed
    if (!updateSampleCount)
    {
        this->sampleCount -= samples;
    }
}

void Renderer::toneMap()
{
    // Update UBOs
    this->bindData();
    this->updateUniform(glm::ivec2(0), this->getSize(), true);

    // Tone map
    this->shader.use();
    Shader::dispatchCompute(Shader::getGroups(this->getSize()));
    Shader::stopUse();
}

#if TX_DENOISE
void Renderer::denoise()
{
    // Create device
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    const glm::uvec2 size = this->getSize();

    // Create color buffer
    const Image colorImage = this->accumulationTexture.upload();
    oidn::BufferRef colorBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    colorBuf.writeAsync(0, colorImage.pixels.size() * sizeof(float), colorImage.pixels.data());

    // Create albedo buffer
    const Image albedoImage = this->albedoTexture.upload();
    oidn::BufferRef albedoBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    albedoBuf.writeAsync(0, albedoImage.pixels.size() * sizeof(float), albedoImage.pixels.data());

    // Create normal buffer
    const Image normalImage = this->normalTexture.upload();
    oidn::BufferRef normalBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    normalBuf.writeAsync(0, normalImage.pixels.size() * sizeof(float), normalImage.pixels.data());

    // Create filter
    device.sync();
    oidn::FilterRef filter = device.newFilter("RT");
    filter.setImage("color", colorBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.setImage("albedo", albedoBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.setImage("normal", normalBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.setImage("output", colorBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.set("hdr", true);
    filter.commit();

    // Denoise
    filter.execute();
    if (const char* errorMessage; device.getError(errorMessage) != oidn::Error::None)
    {
        // Release buffers
        colorBuf.release();
        albedoBuf.release();
        normalBuf.release();

        throw std::runtime_error("Failed to denoise: " + std::string(errorMessage));
    }

    // Update accumulator
    const float* data = (const float*)colorBuf.getData();
    const std::vector<float> pixels(data, data + colorImage.pixels.size());
    this->accumulationTexture.update(Image::loadFromMemory(size, pixels));

    // Update output
    this->toneMap();

    // Release buffers
    colorBuf.release();
    albedoBuf.release();
    normalBuf.release();
}
#endif

#if !NDEBUG
void Renderer::reloadShaders(const std::filesystem::path& shaderPath)
{
    this->shader.reload(shaderPath);
}
#endif

void Renderer::clear()
{
    this->accumulationTexture.clear();
    this->albedoTexture.clear();
    this->normalTexture.clear();
    this->depthTexture.clear();
    this->toneMapTexture.clear();

    this->sampleCount = 0;
}

unsigned int Renderer::getTextureHandler() const
{
    return this->toneMapTexture.getHandler();
}

unsigned int Renderer::getAlbedoTextureHandler() const
{
    return this->albedoTexture.getHandler();
}

unsigned int Renderer::getNormalTextureHandler() const
{
    return this->normalTexture.getHandler();
}

unsigned int Renderer::getDepthTextureHandler() const
{
    return this->depthTexture.getHandler();
}

unsigned int Renderer::getAccumulatorTextureHandler() const
{
    return this->accumulationTexture.getHandler();
}

Image Renderer::getImage() const
{
    return this->toneMapTexture.upload();
}

Image Renderer::getAlbedoImage() const
{
    return this->albedoTexture.upload();
}

Image Renderer::getNormalImage() const
{
    return this->normalTexture.upload();
}

Image Renderer::getDepthImage() const
{
    return this->depthTexture.upload();
}

Image Renderer::getAccumulatorImage() const
{
    return this->accumulationTexture.upload();
}

glm::uvec2 Renderer::getSize() const
{
    return this->accumulationTexture.size;
}

unsigned int Renderer::getSampleCount() const
{
    return this->sampleCount;
}

void Renderer::loadScene(Scene& scene, glm::uvec2 maxTextureArraySize)
{
    // Textures
    this->textureArray.update(scene.textures, maxTextureArraySize);

    // SSBOs
    this->vertexBuffer.update(scene.vertices.data(), scene.vertices.size() * sizeof(Vertex));
    this->triangleBuffer.update(scene.triangles.data(), scene.triangles.size() * sizeof(Triangle));
    this->meshBuffer.update(scene.meshes.data(), scene.meshes.size() * sizeof(Mesh));
    this->updateSceneMeshInstances(scene);
    this->updateSceneMaterials(scene);
    this->blasBuffer.update(scene.blas.data(), scene.blas.size() * sizeof(BvhNode));
}

void Renderer::updateSceneMaterials(const Scene& scene)
{
    this->materialBuffer.update(scene.materials.data(), scene.materials.size() * sizeof(Material));
}

void Renderer::updateSceneMeshInstances(Scene& scene)
{
    struct MeshInstancePayload
    {
        glm::mat4 transform = glm::mat4(1);
        glm::mat4 transformInv = glm::mat4(1);
        int materialId = -1;
        int meshId = -1;
        int padding1 = 0;
        int padding2 = 0;

        MeshInstancePayload(const MeshInstance& meshInstance)
            :
            transform(meshInstance.transform),
            transformInv(glm::inverse(meshInstance.transform)),
            materialId(meshInstance.materialId),
            meshId(meshInstance.meshId)
        {
        }
    };

    std::vector<BvhNode> tlas;
    std::vector<size_t> meshInstancePermutation;
    scene.buildTLAS(tlas, meshInstancePermutation);

    std::vector<MeshInstancePayload> meshInstancesPayload;
    meshInstancesPayload.reserve(scene.meshInstances.size());
    for (const size_t meshInstanceId : meshInstancePermutation)
    {
        const MeshInstance& meshInstance = scene.meshInstances[meshInstanceId];
        meshInstancesPayload.emplace_back(meshInstance);
    }

    this->meshInstanceBuffer.update(meshInstancesPayload.data(), meshInstancesPayload.size() * sizeof(MeshInstancePayload));
    this->tlasBuffer.update(tlas.data(), tlas.size() * sizeof(BvhNode));
}

void Renderer::initData()
{
    // Shader
#if TX_SPIRV
    this->shader.init(Renderer::shaderSrc, Renderer::shaderSrcSize);
#else
    this->shader.init(Renderer::shaderSrc);
#endif

    // Textures
    this->accumulationTexture.init(GL_RGBA32F, GL_NEAREST);
    this->albedoTexture.init(GL_RGBA32F, GL_NEAREST);
    this->normalTexture.init(GL_RGBA32F, GL_NEAREST);
    this->depthTexture.init(GL_R32F, GL_NEAREST);
    this->toneMapTexture.init(GL_RGBA32F, GL_NEAREST);
    this->environment.texture.init(GL_RGBA32F, GL_LINEAR);
    this->environment.cdfTexture.init(GL_R32F, GL_LINEAR);
    this->textureArray.init(GL_RGBA32F);

    // SSBOs
    this->vertexBuffer.init();
    this->triangleBuffer.init();
    this->meshBuffer.init();
    this->meshInstanceBuffer.init();
    this->materialBuffer.init();
    this->blasBuffer.init();
    this->tlasBuffer.init();

    // UBOs
    this->cameraBuffer.init();
    this->environmentBuffer.init();
    this->paramBuffer.init();
}

void Renderer::bindData()
{
    // Images
    this->accumulationTexture.bindImage(0, GL_READ_WRITE);
    this->albedoTexture.bindImage(1, GL_WRITE_ONLY);
    this->normalTexture.bindImage(2, GL_WRITE_ONLY);
    this->depthTexture.bindImage(3, GL_WRITE_ONLY);
    this->toneMapTexture.bindImage(4, GL_WRITE_ONLY);

    // Textures
    this->environment.texture.bind(0);
    this->environment.cdfTexture.bind(1);
    this->textureArray.bind(2);

    // SSBOs
    this->vertexBuffer.bind(0);
    this->triangleBuffer.bind(1);
    this->meshBuffer.bind(2);
    this->meshInstanceBuffer.bind(3);
    this->materialBuffer.bind(4);
    this->blasBuffer.bind(5);
    this->tlasBuffer.bind(6);

    // UBOs
    this->cameraBuffer.bind(0);
    this->environmentBuffer.bind(1);
    this->paramBuffer.bind(2);
}

void Renderer::updateUniform(glm::ivec2 rectPosition, glm::ivec2 rectSize, bool onlyToneMapping)
{
    // cameraBuffer
    const struct
    {
        glm::vec3 position;
        float fov;
        glm::vec3 forward;
        float focalDistance;
        glm::vec3 up;
        float aperture;
        float blur;
        float zNear;
        float zFar;
        int padding1;
    } camera
    {
        this->camera.position,
        this->camera.fov,
        this->camera.forward,
        this->camera.focalDistance,
        this->camera.up,
        this->camera.aperture,
        this->camera.blur,
        this->camera.zNear,
        this->camera.zFar,
        0,
    };
    this->cameraBuffer.update(&camera, sizeof(camera));

    // environmentBuffer
    const struct
    {
        glm::vec4 rotation1;
        glm::vec4 rotation2;
        glm::vec4 rotation3;
        int transparent;
        float intensity;
        float cdfTotal;
        int padding2;
    } environment
    {
        glm::vec4(this->environment.rotation[0], 0),
        glm::vec4(this->environment.rotation[1], 0),
        glm::vec4(this->environment.rotation[2], 0),
        this->environment.transparent,
        this->environment.intensity,
        this->environment.cdfTotal,
        0,
    };
    this->environmentBuffer.update(&environment, sizeof(environment));

    // paramBuffer
    const struct
    {
        glm::ivec2 rectPosition;
        glm::ivec2 rectSize;
        unsigned int sampleCount;
        unsigned int maxDepth;
        unsigned int russianRouletteDepth;
        unsigned int onlyToneMapping;
        unsigned int toneMapMode;
        float gamma;
        int padding2;
        int padding3;
    } params
    {
        rectPosition,
        rectSize,
        this->sampleCount,
        this->maxDepth,
        this->russianRouletteDepth,
        onlyToneMapping,
        static_cast<unsigned int>(this->toneMapMode),
        this->gamma,
        0,
        0,
    };
    this->paramBuffer.update(&params, sizeof(params));
}
