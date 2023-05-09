#include <stdexcept>
#include <GL/glew.h>
#include <OBJ-Loader/OBJLoader.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <TracerX/RendererUI.h>
#include <TracerX/Renderer.h>

namespace TracerX
{

Renderer::Renderer(sf::Vector2i size, Camera camera, int sampleCount, int maxBounceCount)
    : camera(camera), size(size), sampleCount(sampleCount), maxBounceCount(maxBounceCount)
{
    this->window.create(sf::VideoMode(this->size.x, this->size.y), "Ray Tracing");
    this->windowBuffer.create(this->size.x, this->size.y);

    this->buffer1.create(this->size.x, this->size.y);
    this->buffer2.create(this->size.x, this->size.y);

    this->cursor.setFillColor(sf::Color::Transparent);
    this->cursor.setOutlineColor(sf::Color::Red);
    this->cursor.setOutlineThickness(1);
}

void Renderer::loadShader()
{
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << glewGetErrorString(err) << std::endl;
        throw std::runtime_error("GLEW initialization failed");
    }

    if (!this->shader.loadFromMemory(Renderer::ShaderCode, sf::Shader::Fragment))
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
    this->updateTextures();
    
    this->shader.setUniform("WindowSize", (sf::Vector2f)this->size);
    this->shader.setUniform("SampleCount", this->sampleCount);
    this->shader.setUniform("MaxBouceCount", this->maxBounceCount);
    this->shader.setUniform("FocusStrength", this->camera.focusStrength);
    this->shader.setUniform("FocalLength", this->camera.focalLength);

    this->environment.set(this->shader, "Environment");

    this->shader.setUniform("CameraPosition", this->camera.position);
    this->shader.setUniform("CameraForward", this->camera.forward);
    this->shader.setUniform("CameraUp", this->camera.up);
}

