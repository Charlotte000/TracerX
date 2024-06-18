#include <TracerX/Renderer.h>

using namespace TracerX;

const char* Renderer::accumulatorShaderSrc =
R"(
#version 430 core

in vec2 TexCoords;
layout(location=0) out vec4 AccumulatorColor;
layout(location=1) out vec4 AlbedoColor;
layout(location=2) out vec4 NormalColor;

struct Ray
{
    vec3 Origin;
    vec3 Direction;
    vec3 InvDirection;
    vec3 Color;
    vec3 IncomingLight;
};

struct Env
{
    bool Transparent;
    float Intensity;
    mat3 Rotation;
};

struct Cam
{
    vec3 Position;
    vec3 Forward;
    vec3 Up;
    float FOV;
    float FocalDistance;
    float Aperture;
    float Blur;
};

struct Material
{
    vec3 AlbedoColor;
    float Roughness;
    vec3 EmissionColor;
    float EmissionStrength;
    vec3 FresnelColor;
    float FresnelStrength;
    float Metalness;
    float IOR;
    float Density;
    int AlbedoTextureId;
    int MetalnessTextureId;
    int EmissionTextureId;
    int RoughnessTextureId;
    int NormalTextureId;
};

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TextureCoordinate;
};

struct Triangle
{
    int V1;
    int V2;
    int V3;
};

struct Mesh
{
    mat4 Transform;
    mat4 TransformInv;
    int MaterialId;
    int NodeOffset;
    int TriangleOffset;
};

struct CollisionManifold
{
    float Depth;
    vec3 Point;
    vec2 TextureCoordinate;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    int MaterialId;
    bool IsFrontFace;
};

struct Node
{
    vec3 BboxMin;
    vec3 BboxMax;
    int Start;
    int PrimitiveCount;
    int RightOffset;
};
const float INV_PI     = 0.31830988618379067;
const float INV_TWO_PI = 0.15915494309189533;

layout(binding=0) uniform sampler2D AccumulatorTexture;
layout(binding=1) uniform sampler2D EnvironmentTexture;
layout(binding=2) uniform sampler2DArray Textures;
layout(binding=3) uniform samplerBuffer Vertices;
layout(binding=4) uniform isamplerBuffer Triangles;
layout(binding=5) uniform samplerBuffer Meshes;
layout(binding=6) uniform samplerBuffer Materials;
layout(binding=7) uniform samplerBuffer BVH;

uniform uint MaxBounceCount;
uniform float MinRenderDistance;
uniform float MaxRenderDistance;
uniform uint FrameCount;
uniform Cam Camera;
uniform Env Environment;
uniform float Gamma;

Triangle GetTriangle(int index)
{
    ivec4 data = texelFetch(Triangles, index);
    return Triangle(data.x, data.y, data.z);
}

Vertex GetVertex(int index)
{
    vec4 data1 = texelFetch(Vertices, index * 2 + 0);
    vec4 data2 = texelFetch(Vertices, index * 2 + 1);
    return Vertex(data1.xyz, data2.xyz, vec2(data1.w, data2.w));
}

Mesh GetMesh(int index)
{
    vec4 data1 = texelFetch(Meshes, index * 9 + 0);
    vec4 data2 = texelFetch(Meshes, index * 9 + 1);
    vec4 data3 = texelFetch(Meshes, index * 9 + 2);
    vec4 data4 = texelFetch(Meshes, index * 9 + 3);
    vec4 data5 = texelFetch(Meshes, index * 9 + 4);
    vec4 data6 = texelFetch(Meshes, index * 9 + 5);
    vec4 data7 = texelFetch(Meshes, index * 9 + 6);
    vec4 data8 = texelFetch(Meshes, index * 9 + 7);
    vec4 data9 = texelFetch(Meshes, index * 9 + 8);
    return Mesh(mat4(data1, data2, data3, data4), mat4(data5, data6, data7, data8), int(data9.x), int(data9.y), int(data9.z));
}

int GetMeshCount()
{
    return textureSize(Meshes) / 9;
}

