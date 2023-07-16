#include <stdexcept>
#include <algorithm>
#include <GL/glew.h>
#include <OBJ-Loader/OBJLoader.h>
#include <SFML/Graphics.hpp>
#include <TracerX/Renderer.h>

namespace TracerX
{

void Renderer::create(sf::Vector2i size, const Camera& camera, int sampleCount, int maxBounceCount)
{
    this->camera = camera;
    this->buffer1.create(size.x, size.y);
    this->buffer2.create(size.x, size.y);

    this->loadShader();

    // UBO
    this->size.create(&this->shader, "WindowSize", (sf::Vector2f)size);
    this->frameCount.create(&this->shader, "FrameCount", 1);
    this->maxBounceCount.create(&this->shader, "MaxBouceCount", maxBounceCount);
    this->sampleCount.create(&this->shader, "SampleCount", sampleCount);
    this->environment.create(&this->shader);
    this->camera.create(&this->shader);

    // SSBO
    this->materials.create(this, &this->shader, 1);
    this->vertices.create(this, &this->shader, 2);
    this->spheres.create(this, &this->shader, 3, "SphereCount");
    this->indices.create(this, &this->shader, 4);
    this->meshes.create(this, &this->shader, 5, "MeshCount");
    this->boxes.create(this, &this->shader, 6, "BoxCount");

    this->updateTextures();
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
}

void Renderer::renderFrame()
{
    // Update shader parameters
    this->maxBounceCount.updateShader();
    this->sampleCount.updateShader();
    this->frameCount.updateShader();
    this->size.updateShader();
    this->environment.updateShader();
    this->camera.updateShader();

    this->materials.updateShader();
    this->vertices.updateShader();
    this->spheres.updateShader();
    this->indices.updateShader();
    this->meshes.updateShader();
    this->boxes.updateShader();

    // Calculate subframe coordinate
    sf::Vector2i size = sf::Vector2i(this->size.get().x / this->subDivisor.x, this->size.get().y / this->subDivisor.y);
    int y = this->subStage / this->subDivisor.x;
    int x = this->subStage - y * this->subDivisor.x;
    x *= size.x;
    y *= size.y;
    this->subFrame = sf::IntRect(x, y, size.x, size.y);

    // Swap target textures
    this->targetTexture = (this->frameCount.get() & 1) == 1 ? &this->buffer1 : &this->buffer2;
    this->bufferTargetTexture = (this->frameCount.get() & 1) == 1 ? &this->buffer2 : &this->buffer1;
    
    // Ray trace into new buffer
    sf::Sprite newSprite(this->targetTexture->getTexture());
    sf::Sprite oldSprite(this->bufferTargetTexture->getTexture());
    oldSprite.setTextureRect(this->subFrame);
    oldSprite.setPosition((float)this->subFrame.left, (float)this->subFrame.top);
    this->targetTexture->draw(oldSprite, &this->shader);
    this->targetTexture->display();

    // Update subframe
    this->subStage++;
    if (this->subStage >= this->subDivisor.x * this->subDivisor.y)
    {
        this->subStage = 0;
        this->frameCount.set(this->frameCount.get() + 1);

        // Update animation
        if (this->animation.hasLoaded() && this->frameCount.get() > this->animation.frameIteration)
        {
            this->saveToFile(this->animation.name + std::to_string(this->animation.currentFrame) + ".png");
            this->frameCount.set(1);
            this->buffer1.clear();
            this->buffer2.clear();
            this->buffer1.display();
            this->buffer2.display();

            if (!this->animation.getNextFrame(this->camera))
            {
                this->animation = Animation();
                return;
            }
        }
    }
}

void Renderer::saveToFile(const std::string& name)
{
    this->targetTexture->getTexture().copyToImage().saveToFile(name);
}

int Renderer::getPixelDifference() const
{
    sf::Image image1 = this->buffer1.getTexture().copyToImage();
    sf::Image image2 = this->buffer2.getTexture().copyToImage();

    int result = 0;
    for (int x = 0; x < this->size.get().x; x++)
    {
        for (int y = 0; y < this->size.get().y; y++)
        {
            if (image1.getPixel(x, y) != image2.getPixel(x, y))
            {
                result++;
            }
        }
    }

    return result;
}

int Renderer::getMaterialId(const Material& material)
{
    std::vector<Material>::const_iterator it = std::find(this->materials.get().cbegin(), this->materials.get().cend(), material);
    if (it != this->materials.get().cend())
    {
        return (int)(it - this->materials.get().cbegin());
    }

    this->materials.add(material);
    return (int)(this->materials.get().size() - 1);
}

int Renderer::addTexture(const std::string& filePath)
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

void Renderer::addFile(const std::string& filePath, sf::Vector3f offset, sf::Vector3f scale, sf::Vector3f rotation)
{
    objl::Loader loader;
    if (!loader.LoadFile(filePath))
    {
        throw std::runtime_error("Load file failed");
    }

    for (const objl::Mesh& mesh : loader.LoadedMeshes)
    {
        Material material;
        material.albedoColor = *(sf::Vector3f*)&mesh.MeshMaterial.Kd;
        material.roughness = sqrtf(2.0f / (mesh.MeshMaterial.Ns + 2));
        material.metalnessColor = *(sf::Vector3f*)&mesh.MeshMaterial.Ka;
        material.metalness = mesh.MeshMaterial.Ka.X;
        int materialId = this->getMaterialId(material);

        Mesh myMesh((int)this->indices.get().size(), (int)this->indices.get().size() + (int)mesh.Indices.size(), materialId);

        int verticesOffset = (int)this->vertices.get().size();
        for (auto index : mesh.Indices)
        {
            this->indices.add(index + verticesOffset);
        }

        for (const objl::Vertex& vertex : mesh.Vertices)
        {
            this->vertices.add(Vertex3(
                *(sf::Vector3f*)&vertex.Position,
                *(sf::Vector3f*)&vertex.Normal,
                *(sf::Vector2f*)&vertex.TextureCoordinate));
        }
        
        myMesh.scale(scale, this->indices, this->vertices);
        myMesh.rotate(rotation, this->indices, this->vertices);
        myMesh.offset(offset, this->indices, this->vertices);
        this->meshes.add(myMesh);
    }
}

void Renderer::addCornellBox(const Material& up, const Material& down, const Material& left, const Material& right, const Material& forward, const Material& backward, const Material& lightSource)
{
    this->addCornellBox(up, down, left, right, forward, backward);
    this->boxes.add(Box(sf::Vector3f(0, .998f, 0), sf::Vector3f(.5f, .001f, .5f), this->getMaterialId(lightSource)));
}

void Renderer::addCornellBox(const Material& up, const Material& down, const Material& left, const Material& right, const Material& forward, const Material& backward)
{
    this->boxes.add(Box(sf::Vector3f(0, 1.5f, -.5f), sf::Vector3f(2, 1, 3), this->getMaterialId(up)));
    this->boxes.add(Box(sf::Vector3f(0, -1.5f, -.5f), sf::Vector3f(2, 1, 3), this->getMaterialId(down)));

    this->boxes.add(Box(sf::Vector3f(1.5f, 0, -.5f), sf::Vector3f(1, 2, 3), this->getMaterialId(left)));
    this->boxes.add(Box(sf::Vector3f(-1.5f, 0, -.5f), sf::Vector3f(1, 2, 3), this->getMaterialId(right)));

    this->boxes.add(Box(sf::Vector3f(0, 0, 1.5f), sf::Vector3f(2, 2, 1), this->getMaterialId(forward)));
    this->boxes.add(Box(sf::Vector3f(0, 0, -2.5f), sf::Vector3f(2, 2, 1), this->getMaterialId(backward)));
}

void Renderer::updateTextures()
{
    for (int i = 0; i < this->textures.size(); i++)
    {
        this->shader.setUniform("Textures[" + std::to_string(i) + ']', this->textures[i]);
    }
}

const char Renderer::ShaderCode[] = R"(
#version 430 core
#define PI 3.14159265358979323846
#define MAX_TEXTURE_COUNT 10

out vec4 FragColor;

struct Cam
{
    vec3 PrevPosition;
    vec3 PrevForward;
    vec3 PrevUp;
    vec3 NextPosition;
    vec3 NextForward;
    vec3 NextUp;
    float FOV;
    float FocalLength;
    float FocusStrength;
};

struct Env
{
    vec3 SkyColorHorizon;
    vec3 SkyColorZenith;
    vec3 GroundColor;
    vec3 SunColor;
    vec3 SunDirection;
    float SunFocus;
    float SunIntensity;
    float SkyIntensity;
    bool Enabled;
};

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
    float Density;
};

