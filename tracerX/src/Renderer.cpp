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

#pragma region Payload Structs
struct MeshInstancePayload
{
    glm::mat4 transform = glm::mat4(1);
    glm::mat4 transformInv = glm::mat4(1);
    int materialId = -1;
    int meshId = -1;
    int padding1 = 0;
    int padding2 = 0;
};

struct CameraPayload
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
    int padding1 = 0;
};

struct EnvironmentPayload
{
    glm::vec4 rotation1;
    glm::vec4 rotation2;
    glm::vec4 rotation3;
    int transparent;
    float intensity;
    float cdfTotal;
    int padding1 = 0;
};

struct AccumParamsPayload
{
    glm::ivec2 rectPosition;
    glm::ivec2 rectSize;
    unsigned int sampleCount;
    unsigned int maxDepth;
    unsigned int russianRouletteDepth;
    int padding1 = 0;
};

struct ToneMapParamsPayload
{
    glm::ivec2 rectPosition;
    glm::ivec2 rectSize;
    unsigned int sampleCount;
    unsigned int toneMapMode;
    float gamma;
};
#pragma endregion

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
        throw std::runtime_error("Failed to initialize GLEW: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(status))));
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
    this->accumShader.shutdown();
    this->toneMapShader.shutdown();
}

void Renderer::render(unsigned int samples, glm::uvec2 pos, glm::uvec2 size, bool updateSampleCount)
{
    this->accumulate(samples, pos, size);
    this->toneMap(pos, size);

    // Revert sampleCount if needed
    if (!updateSampleCount)
    {
        this->sampleCount -= samples;
    }
}

void Renderer::render(unsigned int samples)
{
    this->render(samples, glm::uvec2(0, 0), this->getSize());
}

void Renderer::accumulate(unsigned int samples, glm::uvec2 pos, glm::uvec2 size)
{
    // === Bind Data ===
    // Images
    this->accumulationTexture.bindImage(0, GL_READ_WRITE);
    this->albedoTexture.bindImage(1, GL_WRITE_ONLY);
    this->normalTexture.bindImage(2, GL_WRITE_ONLY);
    this->depthTexture.bindImage(3, GL_WRITE_ONLY);
    this->toneMapTexture.bindImage(4, GL_WRITE_ONLY);

    // Samplers
    this->environment.texture.bindSampler(0);
    this->environment.cdfTexture.bindSampler(1);
    this->textureArray.bindSampler(2);

    // SSBOs
    this->vertexBuffer.bindBuffer(0);
    this->triangleBuffer.bindBuffer(1);
    this->meshBuffer.bindBuffer(2);
    this->meshInstanceBuffer.bindBuffer(3);
    this->materialBuffer.bindBuffer(4);
    this->blasBuffer.bindBuffer(5);
    this->tlasBuffer.bindBuffer(6);

    // UBOs
    this->cameraBuffer.bindUniform(0);
    this->environmentBuffer.bindUniform(1);
    this->paramBuffer.bindUniform(2);
    // === Bind Data ===

    // === Update Data ===
    // cameraBuffer
    const CameraPayload cameraPayload
    {
        .position = this->camera.position,
        .fov = this->camera.fov,
        .forward = this->camera.forward,
        .focalDistance = this->camera.focalDistance,
        .up = this->camera.up,
        .aperture = this->camera.aperture,
        .blur = this->camera.blur,
        .zNear = this->camera.zNear,
        .zFar = this->camera.zFar,
    };
    this->cameraBuffer.update(&cameraPayload, sizeof(CameraPayload));

    // environmentBuffer
    const EnvironmentPayload environmentPayload
    {
        .rotation1 = glm::vec4(this->environment.rotation[0], 0),
        .rotation2 = glm::vec4(this->environment.rotation[1], 0),
        .rotation3 = glm::vec4(this->environment.rotation[2], 0),
        .transparent = this->environment.transparent,
        .intensity = this->environment.intensity,
        .cdfTotal = this->environment.cdfTotal,
    };
    this->environmentBuffer.update(&environmentPayload, sizeof(EnvironmentPayload));

    // paramBuffer
    const AccumParamsPayload paramsPayload
    {
        .rectPosition = pos,
        .rectSize = size,
        .sampleCount = this->sampleCount,
        .maxDepth = this->maxDepth,
        .russianRouletteDepth = this->russianRouletteDepth,
    };
    this->paramBuffer.update(&paramsPayload, sizeof(AccumParamsPayload));
    // === Update Data ===

    // Accumulate
    this->accumShader.use();
    for (unsigned int i = 0; i < samples; i++)
    {
        this->paramBuffer.update(&this->sampleCount, sizeof(this->sampleCount), offsetof(AccumParamsPayload, sampleCount));
        Shader::dispatchCompute(Shader::getGroups(size));
        this->sampleCount++;
    }

    Shader::stopUse();
}

