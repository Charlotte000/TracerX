#include "Renderer.h"

#include <fstream>
#include <SFML/Graphics.hpp>
#include <OBJLoader.h>

using namespace std;

Renderer::Renderer(Vector2i size, Camera camera, int sampleCount, int maxBounceCount)
    : camera(camera), size(size), sampleCount(sampleCount), maxBounceCount(maxBounceCount)
{
    this->buffer1.create(this->size.x, this->size.y);
    this->buffer2.create(this->size.x, this->size.y);
}

void Renderer::loadScene()
{
    if (this->materials.size() == 0)
    {
        this->materials.push_back(Material(Vector3f(.8f, .8f, .8f), .7f));
    }

    if (this->aabbs.size() == 0 && this->spheres.size() == 0 && this->meshes.size() == 0)
    {
        this->aabbs.push_back(AABB(Vector3f(0, 0, 0), Vector3f(1, 1, 1), 0));
    }

    this->loadShader();
    this->shader.setUniform("WindowSize", (Vector2f)this->size);
    this->shader.setUniform("SampleCount", this->sampleCount);
    this->shader.setUniform("MaxBouceCount", this->maxBounceCount);
    this->shader.setUniform("FocusStrength", this->camera.focusStrength);
    this->shader.setUniform("FocalLength", this->camera.focalLength);

    for (int i = 0; i < this->materials.size(); i++)
    {
        this->materials[i].set(this->shader, "Materials[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->spheres.size(); i++)
    {
        this->spheres[i].set(this->shader, "Spheres[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->aabbs.size(); i++)
    {
        this->aabbs[i].set(this->shader, "AABBs[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->vertices.size(); i++)
    {
        this->vertices[i].set(this->shader, "Vertices[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->meshes.size(); i++)
    {
        this->meshes[i].set(this->shader, "Meshes[" + to_string(i) + ']');
    }

    for (int i = 0; i < this->indices.size(); i++)
    {
        this->shader.setUniform("Indices[" + to_string(i) + ']', this->indices[i]);
    }

    for (int i = 0; i < this->albedoMaps.size(); i++)
    {
        this->shader.setUniform("AlbedoMaps[" + to_string(i) + ']', this->albedoMaps[i]);
    }


    this->environment.set(this->shader, "Environment");
}

void Renderer::run(int iterationCount, const string imagePath)
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

        RenderTexture* newBuffer = (this->frameCount & 1) == 1 ? &this->buffer1 : &this->buffer2;
        RenderTexture* oldBuffer = (this->frameCount & 1) == 1 ? &this->buffer2 : &this->buffer1;
        Sprite oldSprite(oldBuffer->getTexture());
        Sprite newSprite(newBuffer->getTexture());
        oldSprite.setTextureRect(IntRect(x, y, subWidth, subHeight));
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
    Image image1 = this->buffer1.getTexture().copyToImage();
    Image image2 = this->buffer2.getTexture().copyToImage();

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
    vector<Material>::iterator it = find(this->materials.begin(), this->materials.end(), material);
    if (it != this->materials.end())
    {
        return (int)(it - this->materials.begin());
    }

    this->materials.push_back(material);
    return (int)(this->materials.size() - 1);
}

void Renderer::addFile(const string filePath, Vector3f offset, Vector3f scale, Vector3f rotation)
{
    objl::Loader loader;
    loader.LoadFile(filePath);

    for (const objl::Mesh& mesh : loader.LoadedMeshes)
    {
        for (const objl::Vertex& vertex : mesh.Vertices)
        {
            this->vertices.push_back(Vertex3(
                Vector3f(vertex.Position.X, vertex.Position.Y, vertex.Position.Z),
                Vector3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z),
                Vector2f(vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y)));
        }

        Material material;
        material.albedoColor = Vector3f(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
        material.roughness = sqrtf(2.0f / (mesh.MeshMaterial.Ns + 2));
        material.metalnessColor = Vector3f(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
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
    this->add(AABB(Vector3f(0, .998f, 0), Vector3f(.5f, .001f, .5f)), lightSource);
}

void Renderer::addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward)
{
    this->add(AABB(Vector3f(0, 1.5f, -1), Vector3f(2, 1, 4)), up);
    this->add(AABB(Vector3f(0, -1.5f, -1), Vector3f(2, 1, 4)), down);

    this->add(AABB(Vector3f(1.5f, 0, -1), Vector3f(1, 2, 4)), left);
    this->add(AABB(Vector3f(-1.5f, 0, -1), Vector3f(1, 2, 4)), right);

    this->add(AABB(Vector3f(0, 0, 1.5f), Vector3f(2, 2, 1)), forward);
    this->add(AABB(Vector3f(0, 0, -3.5f), Vector3f(2, 2, 1)), backward);
}

void Renderer::loadShader()
{
    ifstream myFile(this->path);
    string content;
    content.assign(istreambuf_iterator<char>(myFile), istreambuf_iterator<char>());
    myFile.close();
    if (!this->shader.loadFromMemory(
        (!this->spheres.empty() ? "#define SphereCount " + to_string(this->spheres.size()) + "\n" : "") +
        (!this->aabbs.empty() ? "#define AABBCount " + to_string(this->aabbs.size()) + "\n" : "") +
        (!this->materials.empty() ? "#define MaterialCount " + to_string(this->materials.size()) + "\n" : "") +
        (!this->vertices.empty() ? "#define VertexCount " + to_string(this->vertices.size()) + "\n" : "") +
        (!this->meshes.empty() ? "#define MeshCount " + to_string(this->meshes.size()) + "\n" : "") +
        (!this->indices.empty() ? "#define IndexCount " + to_string(this->indices.size()) + "\n" : "") +
        (!this->albedoMaps.empty() ? "#define AlbedoMapCount " + to_string(this->albedoMaps.size()) + "\n" : "") +
        content,
        Shader::Fragment))
    {
        cout << "Error" << endl;
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