Material GetMaterial(int index)
{
    vec4 data1 = texelFetch(Materials, index * 5 + 0);
    vec4 data2 = texelFetch(Materials, index * 5 + 1);
    vec4 data3 = texelFetch(Materials, index * 5 + 2);
    vec4 data4 = texelFetch(Materials, index * 5 + 3);
    vec4 data5 = texelFetch(Materials, index * 5 + 4);
    return Material(data1.rgb, data1.a, data2.rgb, data2.a, data3.rgb, data3.a, data4.x, data4.y, data4.z, int(data4.w), int(data5.x), int(data5.y), int(data5.z), int(data5.w));
}

Node GetNode(int index)
{
    vec4 data1 = texelFetch(BVH, index * 3 + 0);
    vec4 data2 = texelFetch(BVH, index * 3 + 1);
    vec4 data3 = texelFetch(BVH, index * 3 + 2);
    return Node(data1.xyz, data2.xyz, int(data3.x), int(data3.y), int(data3.z));
}

vec3 GetEnvironment(in Ray ray)
{
    vec3 direction = Environment.Rotation * ray.Direction;
    float u = atan(direction.z, direction.x) * INV_TWO_PI + 0.5;
    float v = acos(direction.y) * INV_PI;
    return texture(EnvironmentTexture, vec2(u, v)).rgb * Environment.Intensity;
}
const float TWO_PI     = 6.28318530717958648;

uint Seed = uint((TexCoords.x + TexCoords.y * TexCoords.x) * 549856.0) + FrameCount * 5458u;

