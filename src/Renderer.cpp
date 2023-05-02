#include <fstream>
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include <OBJ-Loader/OBJLoader.h>
#include <GL/glew.h>
#include <TracerX/Renderer.h>

namespace TracerX
{

Renderer::Renderer(sf::Vector2i size, Camera camera, int sampleCount, int maxBounceCount)
    : camera(camera), size(size), sampleCount(sampleCount), maxBounceCount(maxBounceCount)
{
    this->buffer1.create(this->size.x, this->size.y);
    this->buffer2.create(this->size.x, this->size.y);
}

void Renderer::loadScene()
{
    if (this->materials.size() == 0)
    {
        this->materials.push_back(Material(sf::Vector3f(.8f, .8f, .8f), .7f));
    }

    if (this->aabbs.size() == 0 && this->spheres.size() == 0 && this->meshes.size() == 0)
    {
        this->aabbs.push_back(AABB(sf::Vector3f(0, 0, 0), sf::Vector3f(1, 1, 1), 0));
    }

    this->loadShader();

    
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("GLEW initialization failed");
    }
    
    glGenBuffers(1, &this->materialBuffer);
    glGenBuffers(1, &this->vertexBuffer);
    glGenBuffers(1, &this->sphereBuffer);
    glGenBuffers(1, &this->indexBuffer);
    glGenBuffers(1, &this->meshBuffer);
    glGenBuffers(1, &this->aabbBuffer);
    
    this->setMaterials();
    this->setVertices();
    this->setSpheres();
    this->setIndices();
    this->setMeshes();
    this->setAABBs();
    
    this->shader.setUniform("WindowSize", (sf::Vector2f)this->size);
    this->shader.setUniform("SampleCount", this->sampleCount);
    this->shader.setUniform("MaxBouceCount", this->maxBounceCount);
    this->shader.setUniform("FocusStrength", this->camera.focusStrength);
    this->shader.setUniform("FocalLength", this->camera.focalLength);

    for (int i = 0; i < this->albedoMaps.size(); i++)
    {
        this->shader.setUniform("AlbedoMaps[" + std::to_string(i) + ']', this->albedoMaps[i]);
    }

    this->environment.set(this->shader, "Environment");
}

void Renderer::run(int iterationCount, const std::string imagePath)
{
    this->shader.setUniform("CameraPosition", this->camera.position);
    this->shader.setUniform("CameraForward", this->camera.forward);
    this->shader.setUniform("CameraUp", this->camera.up);

    int subWidth = this->size.x / this->subDivisor.x;
    int subHeight = this->size.y / this->subDivisor.y;

    while (this->frameCount <= iterationCount)
    {
        this->shader.setUniform("FrameCount", this->frameCount);

        int y = this->subStage / this->subDivisor.x;
        int x = this->subStage - y * this->subDivisor.x;
        x *= subWidth;
        y *= subHeight;

        sf::RenderTexture* newBuffer = (this->frameCount & 1) == 1 ? &this->buffer1 : &this->buffer2;
        sf::RenderTexture* oldBuffer = (this->frameCount & 1) == 1 ? &this->buffer2 : &this->buffer1;
        sf::Sprite oldSprite(oldBuffer->getTexture());
        sf::Sprite newSprite(newBuffer->getTexture());
        oldSprite.setTextureRect(sf::IntRect(x, y, subWidth, subHeight));
        oldSprite.setPosition((float)x, (float)y);
        newBuffer->draw(oldSprite, &this->shader);
        newBuffer->display();

        this->subStage++;
        if (this->subStage >= this->subDivisor.x * this->subDivisor.y)
        {
            this->subStage = 0;
            this->frameCount++;
        }
    }

    this->buffer1.getTexture().copyToImage().saveToFile(imagePath);
}

int Renderer::getPixelDifference()
{
    sf::Image image1 = this->buffer1.getTexture().copyToImage();
    sf::Image image2 = this->buffer2.getTexture().copyToImage();

    int result = 0;
    for (int x = 0; x < this->size.x; x++)
    {
        for (int y = 0; y < this->size.y; y++)
        {
            if (image1.getPixel(x, y) != image2.getPixel(x, y))
            {
                result++;
            }
        }
    }

    return result;
}

void Renderer::add(Sphere sphere, const Material material)
{
    int materialId = this->add(material);
    sphere.materialId = materialId;
    this->spheres.push_back(sphere);
}

void Renderer::add(Sphere sphere)
{
    this->spheres.push_back(sphere);
}

