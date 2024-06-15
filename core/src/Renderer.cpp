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

    this->accumulatorShader.init(Renderer::vertexShaderSrc, Renderer::accumulatorShaderSrc);
    this->toneMapperShader.init(Renderer::vertexShaderSrc, Renderer::toneMapperShaderSrc);
    this->albedoShader.init(Renderer::vertexShaderSrc, Renderer::albedoShaderSrc);
    this->normalShader.init(Renderer::vertexShaderSrc, Renderer::normalShaderSrc);

    this->initData();

    this->resize(size);
}

void Renderer::resize(glm::uvec2 size)
{
    this->accumulator.colorTexture.update(Image::loadFromMemory(size, std::vector<float>()));
    this->output.colorTexture.update(Image::loadFromMemory(size, std::vector<float>()));
    this->albedo.colorTexture.update(Image::loadFromMemory(size, std::vector<float>()));
    this->normal.colorTexture.update(Image::loadFromMemory(size, std::vector<float>()));
    this->clear();
}

void Renderer::shutdown()
{
    this->quad.shutdown();

    this->accumulator.shutdown();
    this->output.shutdown();
    this->albedo.shutdown();
    this->normal.shutdown();

    this->environment.texture.shutdown();
    this->textureArray.shutdown();

    this->vertexBuffer.shutdown();
    this->triangleBuffer.shutdown();
    this->meshBuffer.shutdown();
    this->materialBuffer.shutdown();
    this->bvhBuffer.shutdown();

    this->accumulatorShader.shutdown();
    this->toneMapperShader.shutdown();
    this->albedoShader.shutdown();
    this->normalShader.shutdown();
}

void Renderer::render(unsigned int count)
{
    this->renderRect(count, glm::uvec2(0, 0), this->output.colorTexture.size);
}

void Renderer::renderRect(unsigned int count, glm::uvec2 position, glm::uvec2 size)
{
    this->accumulate(count, position, size);
    this->toneMap(position, size);
}

void Renderer::accumulate(unsigned int count, glm::uvec2 position, glm::uvec2 size)
{
    this->accumulatorShader.use();
    this->accumulatorShader.updateParam("MaxBounceCount", this->maxBounceCount);
    this->accumulatorShader.updateParam("MinRenderDistance", this->minRenderDistance);
    this->accumulatorShader.updateParam("MaxRenderDistance", this->maxRenderDistance);
    this->accumulatorShader.updateParam("Camera.Position", this->camera.position);
    this->accumulatorShader.updateParam("Camera.Forward", this->camera.forward);
    this->accumulatorShader.updateParam("Camera.Up", this->camera.up);
    this->accumulatorShader.updateParam("Camera.FOV", this->camera.fov);
    this->accumulatorShader.updateParam("Camera.FocalDistance", this->camera.focalDistance);
    this->accumulatorShader.updateParam("Camera.Aperture", this->camera.aperture);
    this->accumulatorShader.updateParam("Camera.Blur", this->camera.blur);
    this->accumulatorShader.updateParam("Environment.Transparent", this->environment.transparent);
    this->accumulatorShader.updateParam("Environment.Intensity", this->environment.intensity);
    this->accumulatorShader.updateParam("Environment.Rotation", this->environment.rotation);

    for (unsigned int i = 0; i < count; i++)
    {
        this->accumulatorShader.updateParam("FrameCount", this->frameCount);
        this->accumulator.useRect(position, size);
        this->quad.draw();
        this->frameCount++;
        FrameBuffer::stopUse();
    }

    Shader::stopUse();
}

void Renderer::toneMap(glm::uvec2 position, glm::uvec2 size)
{
    this->toneMapperShader.use();
    this->toneMapperShader.updateParam("FrameCount", this->frameCount);
    this->toneMapperShader.updateParam("Gamma", this->gamma);

    this->output.useRect(position, size);
    this->quad.draw();

    Shader::stopUse();
    FrameBuffer::stopUse();
}

#ifdef TX_DENOISE
void Renderer::denoise()
{
    // Create device
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    glm::uvec2 size = this->accumulator.colorTexture.size;

    // Create color buffer
    Image colorImage = this->accumulator.colorTexture.upload();
    oidn::BufferRef colorBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    colorBuf.writeAsync(0, colorImage.pixels.size() * sizeof(float), colorImage.pixels.data());

    // Create albedo buffer
    this->updateAlbedo();
    Image albedoImage = this->albedo.colorTexture.upload();
    oidn::BufferRef albedoBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    albedoBuf.writeAsync(0, albedoImage.pixels.size() * sizeof(float), albedoImage.pixels.data());

    // Create normal buffer
    this->updateNormal();
    Image normalImage = this->normal.colorTexture.upload();
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

    // Update accumulator
    float* data = (float*)colorBuf.getData();
    std::vector<float> pixels(data, data + colorImage.pixels.size());
    this->accumulator.colorTexture.update(Image::loadFromMemory(size, pixels));

    // Update output
    this->toneMapperShader.use();
    this->output.use();
    this->quad.draw();
    Shader::stopUse();
    FrameBuffer::stopUse();

    // Release buffers
    colorBuf.release();
    albedoBuf.release();
    normalBuf.release();
}
#endif