float RandomValue()
{
    Seed = Seed * 747796405u + 2891336453u;
    uint result = ((Seed >> ((Seed >> 28) + 4u)) ^ Seed) * 277803737u;
    result = (result >> 22) ^ result;
    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution()
{
    float theta = TWO_PI * RandomValue();
    float rho = sqrt(-2.0 * log(RandomValue()));
    return rho * cos(theta);
}

vec2 RandomVector2()
{
    float angle = RandomValue() * TWO_PI;
    return vec2(cos(angle), sin(angle)) * sqrt(RandomValue());
}

vec3 RandomVector3()
{
    float x = RandomValueNormalDistribution();
    float y = RandomValueNormalDistribution();
    float z = RandomValueNormalDistribution();
    return normalize(vec3(x, y, z));
}
vec3 Slerp(in vec3 a, in vec3 b, float t)
{
    float angle = acos(dot(a, b));
    return isnan(angle) || angle == 0 ? b : (sin((1 - t) * angle) * a + sin(t * angle) * b) / sin(angle);
}

vec3 Transform(in vec3 v, in mat4 matrix, in bool translate)
{
    return (matrix * vec4(v, translate ? 1 : 0)).xyz;
}

vec4 ToneMap(in vec4 pixel, in float gamma)
{
    // Reinhard tone mapping
    pixel.rgb = pixel.rgb / (pixel.rgb + vec3(1));

    // Gamma correction
    pixel.rgb = pow(pixel.rgb, vec3(1 / gamma));

    return pixel;
}
void swap(inout float a, inout float b)
{
    float tmp = a;
    a = b;
    b = tmp;
}

void swap(inout int a, inout int b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

bool TriangleIntersection(in Ray ray, in Vertex v1, in Vertex v2, in Vertex v3, in int materialId, out CollisionManifold manifold)
{
    vec3 edge12 = v2.Position - v1.Position;
    vec3 edge13 = v3.Position - v1.Position;
    vec3 normal = cross(edge12, edge13);
    float det = -dot(ray.Direction, normal);

    if (abs(det) <= length(normal) * 0.01)
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

    if (dst <= 0.001 || u < 0.0 || v < 0.0 || w < 0.0)
    {
        return false;
    }

    vec2 edgeUV12 = v2.TextureCoordinate - v1.TextureCoordinate;
    vec2 edgeUV13 = v3.TextureCoordinate - v1.TextureCoordinate;
    float invDetUV = 1.0 / (edgeUV12.x * edgeUV13.y - edgeUV12.y * edgeUV13.x);

    manifold = CollisionManifold(
        dst,
        ray.Origin + ray.Direction * dst,
        v1.TextureCoordinate * w + v2.TextureCoordinate * u + v3.TextureCoordinate * v,
        normalize(v1.Normal * w + v2.Normal * u + v3.Normal * v),
        normalize((edge12 * edgeUV13.y - edge13 * edgeUV12.y) * invDetUV),
        normalize((edge13 * edgeUV12.x - edge12 * edgeUV13.x) * invDetUV),
        materialId,
        det >= 0);
    return true;
}

bool AABBIntersection(in Ray ray, in vec3 boxMin, in vec3 boxMax, out float tNear, out float tFar)
{
    vec3 tMin = (boxMin - ray.Origin) * ray.InvDirection;
    vec3 tMax = (boxMax - ray.Origin) * ray.InvDirection;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    tNear = max(max(t1.x, t1.y), t1.z);
    tFar = min(min(t2.x, t2.y), t2.z);
    return tNear <= tFar && tFar >= 0;
}

bool MeshIntersection(in Ray ray, in Mesh mesh, in bool firstHit, out CollisionManifold manifold)
{
    vec3 rayOrigin = ray.Origin;
    ray.Origin = Transform(ray.Origin, mesh.TransformInv, true);
    ray.Direction = normalize(Transform(ray.Direction, mesh.TransformInv, false));
    ray.InvDirection = 1 / ray.Direction;

    float localMinRenderDistance = length(Transform(ray.Direction * MinRenderDistance, mesh.TransformInv, false));
    float localMaxRenderDistance = length(Transform(ray.Direction * MaxRenderDistance, mesh.TransformInv, false));
    manifold.Depth = localMaxRenderDistance;

    float bbhits[4];

    vec2 todo[64];
    int stackptr = 0;

    todo[stackptr] = vec2(mesh.NodeOffset, -1);

    while (stackptr >= 0)
    {
        int ni = int(todo[stackptr].x);
        float near = todo[stackptr].y;
        stackptr--;

        Node node = GetNode(ni);

        if (near > manifold.Depth) continue;

        if (node.RightOffset == 0)
        {
            for (int o = 0; o < node.PrimitiveCount; ++o)
            {
                Triangle triangle = GetTriangle(node.Start + o + mesh.TriangleOffset);

                Vertex v1 = GetVertex(triangle.V1);
                Vertex v2 = GetVertex(triangle.V2);
                Vertex v3 = GetVertex(triangle.V3);

                CollisionManifold current;
                if (TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) && current.Depth < manifold.Depth && (!firstHit || current.Depth >= localMinRenderDistance))
                {
                    manifold = current;
                }
            }
        }
        else
        {
            Node c0 = GetNode(ni + 1);
            Node c1 = GetNode(ni + node.RightOffset);

            bool hitc0 = AABBIntersection(ray, c0.BboxMin, c0.BboxMax, bbhits[0], bbhits[1]);
            bool hitc1 = AABBIntersection(ray, c1.BboxMin, c1.BboxMax, bbhits[2], bbhits[3]);

            if (hitc0 && hitc1)
            {
                int closer = ni + 1;
                int other = ni + node.RightOffset;

                if (bbhits[2] < bbhits[0])
                {
                    swap(bbhits[0], bbhits[2]);
                    swap(bbhits[1], bbhits[3]);
                    swap(closer, other);
                }

                todo[++stackptr] = vec2(other, bbhits[2]);
                todo[++stackptr] = vec2(closer, bbhits[0]);
            }
            else if (hitc0)
            {
                todo[++stackptr] = vec2(ni + 1, bbhits[0]);
            }
            else if (hitc1)
            {
                todo[++stackptr] = vec2(ni + node.RightOffset, bbhits[2]);
            }
        }
    }

    if (manifold.Depth < localMaxRenderDistance)
    {
        manifold.Point = Transform(manifold.Point, mesh.Transform, true);
        manifold.Depth = length(manifold.Point - rayOrigin);
        manifold.Normal = normalize(Transform(manifold.Normal, mesh.Transform, false));
        manifold.Tangent = normalize(Transform(manifold.Tangent, mesh.Transform, false));
        manifold.Bitangent = normalize(Transform(manifold.Bitangent, mesh.Transform, false));
        return true;
    }

    return false;
}

bool FindIntersection(in Ray ray, in bool firstHit, out CollisionManifold manifold)
{
    manifold.Depth = MaxRenderDistance;

    int meshCount = GetMeshCount();
    for (int meshId = 0; meshId < meshCount; meshId++)
    {
        Mesh mesh = GetMesh(meshId);

        CollisionManifold current;
        if (MeshIntersection(ray, mesh, firstHit, current) && current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }

    return manifold.Depth < MaxRenderDistance;
}

bool CollisionReact(inout Ray ray, inout CollisionManifold manifold)
{
    Material material = GetMaterial(manifold.MaterialId);

    if (material.AlbedoTextureId >= 0)
    {
        vec4 texAlbedo = texture(Textures, vec3(manifold.TextureCoordinate, material.AlbedoTextureId));
        material.AlbedoColor *= texAlbedo.rgb;

        // Alpha blend
        if (texAlbedo.a < RandomValue())
        {
            return false;
        }
    }

    if (material.MetalnessTextureId >= 0)
    {
        material.Metalness *= texture(Textures, vec3(manifold.TextureCoordinate, material.MetalnessTextureId)).b;
    }

    if (material.RoughnessTextureId >= 0)
    {
        material.Roughness *= texture(Textures, vec3(manifold.TextureCoordinate, material.RoughnessTextureId)).g;
    }

    material.EmissionColor *= material.EmissionStrength;
    if (material.EmissionTextureId >= 0)
    {
        material.EmissionColor *= texture(Textures, vec3(manifold.TextureCoordinate, material.EmissionTextureId)).rgb;
    }

    if (material.NormalTextureId >= 0)
    {
        vec3 texNormal = texture(Textures, vec3(manifold.TextureCoordinate, material.NormalTextureId)).rgb;
        texNormal.y = 1 - texNormal.y;
        texNormal = normalize(texNormal * 2 - 1);
        manifold.Normal = normalize(manifold.Tangent * texNormal.x + manifold.Bitangent * texNormal.y + manifold.Normal * texNormal.z);
    }

    if (!manifold.IsFrontFace)
    {
        manifold.Normal *= -1;
    }

    vec3 specularDir = reflect(ray.Direction, manifold.Normal);
    vec3 diffuseDir = normalize(RandomVector3() + manifold.Normal);

    if (material.Metalness <= RandomValue() && RandomValue() >= 0.2)
    {
        material.Roughness = 1;
    }

    // Fresnel
    if (material.FresnelStrength > 0.0 &&
        1.0 - pow(dot(manifold.Normal, -ray.Direction), material.FresnelStrength) >= RandomValue())
    {
        ray.Origin = manifold.Point;
        ray.Direction = specularDir;

        ray.IncomingLight += material.EmissionColor * ray.Color;
        ray.Color *= material.FresnelColor;
        return true;
    }

    // Density
    if (material.Density > 0.0)
    {
        float depth = -log(RandomValue()) / material.Density;
        if (manifold.IsFrontFace || depth >= manifold.Depth)
        {
            return false;
        }

        ray.Origin += ray.Direction * depth;
        ray.Direction = RandomVector3();

        ray.IncomingLight += material.EmissionColor * ray.Color;
        ray.Color *= material.AlbedoColor;
        return true;
    }

    // Refract
    if (material.IOR > 0.0)
    {
        vec3 refractedDir = refract(ray.Direction, manifold.Normal, manifold.IsFrontFace ? 1.0 / material.IOR : material.IOR);
        if (refractedDir == vec3(0))
        {
            refractedDir = specularDir;
        }

        ray.Origin = manifold.Point;
        ray.Direction = refractedDir;

        ray.IncomingLight += material.EmissionColor * ray.Color;
        ray.Color *= material.AlbedoColor;
        return true;
    }

    // Scatter
    ray.Origin = manifold.Point;
    ray.Direction = Slerp(specularDir, diffuseDir, material.Roughness);

    ray.IncomingLight += material.EmissionColor * ray.Color;
    ray.Color *= material.AlbedoColor;
    return true;
}

vec4 SendRay(in Ray ray)
{
    bool isBackground = false;

    uint bounce = 0;
    while (bounce <= MaxBounceCount)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, bounce == 0, manifold))
        {
            ray.IncomingLight += GetEnvironment(ray) * ray.Color;
            if (bounce == 0)
            {
                isBackground = true;
                AlbedoColor = ToneMap(vec4(ray.IncomingLight, 1), Gamma);
                NormalColor = vec4((1 - ray.Direction) / 2, 1);
            }

            break;
        }

        if (CollisionReact(ray, manifold))
        {
            ray.InvDirection = 1 / ray.Direction;
            if (bounce == 0)
            {
                AlbedoColor = ToneMap(vec4(ray.Color, 1), Gamma);
                NormalColor = vec4((manifold.Normal + 1) / 2, 1);
            }

            bounce++;
        }
        else
        {
            ray.Origin = manifold.Point;
        }
    }

    return vec4(ray.IncomingLight, Environment.Transparent && isBackground ? 0 : 1);
}