void Renderer::run()
{
    this->loadShader();

    sf::Mouse::setPosition(this->size / 2, this->window);

    if (!ImGui::SFML::Init(this->window))
    {
        throw std::runtime_error("ImGui initialization failed");
    }

    sf::Clock clock;
    while (this->window.isOpen())
    {
        sf::Event event;
        while (this->window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                this->window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && this->isCameraControl)
            {
                this->isCameraControl = false;
                this->window.setMouseCursorVisible(true);
            }
        }

        if (this->isCameraControl)
        {
            this->camera.move(this->window);
        }

        if (!this->isProgressive)
        {
            this->clear();
        }

        // Update shader parameters
        this->shader.setUniform("FrameCount", this->frameCount);
        this->shader.setUniform("CameraPosition", this->camera.position);
        this->shader.setUniform("CameraForward", this->camera.forward);
        this->shader.setUniform("CameraUp", this->camera.up);

        // Update UI
        ImGui::SFML::Update(this->window, clock.restart());

        // Calculate subframe coordinate
        int subWidth = this->size.x / this->subDivisor.x;
        int subHeight = this->size.y / this->subDivisor.y;
        int y = this->subStage / this->subDivisor.x;
        int x = this->subStage - y * this->subDivisor.x;
        x *= subWidth;
        y *= subHeight;

        // Get new and old draw buffer 
        sf::RenderTexture* newBuffer = (this->frameCount & 1) == 1 ? &this->buffer1 : &this->buffer2;
        sf::Sprite newSprite(newBuffer->getTexture());

        sf::RenderTexture* oldBuffer = (this->frameCount & 1) == 1 ? &this->buffer2 : &this->buffer1;
        sf::Sprite oldSprite(oldBuffer->getTexture());
        
        // Ray trace into new buffer
        oldSprite.setTextureRect(sf::IntRect(x, y, subWidth, subHeight));
        oldSprite.setPosition((float)x, (float)y);
        newBuffer->draw(oldSprite, &this->shader);
        newBuffer->display();
        
        // Draw to window buffer
        newSprite.setTextureRect(sf::IntRect(x, y, subWidth, subHeight));
        newSprite.setPosition((float)x, (float)y);
        this->windowBuffer.draw(newSprite);
        this->windowBuffer.display();

        // Draw window
        this->window.clear();
        this->window.draw(sf::Sprite(this->windowBuffer.getTexture()));

        if (this->showCursor)
        {
            this->cursor.setSize(sf::Vector2f((float)subWidth, (float)subHeight));
            this->cursor.setPosition(sf::Vector2f((float)x, (float)y));
            this->window.draw(this->cursor);
        }

        // Draw UI
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("Info"))
        {
            InfoUI(*this, this->buffer1);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Material"))
        {
            MaterialUI(*this);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Geometry"))
        {
            GeometryUI(*this);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Environment"))
        {
            EnvironmentUI(*this);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Texture"))
        {
            TextureUI(*this);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::Text("FPS: %i", (int)roundf(ImGui::GetIO().Framerate));

        ImGui::EndMainMenuBar();

        ImGui::SFML::Render(this->window);

        this->window.display();
        
        // Update subframe
        this->subStage++;
        if (this->subStage >= this->subDivisor.x * this->subDivisor.y)
        {
            this->subStage = 0;
            this->frameCount++;
        }
    }

    ImGui::SFML::Shutdown();
}

void Renderer::runHeadless(int iterationCount, const std::string imagePath)
{
    this->window.setVisible(false);

    this->loadShader();

    int subWidth = this->size.x / this->subDivisor.x;
    int subHeight = this->size.y / this->subDivisor.y;
    int subCount = this->subDivisor.x * this->subDivisor.y;
    while (this->frameCount <= iterationCount)
    {
        // Update shader parameters
        this->shader.setUniform("FrameCount", this->frameCount);

        // Calculate subframe coordinate
        int y = this->subStage / this->subDivisor.x;
        int x = this->subStage - y * this->subDivisor.x;
        x *= subWidth;
        y *= subHeight;

        // Get new and old draw buffer 
        sf::RenderTexture* newBuffer = (this->frameCount & 1) == 1 ? &this->buffer1 : &this->buffer2;
        sf::Sprite newSprite(newBuffer->getTexture());

        sf::RenderTexture* oldBuffer = (this->frameCount & 1) == 1 ? &this->buffer2 : &this->buffer1;
        sf::Sprite oldSprite(oldBuffer->getTexture());
        
        // Ray trace into new buffer
        oldSprite.setTextureRect(sf::IntRect(x, y, subWidth, subHeight));
        oldSprite.setPosition((float)x, (float)y);
        newBuffer->draw(oldSprite, &this->shader);
        newBuffer->display();
        
        // Update subframe
        this->subStage++;
        if (this->subStage >= subCount)
        {
            this->subStage = 0;
            this->frameCount++;
        }

        // Print progress
        static const int barWidth = 50;
        float progress = (float)(this->subStage) / subCount;
        int progressWidth = progress * barWidth;
        std::cout << std::nounitbuf;
        std::cout << "\x1b[F[" << std::string(progressWidth, '#');
        std::cout << std::string(barWidth - progressWidth, ' ') << "] ";
        std::cout << (int)(progress * 100) << "%  " << std::endl;

        progress = (float)(this->frameCount - 1) / iterationCount;
        progressWidth = progress * barWidth;
        std::cout << '[' << std::string(progressWidth, '#');
        std::cout << std::string(barWidth - progressWidth, ' ') << "] ";
        std::cout << (int)(progress * 100) << '%';
        std::cout << std::flush;
    }

    std::cout << std::endl;

    this->buffer1.getTexture().copyToImage().saveToFile(imagePath);
}

void Renderer::reset()
{
    this->frameCount = 1;
    this->subStage = 0;
    this->isProgressive = false;
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

void Renderer::clear()
{
    this->buffer1.clear();
    this->buffer1.display();
    this->buffer2.clear();
    this->buffer2.display();
    this->frameCount = 1;
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
    if (this->textures.size() >= 10)
    {
        return -1;
    }

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

void Renderer::updateTextures()
{
    for (int i = 0; i < this->textures.size(); i++)
    {
        this->shader.setUniform("Textures[" + std::to_string(i) + ']', this->textures[i]);
    }
}

#pragma endregion

const std::string Renderer::ShaderCode = R"(
#version 430 core
#define PI 3.14159265358979323846
#define MAX_TEXTURE_COUNT 10

out vec4 FragColor;

struct Ray
{
    vec3 Origin;
    vec3 Direction;
    vec3 Color;
    vec3 IncomingLight;
};

struct Material
{
    vec3 AlbedoColor;
    vec3 MetalnessColor;
    vec3 EmissionColor;
    vec3 FresnelColor;
    int AlbedoMapId;
    float Roughness;
    float Metalness;
    float EmissionStrength;
    float FresnelStrength;
    float RefractionFactor;
};

struct Sphere
{
    vec3 Origin;
    float Radius;
    int MaterialId;
};

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TextureCoordinate;
};

struct Mesh
{
    vec3 BoxMin;
    vec3 BoxMax;
    vec3 Position;
    vec3 Size;
    vec3 Rotation;
    int IndicesStart;
    int IndicesEnd;
    int MaterialId;
};

struct Box
{
    vec3 Origin;
    vec3 Size;
    int MaterialId;
};

struct CollisionManifold
{
    float Depth;
    vec3 Point;
    vec2 TextureCoordinate;
    vec3 Normal;
    int MaterialId;
    bool IsFrontFace;
};

uniform sampler2D OldBuffer;

uniform vec2 WindowSize;
uniform int SampleCount;
uniform int MaxBouceCount;
uniform int FrameCount;
uniform float FocusStrength;
uniform float FocalLength;

uniform vec3 CameraUp;
uniform vec3 CameraPosition;
uniform vec3 CameraForward;

layout (std430, binding = 1) buffer MaterialBuffer
{
    Material Materials[];
};

layout (std430, binding = 2) buffer VertexBuffer
{
    Vertex Vertices[];
};

uniform int SphereCount;
layout (std430, binding = 3) buffer SphereBuffer
{
    Sphere Spheres[];
};

layout (std430, binding = 4) buffer IndexBuffer
{
    int Indices[];
};

uniform int MeshCount;
layout (std430, binding = 5) buffer MeshBuffer
{
    Mesh Meshes[];
};

uniform int BoxCount;
layout (std430, binding = 6) buffer BoxBuffer
{
    Box Boxes[];
};

uniform sampler2D Textures[MAX_TEXTURE_COUNT];

const float SmallNumber = 0.001;
vec3 CameraRight = cross(CameraForward, CameraUp);
uint Seed = uint((gl_FragCoord.x + gl_FragCoord.y * gl_FragCoord.x / WindowSize.x) * 549856.0) + uint(FrameCount) * 5458u;

uniform struct
{
    vec3 SkyColorHorizon;
    vec3 SkyColorZenith;
    vec3 GroundColor;
    vec3 SunColor;
    vec3 SunDirection;
    float SunFocus;
    float SunIntensity;
    bool Enabled;
} Environment;

vec3 GetEnvironmentLight(Ray ray)
{    
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.Direction.y), 0.35);
    vec3 skyGradient = mix(Environment.SkyColorHorizon, Environment.SkyColorZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.Direction, -Environment.SunDirection)), Environment.SunFocus) * Environment.SunIntensity;
    
    float groundToSkyT = smoothstep(-0.01, 0.0, ray.Direction.y);
    float sunMask = groundToSkyT >= 1.0 ? 1.0 : 0.0;
    return mix(Environment.GroundColor, skyGradient, groundToSkyT) + Environment.SunColor * sun * sunMask;
}

