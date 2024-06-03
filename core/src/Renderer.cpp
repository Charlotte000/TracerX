#include "TracerX/Renderer.h"

#include <iostream>
#ifdef TX_DENOISE
#include <OpenImageDenoise/oidn.hpp>
#endif

using namespace TracerX;
using namespace TracerX::core;

void Renderer::init()
{
    // Init GLEW
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        throw std::runtime_error((const char*)glewGetErrorString(status));
    }

    this->pathTracer.init(Renderer::vertexShaderSrc, Renderer::pathTracerShaderSrc);
    this->toneMapper.init(Renderer::vertexShaderSrc, Renderer::toneMapperShaderSrc);

    this->initData();
}

void Renderer::resize(glm::uvec2 size)
{
    this->accumulator.colorTexture.update(Image::loadFromMemory(size, std::vector<float>()));
    this->output.colorTexture.update(Image::loadFromMemory(size, std::vector<float>()));
    this->resetAccumulator();
}

void Renderer::shutdown()
{
    this->quad.shutdown();

    this->accumulator.shutdown();
    this->output.shutdown();

    this->environment.texture.shutdown();
    this->textureArray.shutdown();

    this->vertexBuffer.shutdown();
    this->triangleBuffer.shutdown();
    this->meshBuffer.shutdown();
    this->materialBuffer.shutdown();
    this->bvhBuffer.shutdown();

    this->pathTracer.shutdown();
    this->toneMapper.shutdown();
}

void Renderer::render(unsigned int count)
{
    this->renderRect(count, glm::uvec2(0, 0), this->output.colorTexture.size);
}

void Renderer::renderRect(unsigned int count, glm::uvec2 position, glm::uvec2 size)
{
    // Update accumulator
    this->pathTracer.use();
    this->pathTracer.updateParam("MaxBouceCount", this->maxBouceCount);
    this->pathTracer.updateParam("MinRenderDistance", this->minRenderDistance);
    this->pathTracer.updateParam("MaxRenderDistance", this->maxRenderDistance);
    this->pathTracer.updateParam("Camera.Position", this->camera.position);
    this->pathTracer.updateParam("Camera.Forward", this->camera.forward);
    this->pathTracer.updateParam("Camera.Up", this->camera.up);
    this->pathTracer.updateParam("Camera.FOV", this->camera.fov);
    this->pathTracer.updateParam("Camera.FocalDistance", this->camera.focalDistance);
    this->pathTracer.updateParam("Camera.Aperture", this->camera.aperture);
    this->pathTracer.updateParam("Camera.Blur", this->camera.blur);
    this->pathTracer.updateParam("Environment.Transparent", this->environment.transparent);
    this->pathTracer.updateParam("Environment.Intensity", this->environment.intensity);
    this->pathTracer.updateParam("Environment.Rotation", this->environment.rotation);

    for (unsigned int i = 0; i < count; i++)
    {
        this->pathTracer.updateParam("FrameCount", this->frameCount);

        this->accumulator.useRect(position, size);
        this->quad.draw();
        this->frameCount++;
        FrameBuffer::stopUse();
    }

    // Update output
    this->toneMapper.use();
    this->toneMapper.updateParam("FrameCount", this->frameCount);
    this->toneMapper.updateParam("Gamma", this->gamma);

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

    // Create color buffer
    Image colorImage = this->accumulator.colorTexture.upload();
    glm::uvec2 size = this->accumulator.colorTexture.size;
    oidn::BufferRef colorBuf = device.newBuffer(size.x * size.y * 4 * sizeof(float));
    colorBuf.write(0, colorImage.pixels.size() * sizeof(float), colorImage.pixels.data());

    // Create filter
    oidn::FilterRef filter = device.newFilter("RT");
    filter.setImage("color", colorBuf, oidn::Format::Float3, size.x, size.y, 0, 4 * sizeof(float));
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
    colorBuf.release();

    // Update output
    this->toneMapper.use();
    this->output.use();
    this->quad.draw();
    Shader::stopUse();
    FrameBuffer::stopUse();
}
#endif

void Renderer::resetAccumulator()
{
    this->accumulator.clear();
    this->frameCount = 0;
}

void Renderer::resetMeshes(const std::vector<Mesh>& meshes)
{
    this->meshBuffer.update(meshes);
}

void Renderer::resetBVH(const std::vector<glm::vec3>& bvh, const std::vector<Triangle> triangles)
{
    this->bvhBuffer.update(bvh);
    this->triangleBuffer.update(triangles);
}

void Renderer::resetMaterials(const std::vector<Material>& materials)
{
    this->materialBuffer.update(materials);
}

void Renderer::resetScene(Scene& scene)
{
    std::vector<glm::vec3> bvh = scene.createBVH();

    this->textureArray.update(glm::uvec2(2048, 2048), scene.textures);
    this->vertexBuffer.update(scene.vertices);
    this->triangleBuffer.update(scene.triangles);
    this->meshBuffer.update(scene.meshes);
    this->materialBuffer.update(scene.materials);
    this->bvhBuffer.update(bvh);

    this->resetAccumulator();
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

    // Buffers
    this->vertexBuffer.init(GL_RGB32F);
    this->triangleBuffer.init(GL_RGBA32I);
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
