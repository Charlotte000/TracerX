/**
 * @file Renderer.cpp
 */
#include "TracerX/Renderer.h"

#include <iostream>
#ifdef TX_DENOISE
#include <OpenImageDenoise/oidn.hpp>
#endif

using namespace TracerX;
using namespace TracerX::core;

void Renderer::init(glm::uvec2 size)
{
    // Init GLEW
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        throw std::runtime_error((const char*)glewGetErrorString(status));
    }

    this->shader.init(Renderer::shaderSrc);

    this->initData();

    this->resize(size);
}

void Renderer::resize(glm::uvec2 size)
{
    this->accumulationTexture.resize(size);
    this->albedoTexture.resize(size);
    this->normalTexture.resize(size);
    this->depthTexture.resize(size);
    this->toneMapTexture.resize(size);
    this->clear();
}

void Renderer::shutdown()
{
    this->accumulationTexture.shutdown();
    this->albedoTexture.shutdown();
    this->normalTexture.shutdown();
    this->depthTexture.shutdown();
    this->toneMapTexture.shutdown();
    this->environment.texture.shutdown();
    this->textureArray.shutdown();

    this->vertexBuffer.shutdown();
    this->triangleBuffer.shutdown();
    this->meshBuffer.shutdown();
    this->meshInstanceBuffer.shutdown();
    this->materialBuffer.shutdown();
    this->bvhBuffer.shutdown();

    this->cameraBuffer.shutdown();
    this->environmentBuffer.shutdown();
    this->paramBuffer.shutdown();

    this->shader.shutdown();
}

void Renderer::render(unsigned int samples)
{
    this->renderRect(samples, glm::uvec2(0, 0), this->accumulationTexture.size);
}

void Renderer::renderRect(unsigned int samples, glm::uvec2 rectPosition, glm::uvec2 rectSize)
{
    this->bindData();
    this->updateUniform(rectPosition, rectSize, false);

    this->shader.use();
    for (unsigned int i = 0; i < samples; i++)
    {
        this->paramBuffer.updateSub(&this->sampleCount, sizeof(this->sampleCount), sizeof(int) * 4);
        Shader::dispatchCompute(Shader::getGroups(rectSize));
        this->sampleCount++;
    }

    Shader::stopUse();
}

void Renderer::toneMap()
{
    this->bindData();
    this->updateUniform(glm::ivec2(0), this->accumulationTexture.size, true);

    this->shader.use();
    Shader::dispatchCompute(Shader::getGroups(this->accumulationTexture.size));
    Shader::stopUse();
}

#ifdef TX_DENOISE
void Renderer::denoise()
{
    // Create device
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    glm::uvec2 size = this->accumulationTexture.size;

    // Create color buffer
    Image colorImage = this->accumulationTexture.upload();
    oidn::BufferRef colorBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    colorBuf.writeAsync(0, colorImage.pixels.size() * sizeof(float), colorImage.pixels.data());

    // Create albedo buffer
    Image albedoImage = this->albedoTexture.upload();
    oidn::BufferRef albedoBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    albedoBuf.writeAsync(0, albedoImage.pixels.size() * sizeof(float), albedoImage.pixels.data());

    // Create normal buffer
    Image normalImage = this->normalTexture.upload();
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
    const char* errorMessage;
    if (device.getError(errorMessage) != oidn::Error::None)
    {
        std::cerr << "Failed to denoise: " << errorMessage << std::endl;
    }
    else
    {
        // Update accumulator
        float* data = (float*)colorBuf.getData();
        std::vector<float> pixels(data, data + colorImage.pixels.size());
        this->accumulationTexture.update(Image::loadFromMemory(size, pixels));

        // Update output
        this->toneMap();
    }

    // Release buffers
    colorBuf.release();
    albedoBuf.release();
    normalBuf.release();
}
#endif

void Renderer::clear()
{
    this->accumulationTexture.clear();
    this->sampleCount = 0;
}

GLuint Renderer::getTextureHandler() const
{
    return this->toneMapTexture.getHandler();
}