float RandomValue()
{
    Seed = Seed * 747796405u + 2891336453u;
    uint result = ((Seed >> ((Seed >> 28) + 4u)) ^ Seed) * 277803737u;
    result = (result >> 22) ^ result;
    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution()
{
    float theta = 2.0 * PI * RandomValue();
    float rho = sqrt(-2.0 * log(RandomValue()));
    return rho * cos(theta);
}

vec2 RandomVector2()
{
    float angle = RandomValue() * 2.0 * PI;
    return vec2(cos(angle), sin(angle)) * sqrt(RandomValue());
}

vec3 RandomVector3()
{
    float x = RandomValueNormalDistribution();
    float y = RandomValueNormalDistribution();
    float z = RandomValueNormalDistribution();
    return normalize(vec3(x, y, z));
}

bool SphereIntersection(in Ray ray, in Sphere sphere, out CollisionManifold manifold)
{
    vec3 offset = ray.Origin - sphere.Origin;
    float b = dot(offset, ray.Direction);
    float c = dot(offset, offset) - sphere.Radius * sphere.Radius;
    
    float discriminant = b * b - c;
    if (discriminant < 0.0)
    {
        return false;
    }

    discriminant = sqrt(discriminant);

    float tN = -b - discriminant;
    float tF = -b + discriminant;

    if (tN < SmallNumber && tF < SmallNumber)
    {
        return false;
    }

    bool isFrontFace = tN > SmallNumber;
    float depth = isFrontFace ? tN : tF;
    vec3 point = ray.Origin + ray.Direction * depth;

    vec3 uvPoint = -normalize(point);
    float theta = acos(-uvPoint.y);
    float phi = atan(-uvPoint.z, uvPoint.x) + PI;
    manifold = CollisionManifold(
        depth,
        point,
        vec2(phi / (2 * PI), theta / PI),
        normalize(point - sphere.Origin) * (isFrontFace ? 1.0 : -1.0),
        sphere.MaterialId,
        isFrontFace);
    return true;
}

bool TriangleIntersection(in Ray ray, in Vertex v1, in Vertex v2, in Vertex v3, in int materialId, out CollisionManifold manifold)
{
    vec3 edge12 = v2.Position - v1.Position;
    vec3 edge13 = v3.Position - v1.Position;
    vec3 normal = cross(edge12, edge13);
    float det = -dot(ray.Direction, normal);

    bool isFrontFace = det > SmallNumber;
    if (abs(det) < SmallNumber)
    {
        return false;
    }

    vec3 ao = ray.Origin - v1.Position;
    vec3 dao = cross(ao, ray.Direction);

    float invDet = 1.0 / det;
    
    float dst = dot(ao, normal) * invDet;
    float u = dot(edge13, dao) * invDet;
    float v = -dot(edge12, dao) * invDet;
    float w = 1.0 - u - v;
        
    if (dst < SmallNumber || u < 0.0 || v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    manifold = CollisionManifold(
        dst,
        ray.Origin + ray.Direction * dst,
        v1.TextureCoordinate * w + v2.TextureCoordinate * u + v3.TextureCoordinate * v,
        normalize(v1.Normal * w + v2.Normal * u + v3.Normal * v) * (isFrontFace ? 1.0 : -1.0),
        materialId,
        isFrontFace);
    return true;
}

bool BoxIntersection(in Ray ray, in Box box, out CollisionManifold manifold)
{
    vec3 m = 1.0 / ray.Direction;
    vec3 n = m * (ray.Origin - box.Origin);
    vec3 k = abs(m) * box.Size / 2.0;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);
    if (tN > tF || tF < SmallNumber)
    {
        return false;
    }

    bool isFrontFace = tN > SmallNumber;

    vec3 normal = -sign(ray.Direction) * (isFrontFace ?  step(vec3(tN), t1) : step(t2, vec3(tF)));
    float depth = isFrontFace? tN : tF;
    manifold = CollisionManifold(
        depth,
        ray.Origin + ray.Direction * depth,
        vec2(0, 0),
        normal,
        box.MaterialId,
        isFrontFace);
    return true;
}

bool AABBIntersection(in Ray ray, in vec3 invRayDir, in vec3 boxMin, in vec3 boxMax)
{
    vec3 tMin = (boxMin - ray.Origin) * invRayDir;
    vec3 tMax = (boxMax - ray.Origin) * invRayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);
    return tN <= tF && tF >= SmallNumber;
}