void Renderer::clear()
{
    this->accumulator.clear();
    this->frameCount = 0;
}

GLuint Renderer::getTextureHandler() const
{
    return this->output.colorTexture.getHandler();
}

Image Renderer::getImage() const
{
    return this->output.colorTexture.upload();
}

glm::uvec2 Renderer::getSize() const
{
    return this->output.colorTexture.size;
}

unsigned int Renderer::getFrameCount() const
{
    return this->frameCount;
}

void Renderer::loadScene(const Scene& scene, glm::uvec2 texturesSize)
{
    this->textureArray.update(texturesSize, scene.textures);
    this->bvhBuffer.update(scene.bvh);
    this->vertexBuffer.update(scene.vertices);
    this->triangleBuffer.update(scene.triangles);

    this->updateSceneMeshes(scene);
    this->updateSceneMaterials(scene);
}

void Renderer::updateSceneMaterials(const Scene& scene)
{
    this->materialBuffer.update(scene.materials);
}

void Renderer::updateSceneMeshes(const Scene& scene)
{
    this->meshBuffer.update(scene.meshes);
}

void Renderer::initData()
{
    this->quad.init();

    // Textures
    this->environment.texture.init();
    this->textureArray.init();

    // Frame buffers
    this->accumulator.init();
    this->output.init();
    this->albedo.init();
    this->normal.init();

    // Buffers
    this->vertexBuffer.init(GL_RGBA32F);
    this->triangleBuffer.init(GL_RGB32I);
    this->meshBuffer.init(GL_RGBA32F);
    this->materialBuffer.init(GL_RGBA32F);
    this->bvhBuffer.init(GL_RGB32F);

    // Bind textures
    this->accumulator.colorTexture.bind(0);
    this->environment.texture.bind(1);
    this->textureArray.bind(2);
    this->vertexBuffer.bind(3);
    this->triangleBuffer.bind(4);
    this->meshBuffer.bind(5);
    this->materialBuffer.bind(6);
    this->bvhBuffer.bind(7);
}

void Renderer::updateAlbedo()
{
    this->albedoShader.use();
    this->albedoShader.updateParam("MaxBounceCount", this->maxBounceCount);
    this->albedoShader.updateParam("MinRenderDistance", this->minRenderDistance);
    this->albedoShader.updateParam("MaxRenderDistance", this->maxRenderDistance);
    this->albedoShader.updateParam("Camera.Position", this->camera.position);
    this->albedoShader.updateParam("Camera.Forward", this->camera.forward);
    this->albedoShader.updateParam("Camera.Up", this->camera.up);
    this->albedoShader.updateParam("Camera.FOV", this->camera.fov);
    this->albedoShader.updateParam("Camera.FocalDistance", this->camera.focalDistance);
    this->albedoShader.updateParam("Camera.Aperture", this->camera.aperture);
    this->albedoShader.updateParam("Camera.Blur", this->camera.blur);
    this->albedoShader.updateParam("Environment.Transparent", this->environment.transparent);
    this->albedoShader.updateParam("Environment.Intensity", this->environment.intensity);
    this->albedoShader.updateParam("Environment.Rotation", this->environment.rotation);
    this->albedoShader.updateParam("FrameCount", this->frameCount);

    this->albedo.use();
    this->quad.draw();
    FrameBuffer::stopUse();
    Shader::stopUse();
}

void Renderer::updateNormal()
{
    this->normalShader.use();
    this->normalShader.updateParam("MaxBounceCount", this->maxBounceCount);
    this->normalShader.updateParam("MinRenderDistance", this->minRenderDistance);
    this->normalShader.updateParam("MaxRenderDistance", this->maxRenderDistance);
    this->normalShader.updateParam("Camera.Position", this->camera.position);
    this->normalShader.updateParam("Camera.Forward", this->camera.forward);
    this->normalShader.updateParam("Camera.Up", this->camera.up);
    this->normalShader.updateParam("Camera.FOV", this->camera.fov);
    this->normalShader.updateParam("Camera.FocalDistance", this->camera.focalDistance);
    this->normalShader.updateParam("Camera.Aperture", this->camera.aperture);
    this->normalShader.updateParam("Camera.Blur", this->camera.blur);
    this->normalShader.updateParam("Environment.Transparent", this->environment.transparent);
    this->normalShader.updateParam("Environment.Intensity", this->environment.intensity);
    this->normalShader.updateParam("Environment.Rotation", this->environment.rotation);
    this->normalShader.updateParam("FrameCount", this->frameCount);

    this->normal.use();
    this->quad.draw();
    FrameBuffer::stopUse();
    Shader::stopUse();
}
