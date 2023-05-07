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

void Renderer::loadShader()
{
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("GLEW initialization failed");
    }

    std::ifstream shaderFile(this->path);
    std::string content;
    content.assign(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
    shaderFile.close();
    if (!this->shader.loadFromMemory(
        "#version 430 core\n" +
        (!this->textures.empty() ? "#define TextureCount " + std::to_string(this->textures.size()) + "\n" : "") +
        content,
        sf::Shader::Fragment))
    {
        throw std::runtime_error("Failed to load shader");
    }

    glGenBuffers(1, &this->materialBuffer);
    glGenBuffers(1, &this->vertexBuffer);
    glGenBuffers(1, &this->sphereBuffer);
    glGenBuffers(1, &this->indexBuffer);
    glGenBuffers(1, &this->meshBuffer);
    glGenBuffers(1, &this->boxBuffer);
    
    this->updateMaterials();
    this->updateVertices();
    this->updateSpheres();
    this->updateIndices();
    this->updateMeshes();
    this->updateBoxes();
    
    this->shader.setUniform("WindowSize", (sf::Vector2f)this->size);
    this->shader.setUniform("SampleCount", this->sampleCount);
    this->shader.setUniform("MaxBouceCount", this->maxBounceCount);
    this->shader.setUniform("FocusStrength", this->camera.focusStrength);
    this->shader.setUniform("FocalLength", this->camera.focalLength);

    for (int i = 0; i < this->textures.size(); i++)
    {
        this->shader.setUniform("Textures[" + std::to_string(i) + ']', this->textures[i]);
    }

    this->environment.set(this->shader, "Environment");

    this->shader.setUniform("CameraPosition", this->camera.position);
    this->shader.setUniform("CameraForward", this->camera.forward);
    this->shader.setUniform("CameraUp", this->camera.up);
}

void Renderer::run(int iterationCount, const std::string imagePath)
{
    this->loadShader();

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

#pragma region Add
void Renderer::add(Sphere sphere, const Material& material)
{
    int materialId = this->add(material);
    sphere.materialId = materialId;
    this->spheres.push_back(sphere);
}

void Renderer::add(Sphere sphere)
{
    this->spheres.push_back(sphere);
}

void Renderer::add(Box box, const Material& material)
{
    int materialId = this->add(material);
    box.materialId = materialId;
    this->boxes.push_back(box);
}

void Renderer::add(Box box)
{
    this->boxes.push_back(box);
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

int Renderer::addTexture(const std::string filePath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(filePath))
    {
        throw std::runtime_error("Load file failed");
    }

    this->textures.push_back(texture);
    return (int)(this->textures.size() - 1);
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
        Material material;
        material.albedoColor = sf::Vector3f(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
        material.roughness = sqrtf(2.0f / (mesh.MeshMaterial.Ns + 2));
        material.metalnessColor = sf::Vector3f(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
        material.metalness = mesh.MeshMaterial.Ka.X;
        int materialId = this->add(material);

        Mesh myMesh((int)this->indices.size(), (int)this->indices.size() + (int)mesh.Indices.size(), materialId);

        int verticesOffset = this->vertices.size();
        for (auto index : mesh.Indices)
        {
            this->indices.push_back(index + verticesOffset);
        }

        for (const objl::Vertex& vertex : mesh.Vertices)
        {
            this->vertices.push_back(Vertex3(
                sf::Vector3f(vertex.Position.X, vertex.Position.Y, vertex.Position.Z),
                sf::Vector3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z),
                sf::Vector2f(vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y)));
        }
        
        myMesh.scale(scale, this->indices, this->vertices);
        myMesh.rotate(rotation, this->indices, this->vertices);
        myMesh.offset(offset, this->indices, this->vertices);
        this->meshes.push_back(myMesh);
    }
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource)
{
    this->addCornellBox(up, down, left, right, forward, backward);
    this->add(Box(sf::Vector3f(0, .998f, 0), sf::Vector3f(.5f, .001f, .5f)), lightSource);
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward)
{
    this->add(Box(sf::Vector3f(0, 1.5f, -1), sf::Vector3f(2, 1, 4)), up);
    this->add(Box(sf::Vector3f(0, -1.5f, -1), sf::Vector3f(2, 1, 4)), down);

    this->add(Box(sf::Vector3f(1.5f, 0, -1), sf::Vector3f(1, 2, 4)), left);
    this->add(Box(sf::Vector3f(-1.5f, 0, -1), sf::Vector3f(1, 2, 4)), right);

    this->add(Box(sf::Vector3f(0, 0, 1.5f), sf::Vector3f(2, 2, 1)), forward);
    this->add(Box(sf::Vector3f(0, 0, -3.5f), sf::Vector3f(2, 2, 1)), backward);
}
#pragma endregion

void Renderer::clear()
{
    this->buffer1.clear();
    this->buffer1.display();
    this->buffer2.clear();
    this->buffer2.display();
    this->frameCount = 1;
}

#pragma region Updates
void Renderer::updateMaterials()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->materialBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->materials.size() * sizeof(Material), this->materials.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->materialBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::updateVertices()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->vertexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->vertices.size() * sizeof(Vertex3), this->vertices.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->vertexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::updateSpheres()
{
    this->shader.setUniform("SphereCount", (int)this->spheres.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->sphereBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->spheres.size() * sizeof(Sphere), this->spheres.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->sphereBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::updateIndices()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->indexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->indices.size() * sizeof(int), this->indices.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, this->indexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::updateMeshes()
{
    this->shader.setUniform("MeshCount", (int)this->meshes.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->meshBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->meshes.size() * sizeof(Mesh), this->meshes.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, this->meshBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::updateBoxes()
{
    this->shader.setUniform("BoxCount", (int)this->boxes.size());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->boxBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->boxes.size() * sizeof(Box), this->boxes.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, this->boxBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
#pragma endregion

}