bool FindIntersection(in Ray ray, out CollisionManifold manifold)
{
    manifold.Depth = 1.0 / 0.0;

    vec3 invRayDir = 1.0 / ray.Direction;

    for (int i = 0; i < SphereCount; i++)
    {
        CollisionManifold current;
        vec3 radius = vec3(Spheres[i].Radius);
        vec3 boxMin = Spheres[i].Origin - radius;
        vec3 boxMax = Spheres[i].Origin + radius;
        if (AABBIntersection(ray, invRayDir, boxMin, boxMax) &&
            SphereIntersection(ray, Spheres[i], current) &&
            current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }

    for (int i = 0; i < MeshCount; i++)
    {
        Mesh mesh = Meshes[i];

        for (int index = mesh.IndicesStart; index < mesh.IndicesEnd; index += 3)
        {
            CollisionManifold current;
            Vertex v1 = Vertices[Indices[index]];
            Vertex v2 = Vertices[Indices[index + 1]];
            Vertex v3 = Vertices[Indices[index + 2]];
            if (AABBIntersection(ray, invRayDir, mesh.BoxMin, mesh.BoxMax) &&
                TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) &&
                current.Depth < manifold.Depth)
            {
                manifold = current;
            }
        }
    }

    for (int i = 0; i < BoxCount; i++)
    {
        CollisionManifold current;
        if (BoxIntersection(ray, Boxes[i], current) && current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }

    return !isinf(manifold.Depth);
}

void CollisionReact(inout Ray ray, in CollisionManifold manifold)
{
    Material material = Materials[manifold.MaterialId];

    vec3 reactedDir = material.RefractionFactor > 0 ?
        refract(ray.Direction, manifold.Normal, manifold.IsFrontFace ? material.RefractionFactor : 1.0 / material.RefractionFactor) :
        normalize(RandomVector3() + manifold.Normal);
    vec3 reflectedDir = reflect(ray.Direction, manifold.Normal);

    float fresnelValue = material.FresnelStrength > 0.0 ? 1.0 - pow(dot(manifold.Normal, -ray.Direction), material.FresnelStrength) : 0.0;
    float fresnelMask = fresnelValue >= RandomValue() ? 0.0 : 1.0;

    ray.Origin = manifold.Point;
    ray.Direction = normalize(mix(reflectedDir, reactedDir, material.Roughness * fresnelMask));

    ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
    ray.Color *= material.FresnelStrength > 0.0 && fresnelMask < 1.0 ? 
        material.FresnelColor : 
        material.Metalness >= RandomValue() ? 
        material.MetalnessColor :
        (material.AlbedoMapId >= 0 ? texture(Textures[material.AlbedoMapId], manifold.TextureCoordinate).rgb : vec3(1)) *
        material.AlbedoColor;
}

vec3 SendRay(in Ray ray)
{
    for (int i = 0; i <= MaxBouceCount; i++)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, manifold))
        {
            if (Environment.Enabled)
            {
                ray.IncomingLight += GetEnvironmentLight(ray) * ray.Color;
            }

            break;
        }

        CollisionReact(ray, manifold);
    }

    return ray.IncomingLight;
}