void Renderer::toneMap(glm::uvec2 pos, glm::uvec2 size)
{
    // === Bind Data ===
    // Images
    this->accumulationTexture.bindImage(0, GL_READ_ONLY);
    this->toneMapTexture.bindImage(1, GL_WRITE_ONLY);

    // UBOs
    this->paramBuffer.bindUniform(0);
    // === Bind Data ===

    // === Update Data ===
    // paramBuffer
    const ToneMapParamsPayload paramsPayload
    {
        .rectPosition = pos,
        .rectSize = size,
        .sampleCount = this->sampleCount,
        .toneMapMode = static_cast<unsigned int>(this->toneMapMode),
        .gamma = this->gamma,
    };
    this->paramBuffer.update(&paramsPayload, sizeof(ToneMapParamsPayload));
    // === Update Data ===

    // Tone map
    this->toneMapShader.use();
    Shader::dispatchCompute(Shader::getGroups(size));
    Shader::stopUse();
}

#if TX_DENOISE
void Renderer::denoise(glm::uvec2 pos, glm::uvec2 size)
{
    // Create device
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    // Create color buffer
    const Image colorImage = this->accumulationTexture.upload(pos, size);
    oidn::BufferRef colorBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    colorBuf.writeAsync(0, colorImage.pixels.size() * sizeof(float), colorImage.pixels.data());

    // Create albedo buffer
    const Image albedoImage = this->albedoTexture.upload(pos, size);
    oidn::BufferRef albedoBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    albedoBuf.writeAsync(0, albedoImage.pixels.size() * sizeof(float), albedoImage.pixels.data());

    // Create normal buffer
    const Image normalImage = this->normalTexture.upload(pos, size);
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
    this->accumulationTexture.update(Image(size, pixels), pos);

    // Update output
    this->toneMap(pos, size);

    // Release buffers
    colorBuf.release();
    albedoBuf.release();
    normalBuf.release();
}

void Renderer::denoise()
{
    this->denoise(glm::uvec2(0, 0), this->getSize());
}
#endif

#if !NDEBUG
void Renderer::reloadShaders(const std::filesystem::path& shaderPath)
{
    this->accumShader.reload(shaderPath / "accumulate" / "main.comp");
    this->toneMapShader.reload(shaderPath / "toneMap" / "main.comp");
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

glm::uvec2 Renderer::getSize() const
{
    return this->accumulationTexture.getSize();
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
    this->triangleBuffer.update(scene.triangles.data(), scene.triangles.size() * sizeof(glm::uvec3));
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
    std::vector<BvhNode> tlas;
    std::vector<size_t> meshInstancePermutation;
    scene.buildTLAS(tlas, meshInstancePermutation);

    std::vector<MeshInstancePayload> meshInstancesPayload;
    meshInstancesPayload.reserve(scene.meshInstances.size());
    for (const size_t meshInstanceId : meshInstancePermutation)
    {
        const MeshInstance& meshInstance = scene.meshInstances[meshInstanceId];
        const MeshInstancePayload meshInstancePayload
        {
            .transform = meshInstance.transform,
            .transformInv = glm::inverse(meshInstance.transform),
            .materialId = meshInstance.materialId,
            .meshId = meshInstance.meshId,
        };
        meshInstancesPayload.push_back(meshInstancePayload);
    }

    this->meshInstanceBuffer.update(meshInstancesPayload.data(), meshInstancesPayload.size() * sizeof(MeshInstancePayload));
    this->tlasBuffer.update(tlas.data(), tlas.size() * sizeof(BvhNode));
}

void Renderer::initData()
{
    // Shader
#if TX_SPIRV
    this->accumShader.init(Renderer::accumShaderSrc, Renderer::accumShaderSrcSize);
    this->toneMapShader.init(Renderer::toneMapShaderSrc, Renderer::toneMapShaderSrcSize);
#else
    this->accumShader.init(Renderer::accumShaderSrc);
    this->toneMapShader.init(Renderer::toneMapShaderSrc);
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