vec3 CameraRight = cross(Camera.Forward, Camera.Up);

vec4 PathTrace(in Ray ray)
{
    vec3 rayOrigin = ray.Origin;
    vec3 rayDirection = ray.Direction;

    // Focal
    vec3 focalPoint = ray.Origin + ray.Direction * Camera.FocalDistance;
    vec2 focal = RandomVector2() * Camera.Aperture;
    rayOrigin += focal.x * CameraRight + focal.y * Camera.Up;
    rayDirection = normalize(focalPoint - rayOrigin);

    // Blur
    vec2 blur = RandomVector2() * Camera.Blur;
    rayOrigin += blur.x * CameraRight + blur.y * Camera.Up;

    return SendRay(Ray(rayOrigin, rayDirection, 1 / rayDirection, ray.Color, ray.IncomingLight));
}

void main()
{
    vec2 size = textureSize(AccumulatorTexture, 0);
    vec2 coord = (TexCoords - vec2(.5)) * vec2(1, size.y / size.x) * 2 * tan(Camera.FOV / 2);
    Ray ray = Ray(Camera.Position, normalize(Camera.Forward + CameraRight * coord.x + Camera.Up * coord.y), vec3(0), vec3(1), vec3(0));

    vec4 pixelColor = PathTrace(ray);

    // Accumulate
    vec4 accumColor = texture(AccumulatorTexture, TexCoords);
    AccumulatorColor = pixelColor + accumColor;
}

)";

