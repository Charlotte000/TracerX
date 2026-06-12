/**
 * @file Renderer.cpp
 */
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <GL/glew.h>

#if TX_DENOISE
#include <OpenImageDenoise/oidn.hpp>
#endif

#include "TracerX/Renderer.h"

using namespace TracerX;
using namespace TracerX::core;

static inline glm::uvec3 getGroupCount(glm::uvec3 globalSize, glm::uvec3 localSize = glm::uvec3(16, 16, 1))
{
    return glm::uvec3(glm::ceil(glm::vec3(globalSize) / glm::vec3(localSize)));
}

static inline OGL::Image3D createImage3D(const std::vector<OGL::Image2D>& images, glm::uvec2 maxTextureSize)
{
    // Calculate image size
    glm::uvec2 maxSize(0);
    for (const OGL::Image2D& img : images)
    {
        maxSize = glm::max(maxSize, img.size);
    }
    maxSize = glm::min(maxSize, maxTextureSize);


    std::vector<glm::vec4> pixels;
    for (const OGL::Image2D& img : images)
    {
        OGL::Image2D resizedImg = img.resize(maxSize);
        pixels.insert(pixels.end(), resizedImg.pixels.begin(), resizedImg.pixels.end());
    }

    return OGL::Image3D(glm::uvec3(maxSize, images.size()), pixels.data());
}

#if !NDEBUG
static inline std::string loadShader(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Shader source not found: " + path.string());
    }

    const std::string includeIdentifier = "#include";

    std::string code;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.find(includeIdentifier) != line.npos)
        {
            // Get include path (remove #include and quotes)
            line.erase(0, includeIdentifier.size() + 2);
            line.pop_back();

            code += loadShader(path.parent_path() / line) + '\n';
            continue;
        }

        code += line + '\n';
    }

    return code;
}
#endif

#if TX_SPIRV
extern const uint8_t accumShaderSrc[];
extern const uint8_t toneMapShaderSrc[];
extern const size_t accumShaderSrcSize;
extern const size_t toneMapShaderSrcSize;
#else
extern const char accumShaderSrc[];
extern const char toneMapShaderSrc[];
#endif

Renderer::Renderer(glm::uvec2 size)
    :
    // Shader
#if TX_SPIRV
    accumShader  ({ OGL::Shader(OGL::ShaderType::COMPUTE, reinterpret_cast<const void*>(accumShaderSrc),   accumShaderSrcSize  )}),
    toneMapShader({ OGL::Shader(OGL::ShaderType::COMPUTE, reinterpret_cast<const void*>(toneMapShaderSrc), toneMapShaderSrcSize)}),
#else
    accumShader  ({ OGL::Shader(OGL::ShaderType::COMPUTE, accumShaderSrc  ) }),
    toneMapShader({ OGL::Shader(OGL::ShaderType::COMPUTE, toneMapShaderSrc) }),
#endif
    // Textures
    frameBuffer({
        { OGL::Attachment::COLOR0, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<2, OGL::Filter>(OGL::Filter::NEAREST)) }, // ToneMap
        { OGL::Attachment::COLOR1, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<2, OGL::Filter>(OGL::Filter::NEAREST)) }, // Accumulator
        { OGL::Attachment::COLOR2, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<2, OGL::Filter>(OGL::Filter::NEAREST)) }, // Albedo
        { OGL::Attachment::COLOR3, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<2, OGL::Filter>(OGL::Filter::NEAREST)) }, // Normal
        { OGL::Attachment::COLOR4, OGL::Texture2D(size, OGL::ImageFormat::R32F,    glm::vec<2, OGL::Filter>(OGL::Filter::NEAREST)) }, // Depth
    }),
    textureArray(glm::uvec3(1), OGL::ImageFormat::RGBA32F)
{
}

OGL::Texture2D& Renderer::albedoTexture()
{
    return this->frameBuffer[OGL::Attachment::COLOR2];
}

const OGL::Texture2D& Renderer::albedoTexture() const
{
    return this->frameBuffer[OGL::Attachment::COLOR2];
}

OGL::Texture2D& Renderer::normalTexture()
{
    return this->frameBuffer[OGL::Attachment::COLOR3];
}

const OGL::Texture2D& Renderer::normalTexture() const
{
    return this->frameBuffer[OGL::Attachment::COLOR3];
}

OGL::Texture2D& Renderer::depthTexture()
{
    return this->frameBuffer[OGL::Attachment::COLOR4];
}

const OGL::Texture2D& Renderer::depthTexture() const
{
    return this->frameBuffer[OGL::Attachment::COLOR4];
}

OGL::Texture2D& Renderer::accumulatorTexture()
{
    return this->frameBuffer[OGL::Attachment::COLOR1];
}

const OGL::Texture2D& Renderer::accumulatorTexture() const
{
    return this->frameBuffer[OGL::Attachment::COLOR1];
}