struct Sphere
{
    vec3 Origin;
    vec3 Rotation;
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
    vec3 Rotation;
    vec3 BoxMin;
    vec3 BoxMax;
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
uniform Cam Camera;
uniform Env Environment;

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
uint Seed = uint((gl_FragCoord.x + gl_FragCoord.y * gl_FragCoord.x / WindowSize.x) * 549856.0) + uint(FrameCount) * 5458u;

vec3 GetEnvironmentLight(in Ray ray)
{    
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.Direction.y), 0.35);
    vec3 skyGradient = mix(Environment.SkyColorHorizon, Environment.SkyColorZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.Direction, -Environment.SunDirection)), Environment.SunFocus) * Environment.SunIntensity;
    
    float groundToSkyT = smoothstep(-0.01, 0.0, ray.Direction.y);
    float sunMask = groundToSkyT >= 1.0 ? 1.0 : 0.0;
    return mix(Environment.GroundColor, skyGradient, groundToSkyT) * Environment.SkyIntensity + Environment.SunColor * sun * sunMask;
}

vec3 Slerp(in vec3 a, in vec3 b, float t)
{
    float angle = acos(dot(a, b));
    return isnan(angle) || angle == 0 ? b : (sin((1 - t) * angle) * a + sin(t * angle) * b) / sin(angle);
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

vec3 RotateXYZ(in vec3 v, in vec3 angle)
{
    vec3 result = v;

    float cosX = cos(angle.x);
    float sinX = sin(angle.x);
    result = vec3(result.x, result.y * cosX - result.z * sinX, result.y * sinX + result.z * cosX);

    float cosY = cos(angle.y);
    float sinY = sin(angle.y);
    result = vec3(result.x * cosY + result.z * sinY, result.y, -result.x * sinY + result.z * cosY);

    float cosZ = cos(angle.z);
    float sinZ = sin(angle.z);
    result = vec3(result.x * cosZ - result.y * sinZ, result.x * sinZ + result.y * cosZ, result.z);
    
    return result;
}

vec3 RotateZYX(in vec3 v, in vec3 angle)
{
    vec3 result = v;

    float cosZ = cos(angle.z);
    float sinZ = sin(angle.z);
    result = vec3(result.x * cosZ - result.y * sinZ, result.x * sinZ + result.y * cosZ, result.z);

    float cosY = cos(angle.y);
    float sinY = sin(angle.y);
    result = vec3(result.x * cosY + result.z * sinY, result.y, -result.x * sinY + result.z * cosY);

    float cosX = cos(angle.x);
    float sinX = sin(angle.x);
    result = vec3(result.x, result.y * cosX - result.z * sinX, result.y * sinX + result.z * cosX);
    
    return result;
}

Ray RotateXYZ(in Ray ray, in vec3 origin, in vec3 angle)
{
    return Ray(RotateXYZ(ray.Origin - origin, angle) + origin, RotateXYZ(ray.Direction, angle), ray.Color, ray.IncomingLight);
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
    vec3 normal = (point - sphere.Origin) / sphere.Radius * (isFrontFace ? 1.0 : -1.0);

    vec3 uv = RotateXYZ(normal, -sphere.Rotation);
    float theta = acos(uv.y);
    float phi = atan(uv.z, -uv.x) + PI;
    manifold = CollisionManifold(
        depth,
        point,
        vec2(phi / (2 * PI), theta / PI),
        normal,
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
    Ray rotatedRay = RotateXYZ(ray, box.Origin, -box.Rotation);
    vec3 m = 1.0 / rotatedRay.Direction;
    vec3 n = m * (rotatedRay.Origin - box.Origin);
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

    vec3 normal = -sign(rotatedRay.Direction) * (isFrontFace ?  step(vec3(tN), t1) : step(t2, vec3(tF)));
    normal = RotateZYX(normal, box.Rotation);
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

        if (AABBIntersection(ray, invRayDir, mesh.BoxMin, mesh.BoxMax))
        {
            for (int index = mesh.IndicesStart; index < mesh.IndicesEnd; index += 3)
            {
                Vertex v1 = Vertices[Indices[index]];
                Vertex v2 = Vertices[Indices[index + 1]];
                Vertex v3 = Vertices[Indices[index + 2]];
            
                CollisionManifold current;
                if (TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) && current.Depth < manifold.Depth)
                {
                    manifold = current;
                }
            }
        }
    }

    for (int i = 0; i < BoxCount; i++)
    {
        CollisionManifold current;
        if (AABBIntersection(ray, invRayDir, Boxes[i].BoxMin, Boxes[i].BoxMax) &&
            BoxIntersection(ray, Boxes[i], current) && current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }

    return !isinf(manifold.Depth);
}

vec3 GetColor(in CollisionManifold manifold)
{
    Material material = Materials[manifold.MaterialId];
    if (material.Metalness >= RandomValue())
    {
        return material.MetalnessColor;
    }

    vec3 albedoMap = material.AlbedoMapId >= 0 ?
        texture(Textures[material.AlbedoMapId], manifold.TextureCoordinate).rgb :
        vec3(1);
    return material.AlbedoColor * albedoMap;
}

void CollisionReact(inout Ray ray, in CollisionManifold manifold)
{
    Material material = Materials[manifold.MaterialId];

    // Fresnel
    if (material.FresnelStrength > 0.0 &&
        1.0 - pow(dot(manifold.Normal, -ray.Direction), material.FresnelStrength) >= RandomValue())
    {
        ray.Origin = manifold.Point;
        ray.Direction = reflect(ray.Direction, manifold.Normal);

        ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
        ray.Color *= material.FresnelColor;
        return;
    }

    // Refraction
    if (material.RefractionFactor > 0.0)
    {
        vec3 reflectedDir = reflect(ray.Direction, manifold.Normal);
        vec3 refractedDir = refract(ray.Direction, manifold.Normal, manifold.IsFrontFace ? material.RefractionFactor : 1.0 / material.RefractionFactor);

        // Density
        if (material.Density > 0.0)
        {
            float depth = -log(RandomValue()) / material.Density;
            if (manifold.IsFrontFace || depth >= manifold.Depth)
            {
                ray.Origin = manifold.Point;
                ray.Direction = normalize(mix(reflectedDir, refractedDir, material.Roughness));
                return;
            }

            ray.Origin += ray.Direction * depth;
            ray.Direction = RandomVector3();

            ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
            ray.Color *= GetColor(manifold);
            return;
        }
        
        ray.Origin = manifold.Point;
        ray.Direction = normalize(mix(reflectedDir, refractedDir, material.Roughness));

        ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
        ray.Color *= GetColor(manifold);
        return;
    }

    // Diffuse
    vec3 reflectedDir = reflect(ray.Direction, manifold.Normal);
    vec3 randomDir = normalize(RandomVector3() + manifold.Normal);

    ray.Origin = manifold.Point;
    ray.Direction = normalize(mix(reflectedDir, randomDir, material.Roughness));

    ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
    ray.Color *= GetColor(manifold);
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

float time = RandomValue();
vec3 CameraPosition = mix(Camera.PrevPosition, Camera.NextPosition, time);
vec3 CameraForward = Slerp(Camera.PrevForward, Camera.NextForward, time);
vec3 CameraUp = Slerp(Camera.PrevUp, Camera.NextUp, time);
vec3 CameraRight = cross(CameraForward, CameraUp);

vec3 SendRayFlow(in Ray ray)
{
    vec3 focalPoint = ray.Origin + ray.Direction * Camera.FocalLength;
    
    vec3 resultLight = vec3(0);
    for (int i = 0; i < SampleCount; i++)
    {
        vec2 jitter = RandomVector2() * Camera.FocusStrength;
        vec3 jitterOrigin = ray.Origin + jitter.x * CameraRight + jitter.y * CameraUp;
        vec3 jitterDirection = normalize(focalPoint - jitterOrigin);
        resultLight += SendRay(Ray(jitterOrigin, jitterDirection, ray.Color, ray.IncomingLight));
    }
    
    return resultLight / float(SampleCount);
}

void main()
{
    float aspectRatio = WindowSize.y / WindowSize.x;
    vec2 coord = (gl_FragCoord.xy - WindowSize / 2) / WindowSize * vec2(1, aspectRatio) * 2 * tan(Camera.FOV / 2);
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