GLuint Renderer::getAlbedoTextureHandler() const
{
    return this->albedoTexture.getHandler();
}

GLuint Renderer::getNormalTextureHandler() const
{
    return this->normalTexture.getHandler();
}

GLuint Renderer::getDepthTextureHandler() const
{
    return this->depthTexture.getHandler();
}

GLuint Renderer::getAccumulatorTextureHandler() const
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

void Renderer::loadScene(const Scene& scene)
{
    this->textureArray.update(scene.textures);
    this->bvhBuffer.update(scene.bvh.data(), scene.bvh.size() * sizeof(Node));
    this->vertexBuffer.update(scene.vertices.data(), scene.vertices.size() * sizeof(Vertex));
    this->triangleBuffer.update(scene.triangles.data(), scene.triangles.size() * sizeof(Triangle));
    this->meshBuffer.update(scene.meshes.data(), scene.meshes.size() * sizeof(Mesh));

    this->updateSceneMeshInstances(scene);
    this->updateSceneMaterials(scene);
}

void Renderer::updateSceneMaterials(const Scene& scene)
{
    this->materialBuffer.update(scene.materials.data(), scene.materials.size() * sizeof(Material));
}

void Renderer::updateSceneMeshInstances(const Scene& scene)
{
    this->meshInstanceBuffer.update(scene.meshInstances.data(), scene.meshInstances.size() * sizeof(MeshInstance));
}

void Renderer::initData()
{
    // Textures
    this->accumulationTexture.init(GL_RGBA32F, GL_NEAREST);
    this->albedoTexture.init(GL_RGBA32F, GL_NEAREST);
    this->normalTexture.init(GL_RGBA32F, GL_NEAREST);
    this->depthTexture.init(GL_R32F, GL_NEAREST);
    this->toneMapTexture.init(GL_RGBA32F, GL_NEAREST);
    this->environment.texture.init(GL_RGBA32F);
    this->textureArray.init(GL_RGBA32F);

    // Storage buffers
    this->vertexBuffer.init();
    this->triangleBuffer.init();
    this->meshBuffer.init();
    this->meshInstanceBuffer.init();
    this->materialBuffer.init();
    this->bvhBuffer.init();

    // Uniform buffers
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
    this->textureArray.bind(1);

    // Storage buffers
    this->vertexBuffer.bind(0);
    this->triangleBuffer.bind(1);
    this->meshBuffer.bind(2);
    this->meshInstanceBuffer.bind(3);
    this->materialBuffer.bind(4);
    this->bvhBuffer.bind(5);

    // Uniform buffers
    this->cameraBuffer.bind(0);
    this->environmentBuffer.bind(1);
    this->paramBuffer.bind(2);
}

void Renderer::updateUniform(glm::ivec2 rectPosition, glm::ivec2 rectSize, bool onlyToneMapping)
{
    this->cameraBuffer.update(&this->camera, sizeof(this->camera));

    struct
    {
        glm::vec4 rotation1;
        glm::vec4 rotation2;
        glm::vec4 rotation3;
        int transparent;
        float intensity;
        int padding1;
        int padding2;
    } environment
    {
        glm::vec4(this->environment.rotation[0], 0),
        glm::vec4(this->environment.rotation[1], 0),
        glm::vec4(this->environment.rotation[2], 0),
        this->environment.transparent,
        this->environment.intensity,
        0,
        0,
    };
    this->environmentBuffer.update(&environment, sizeof(environment));

    struct
    {
        glm::ivec2 rectPosition;
        glm::ivec2 rectSize;
        unsigned int sampleCount;
        unsigned int maxBounceCount;
        float minRenderDistance;
        float maxRenderDistance;
        float gamma;
        unsigned int onlyToneMapping;
        unsigned int toneMapMode;
    } params
    {
        rectPosition,
        rectSize,
        this->sampleCount,
        this->maxBounceCount,
        this->minRenderDistance,
        this->maxRenderDistance,
        this->gamma,
        onlyToneMapping,
        static_cast<unsigned int>(this->toneMapMode),
    };
    this->paramBuffer.update(&params, sizeof(params));
}