OGL::Texture2D& Renderer::toneMapTexture()
{
    return this->frameBuffer[OGL::Attachment::COLOR0];
}

const OGL::Texture2D& Renderer::toneMapTexture() const
{
    return this->frameBuffer[OGL::Attachment::COLOR0];
}

void Renderer::resize(glm::uvec2 size)
{
    this->frameBuffer = OGL::FrameBuffer(
    {
        { OGL::Attachment::COLOR0, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<3, OGL::Filter>(OGL::Filter::NEAREST)) }, // ToneMap
        { OGL::Attachment::COLOR1, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<3, OGL::Filter>(OGL::Filter::NEAREST)) }, // Accumulator
        { OGL::Attachment::COLOR2, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<3, OGL::Filter>(OGL::Filter::NEAREST)) }, // Albedo
        { OGL::Attachment::COLOR3, OGL::Texture2D(size, OGL::ImageFormat::RGBA32F, glm::vec<3, OGL::Filter>(OGL::Filter::NEAREST)) }, // Normal
        { OGL::Attachment::COLOR4, OGL::Texture2D(size, OGL::ImageFormat::R32F,    glm::vec<3, OGL::Filter>(OGL::Filter::NEAREST)) }, // Depth
    });

    // Clear for safety
    this->clear();
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
    this->accumulatorTexture().bindImage(0, OGL::ImageUnitFormat::RGBA32F, OGL::Access::READ_WRITE);
    this->albedoTexture().bindImage(     1, OGL::ImageUnitFormat::RGBA32F, OGL::Access::WRITE_ONLY);
    this->normalTexture().bindImage(     2, OGL::ImageUnitFormat::RGBA32F, OGL::Access::WRITE_ONLY);
    this->depthTexture().bindImage(      3, OGL::ImageUnitFormat::R32F,    OGL::Access::WRITE_ONLY);

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
    const struct
    {
        glm::vec3 position;
        float fov;
        glm::vec3 forward;
        float focalDistance;
        glm::vec3 up;
        float aperture;
        glm::vec3 right;
        float blur;
        float zNear;
        float zFar;
        int padding1 = 0;
        int padding2 = 0;
    } cameraPayload
    {
        .position = this->camera.position,
        .fov = this->camera.fov,
        .forward = this->camera.forward,
        .focalDistance = this->camera.focalDistance,
        .up = this->camera.up,
        .aperture = this->camera.aperture,
        .right = glm::cross(this->camera.forward, this->camera.up),
        .blur = this->camera.blur,
        .zNear = this->camera.zNear,
        .zFar = this->camera.zFar,
    };
    this->cameraBuffer.write(&cameraPayload, sizeof(cameraPayload));

    // environmentBuffer
    const struct
    {
        glm::vec4 rotation1;
        glm::vec4 rotation2;
        glm::vec4 rotation3;
        int transparent;
        float intensity;
        float cdfTotal;
        int padding1 = 0;
    } environmentPayload
    {
        .rotation1 = glm::vec4(this->environment.rotation[0], 0),
        .rotation2 = glm::vec4(this->environment.rotation[1], 0),
        .rotation3 = glm::vec4(this->environment.rotation[2], 0),
        .transparent = this->environment.transparent,
        .intensity = this->environment.intensity,
        .cdfTotal = this->environment.cdfTotal,
    };
    this->environmentBuffer.write(&environmentPayload, sizeof(environmentPayload));

    // paramBuffer
    const struct
    {
        glm::ivec2 rectPosition;
        glm::ivec2 rectSize;
        unsigned int sampleCount;
        unsigned int maxDepth;
        unsigned int russianRouletteDepth;
        int padding1 = 0;
    } paramsPayload
    {
        .rectPosition = pos,
        .rectSize = size,
        .sampleCount = this->sampleCount,
        .maxDepth = this->maxDepth,
        .russianRouletteDepth = this->russianRouletteDepth,
    };
    this->paramBuffer.write(&paramsPayload, sizeof(paramsPayload));
    // === Update Data ===

    // Accumulate
    this->accumShader.use();
    for (unsigned int i = 0; i < samples; i++)
    {
        this->paramBuffer.update(&this->sampleCount, offsetof(decltype(paramsPayload), sampleCount), sizeof(this->sampleCount));
        OGL::Program::dispatchCompute(getGroupCount(glm::uvec3(size, 1)));
        this->sampleCount++;
    }

    OGL::Program::stopUse();
}