vec3 SendRayFlow(in Ray ray)
{
    vec3 focalPoint = ray.Origin + ray.Direction * FocalLength;
    
    vec3 resultLight = vec3(0);
    for (int i = 0; i < SampleCount; i++)
    {
        vec2 jitter = RandomVector2() * FocusStrength;
        vec3 jitterOrigin = ray.Origin + jitter.x * CameraRight + jitter.y * CameraUp;
        vec3 jitterDirection = normalize(focalPoint - jitterOrigin);
        resultLight += SendRay(Ray(jitterOrigin, jitterDirection, ray.Color, ray.IncomingLight));
    }
    
    return resultLight / float(SampleCount);
}

void main()
{
    float aspectRatio = WindowSize.x / WindowSize.y;
    vec2 coord = (gl_FragCoord.xy - WindowSize / 2) / WindowSize * vec2(aspectRatio, 1);
    Ray ray = Ray(CameraPosition, normalize(CameraForward + CameraRight * coord.x + CameraUp * coord.y), vec3(1), vec3(0));

    vec3 newColor = SendRayFlow(ray);

    // Tone mapping
    const float white = 4.0;
    const float exposure = 16.0;
    newColor *= white * exposure;
    newColor = (newColor * (1.0 + newColor / white / white)) / (1.0 + newColor);

    // Progressive rendering mixing
    vec3 oldColor = texture(OldBuffer, gl_FragCoord.xy / WindowSize).rgb;
    float weight = 1.0 / float(FrameCount);
    FragColor = vec4(mix(oldColor, newColor, weight), 1.0);
}

)";

}