void Renderer::add(AABB aabb, const Material material)
{
    int materialId = this->add(material);
    aabb.materialId = materialId;
    this->aabbs.push_back(aabb);
}

void Renderer::add(AABB aabb)
{
    this->aabbs.push_back(aabb);
}

int Renderer::add(const Material& material)
{
    std::vector<Material>::iterator it = find(this->materials.begin(), this->materials.end(), material);
    if (it != this->materials.end())
    {
        return (int)(it - this->materials.begin());
    }

    this->materials.push_back(material);
    return (int)(this->materials.size() - 1);
}

void Renderer::addFile(const std::string filePath, sf::Vector3f offset, sf::Vector3f scale, sf::Vector3f rotation)
{
    objl::Loader loader;
    if (!loader.LoadFile(filePath))
    {
        throw std::runtime_error("Load file failed");
    }

    for (const objl::Mesh& mesh : loader.LoadedMeshes)
    {
        for (const objl::Vertex& vertex : mesh.Vertices)
        {
            this->vertices.push_back(Vertex3(
                sf::Vector3f(vertex.Position.X, vertex.Position.Y, vertex.Position.Z),
                sf::Vector3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z),
                sf::Vector2f(vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y)));
        }

        Material material;
        material.albedoColor = sf::Vector3f(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
        material.roughness = sqrtf(2.0f / (mesh.MeshMaterial.Ns + 2));
        material.metalnessColor = sf::Vector3f(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
        material.metalness = mesh.MeshMaterial.Ka.X;
        int materialId = this->add(material);

        Mesh myMesh((int)this->indices.size(), (int)mesh.Indices.size(), materialId);
        this->indices.insert(this->indices.end(), mesh.Indices.begin(), mesh.Indices.end());

        myMesh.scale(scale, this->indices, this->vertices);
        myMesh.rotate(rotation, this->indices, this->vertices);
        myMesh.offset(offset, this->indices, this->vertices);
        this->meshes.push_back(myMesh);
    }
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource)
{
    this->addCornellBox(up, down, left, right, forward, backward);
    this->add(AABB(sf::Vector3f(0, .998f, 0), sf::Vector3f(.5f, .001f, .5f)), lightSource);
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward)
{
    this->add(AABB(sf::Vector3f(0, 1.5f, -1), sf::Vector3f(2, 1, 4)), up);
    this->add(AABB(sf::Vector3f(0, -1.5f, -1), sf::Vector3f(2, 1, 4)), down);

    this->add(AABB(sf::Vector3f(1.5f, 0, -1), sf::Vector3f(1, 2, 4)), left);
    this->add(AABB(sf::Vector3f(-1.5f, 0, -1), sf::Vector3f(1, 2, 4)), right);

    this->add(AABB(sf::Vector3f(0, 0, 1.5f), sf::Vector3f(2, 2, 1)), forward);
    this->add(AABB(sf::Vector3f(0, 0, -3.5f), sf::Vector3f(2, 2, 1)), backward);
}

void Renderer::loadShader()
{
    std::ifstream shaderFile(this->path);
    std::string content;
    content.assign(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
    shaderFile.close();
    if (!this->shader.loadFromMemory(
        "#version 430 core\n" +
        (!this->albedoMaps.empty() ? "#define AlbedoMapCount " + std::to_string(this->albedoMaps.size()) + "\n" : "") +
        content,
        sf::Shader::Fragment))
    {
        throw std::runtime_error("Failed to load shader");
    }
}

void Renderer::clear()
{
    this->buffer1.clear();
    this->buffer1.display();
    this->buffer2.clear();
    this->buffer2.display();
    this->frameCount = 1;
}

void Renderer::setMaterials()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->materialBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->materials.size() * sizeof(Material), this->materials.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->materialBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::setVertices()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->vertexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->vertices.size() * sizeof(Vertex3), this->vertices.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->vertexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::setSpheres()
{
    this->shader.setUniform("SphereCount", (int)this->spheres.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->sphereBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->spheres.size() * sizeof(Sphere), this->spheres.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->sphereBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::setIndices()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->indexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->indices.size() * sizeof(int), this->indices.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, this->indexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::setMeshes()
{
    this->shader.setUniform("MeshCount", (int)this->meshes.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->meshBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->meshes.size() * sizeof(Mesh), this->meshes.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, this->meshBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::setAABBs()
{
    this->shader.setUniform("AABBCount", (int)this->aabbs.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->aabbBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->aabbs.size() * sizeof(AABB), this->aabbs.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, this->aabbBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

}