void Renderer::toneMap(glm::uvec2 pos, glm::uvec2 size)
{
    // === Bind Data ===
    // Images
    this->accumulatorTexture().bindImage(0, OGL::ImageUnitFormat::RGBA32F, OGL::Access::READ_ONLY);
    this->toneMapTexture().bindImage(1, OGL::ImageUnitFormat::RGBA32F, OGL::Access::WRITE_ONLY);

    // UBOs
    this->paramBuffer.bindUniform(0);
    // === Bind Data ===

    // === Update Data ===
    // paramBuffer
    const struct
    {
        glm::ivec2 rectPosition;
        glm::ivec2 rectSize;
        unsigned int sampleCount;
        unsigned int toneMapMode;
        float gamma;
    } paramsPayload
    {
        .rectPosition = pos,
        .rectSize = size,
        .sampleCount = this->sampleCount,
        .toneMapMode = static_cast<unsigned int>(this->toneMapMode),
        .gamma = this->gamma,
    };
    this->paramBuffer.write(&paramsPayload, sizeof(paramsPayload));
    // === Update Data ===

    // Tone map
    this->toneMapShader.use();
    OGL::Program::dispatchCompute(getGroupCount(glm::uvec3(size, 1)));
    OGL::Program::stopUse();
}

#if TX_DENOISE
void Renderer::denoise(glm::uvec2 pos, glm::uvec2 size)
{
    // Create device
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    // Create color buffer
    const OGL::Image2D colorImage = this->accumulatorTexture().read(pos, size);
    oidn::BufferRef colorBuf = device.newBuffer(colorImage.pixels.size() * sizeof(glm::vec4));
    colorBuf.writeAsync(0, colorImage.pixels.size() * sizeof(glm::vec4), colorImage.pixels.data());

    // Create albedo buffer
    const OGL::Image2D albedoImage = this->albedoTexture().read(pos, size);
    oidn::BufferRef albedoBuf = device.newBuffer(albedoImage.pixels.size() * sizeof(glm::vec4));
    albedoBuf.writeAsync(0, albedoImage.pixels.size() * sizeof(glm::vec4), albedoImage.pixels.data());

    // Create normal buffer
    const OGL::Image2D normalImage = this->normalTexture().read(pos, size);
    oidn::BufferRef normalBuf = device.newBuffer(normalImage.pixels.size() * sizeof(glm::vec4));
    normalBuf.writeAsync(0, normalImage.pixels.size() * sizeof(glm::vec4), normalImage.pixels.data());

    // Create filter
    device.sync();
    oidn::FilterRef filter = device.newFilter("RT");
    filter.setImage("color", colorBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.setImage("albedo", albedoBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.setImage("normal", normalBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.setImage("output", colorBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
    filter.set("cleanAux", true);
    filter.set("quality", oidn::Quality::High);
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
    this->accumulatorTexture().update(OGL::Image2D(size, reinterpret_cast<const glm::vec4*>(colorBuf.getData())), pos);

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
    this->accumShader   = OGL::Program({ OGL::Shader(OGL::ShaderType::COMPUTE, loadShader(shaderPath / "accumulate" / "main.comp").c_str()) });
    this->toneMapShader = OGL::Program({ OGL::Shader(OGL::ShaderType::COMPUTE, loadShader(shaderPath / "toneMap" / "main.comp").c_str())    });
}
#endif

void Renderer::clear()
{
    this->accumulatorTexture().clear();
    this->albedoTexture().clear();
    this->normalTexture().clear();
    this->depthTexture().clear();
    this->toneMapTexture().clear();

    this->sampleCount = 0;
}

glm::uvec2 Renderer::getSize() const
{
    return this->frameBuffer.size();
}

unsigned int Renderer::getSampleCount() const
{
    return this->sampleCount;
}

void Renderer::loadScene(Scene& scene, glm::uvec2 maxTextureArraySize)
{
    // Textures
    OGL::Image3D img = createImage3D(scene.textures, maxTextureArraySize);
    this->textureArray = OGL::Texture2DArray(img.size, OGL::ImageFormat::RGBA32F);
    this->textureArray.update(img, glm::uvec3(0));

    // SSBOs
    this->vertexBuffer.write(scene.vertices.data(), scene.vertices.size() * sizeof(Vertex));
    this->triangleBuffer.write(scene.triangles.data(), scene.triangles.size() * sizeof(glm::uvec3));
    this->meshBuffer.write(scene.meshes.data(), scene.meshes.size() * sizeof(Mesh));
    this->updateSceneMeshInstances(scene);
    this->updateSceneMaterials(scene);
    this->blasBuffer.write(scene.blas.data(), scene.blas.size() * sizeof(BvhNode));
}

void Renderer::updateSceneMaterials(const Scene& scene)
{
    this->materialBuffer.write(scene.materials.data(), scene.materials.size() * sizeof(Material));
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
    };

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

    this->meshInstanceBuffer.write(meshInstancesPayload.data(), meshInstancesPayload.size() * sizeof(MeshInstancePayload));
    this->tlasBuffer.write(tlas.data(), tlas.size() * sizeof(BvhNode));
}
