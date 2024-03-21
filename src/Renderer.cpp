#include "TracerX/Renderer.h"

#include <iostream>
#include <OpenImageDenoise/oidn.hpp>


void Renderer::init()
{
    // Init GLEW
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        throw std::runtime_error((const char*)glewGetErrorString(status));
    }

    this->pathTracer.init("../shaders/vertex/main.glsl", "../shaders/fragment/pathTracer/main.glsl");
    this->toneMapper.init("../shaders/vertex/main.glsl", "../shaders/fragment/toneMapper/main.glsl");

    this->initData();
}

void Renderer::resize(glm::ivec2 size)
{
    this->camera.aspectRatio = (float)size.x / size.y;
    this->accumulator.colorTexture.update(size, 0);
    this->output.colorTexture.update(size, 0);
    this->resetAccumulator();
}

void Renderer::shutdown()
{
    this->accumulator.shutdown();
    this->output.shutdown();

    this->environmentTexture.shutdown();
    this->textureArray.shutdown();

    this->vertexBuffer.shutdown();
    this->triangleBuffer.shutdown();
    this->meshBuffer.shutdown();
    this->materialBuffer.shutdown();
    this->bvhBuffer.shutdown();

    this->pathTracer.shutdown();
    this->toneMapper.shutdown();
}

void Renderer::render()
{
    this->updateShaders();

    // Update accumulator
    this->pathTracer.use();
    this->accumulator.draw();

    // Update output
    this->toneMapper.use();
    this->output.draw();

    Shader::stopUse();

    this->frameCount++;
}

void Renderer::denoise()
{
    // Create device
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();

    // Create color buffer
    glm::ivec2 size = this->accumulator.colorTexture.size;
    oidn::BufferRef colorBuf = device.newBuffer(size.x * size.y * 3 * sizeof(float));
    this->accumulator.colorTexture.upload((float*)colorBuf.getData());

    // Create filter
    oidn::FilterRef filter = device.newFilter("RT");
    filter.setImage("color", colorBuf, oidn::Format::Float3, size.x, size.y);
    filter.setImage("output", colorBuf, oidn::Format::Float3, size.x, size.y);
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
    this->accumulator.colorTexture.update(size, (float*)colorBuf.getData());
    colorBuf.release();

    // Update output
    this->toneMapper.use();
    this->output.draw();
}

void Renderer::resetAccumulator()
{
    this->accumulator.clear();
    this->frameCount = 1;
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

void Renderer::resetEnvironment(const Image& image)
{
    this->environmentTexture.update(image);
}

void Renderer::resetScene(Scene& scene)
{
    std::vector<glm::vec3> bvh = scene.createBVH();

    this->textureArray.update(glm::ivec2(2048, 2048), scene.textures);
    this->vertexBuffer.update(scene.vertices);
    this->triangleBuffer.update(scene.triangles);
    this->meshBuffer.update(scene.meshes);
    this->materialBuffer.update(scene.materials);
    this->bvhBuffer.update(bvh);

    this->resetAccumulator();
    this->resetEnvironment(scene.environment);
}

void Renderer::initData()
{
    // Textures
    this->environmentTexture.init();
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
    this->environmentTexture.bind(1);
    this->textureArray.bind(2);
    this->vertexBuffer.bind(3);
    this->triangleBuffer.bind(4);
    this->meshBuffer.bind(5);
    this->materialBuffer.bind(6);
    this->bvhBuffer.bind(7);
}

void Renderer::updateShaders()
{
    // Path tracer
    this->pathTracer.use();
    this->pathTracer.updateParam("MaxBouceCount", this->maxBouceCount);
    this->pathTracer.updateParam("FrameCount", this->frameCount);
    this->pathTracer.updateParam("Camera.Position", this->camera.position);
    this->pathTracer.updateParam("Camera.Forward", this->camera.forward);
    this->pathTracer.updateParam("Camera.Up", this->camera.up);
    this->pathTracer.updateParam("Camera.FOV", this->camera.fov);
    this->pathTracer.updateParam("Camera.FocalDistance", this->camera.focalDistance);
    this->pathTracer.updateParam("Camera.Aperture", this->camera.aperture);
    this->pathTracer.updateParam("Camera.AspectRatio", this->camera.aspectRatio);
    this->pathTracer.updateParam("EnvironmentIntensity", this->environmentIntensity);

    // Tone mapper
    this->toneMapper.use();
    this->toneMapper.updateParam("FrameCount", this->frameCount);
    this->toneMapper.updateParam("Gamma", this->gamma);

    Shader::stopUse();
}