const char* Renderer::toneMapperShaderSrc =
R"(
#version 430 core

layout(binding=0) uniform sampler2D Accumulator;

uniform uint FrameCount;
uniform float Gamma;

in vec2 TexCoords;
layout(location=3) out vec4 ToneMapColor;

vec3 Slerp(in vec3 a, in vec3 b, float t)
{
    float angle = acos(dot(a, b));
    return isnan(angle) || angle == 0 ? b : (sin((1 - t) * angle) * a + sin(t * angle) * b) / sin(angle);
}

vec3 Transform(in vec3 v, in mat4 matrix, in bool translate)
{
    return (matrix * vec4(v, translate ? 1 : 0)).xyz;
}

vec4 ToneMap(in vec4 pixel, in float gamma)
{
    // Reinhard tone mapping
    pixel.rgb = pixel.rgb / (pixel.rgb + vec3(1));

    // Gamma correction
    pixel.rgb = pow(pixel.rgb, vec3(1 / gamma));

    return pixel;
}

void main()
{
    vec4 pixel = texture(Accumulator, TexCoords) / FrameCount;
    ToneMapColor = ToneMap(pixel, Gamma);
}

)";

const char* Renderer::vertexShaderSrc =
R"(
#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position.x, position.y, 0, 1);
    TexCoords = texCoords;
}

)";
