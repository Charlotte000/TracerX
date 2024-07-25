#include <TracerX/Renderer.h>

using namespace TracerX;

const char* Renderer::shaderSrc =
R"(
#version 430 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

const float INV_PI     = 0.31830988618379067;
const float INV_TWO_PI = 0.15915494309189533;
const float TWO_PI     = 6.28318530717958648;

struct Ray
{
    vec3 Origin;
    vec3 Direction;
    vec3 InvDirection;
};

struct Env
{
    mat3 Rotation;
    bool Transparent;
    float Intensity;
};

struct Cam
{
    vec3 Position;
    float FOV;
    vec3 Forward;
    float FocalDistance;
    vec3 Up;
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
    vec4 PositionU;
    vec4 NormalV;
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
    int TriangleSize;
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
    vec4 BboxMin;
    vec4 BboxMax;
    int Start;
    int PrimitiveCount;
    int RightOffset;

    int Padding;
};
layout(rgba32f, binding=0)           uniform image2D AccumulatorImage;
layout(rgba32f, binding=1) writeonly uniform image2D AlbedoImage;
layout(rgba32f, binding=2) writeonly uniform image2D NormalImage;
layout(r32f,    binding=3) writeonly uniform image2D DepthImage;
layout(rgba32f, binding=4) writeonly uniform image2D ToneMapImage;

layout(binding=0) uniform sampler2D EnvironmentTexture;
layout(binding=1) uniform sampler2DArray Textures;

layout(std430, binding=0) readonly buffer VertexSSBO
{
    Vertex Vertices[];
};

layout(std430, binding=1) readonly buffer TriangleSSBO
{
    Triangle Triangles[];
};

layout(std430, binding=2) readonly buffer MeshSSBO
{
    Mesh Meshes[];
};

layout(std430, binding=3) readonly buffer MaterialSSBO
{
    Material Materials[];
};

layout(std430, binding=4) readonly buffer BvhSSBO
{
    Node BVH[];
};

layout(std140, binding=0) uniform CameraUBO
{
    Cam Camera;
};

layout(std140, binding=1) uniform EnvironmentUBO
{
    Env Environment;
};

layout(std140, binding=2) uniform ParamsUBO
{
    ivec2 UVlow;
    ivec2 UVup;
    uint SampleCount;
    uint MaxBounceCount;
    float MinRenderDistance;
    float MaxRenderDistance;
    float Gamma;
    bool OnlyToneMapping;
};

ivec2 Size = imageSize(AccumulatorImage);
ivec2 TexelCoord = ivec2(gl_GlobalInvocationID.xy);
vec2 UV = vec2(TexelCoord) / Size;
vec3 CameraRight = cross(Camera.Forward, Camera.Up);

vec3 GetEnvironment(in Ray ray)
{
    vec3 direction = Environment.Rotation * ray.Direction;
    float u = atan(direction.z, direction.x) * INV_TWO_PI + 0.5;
    float v = acos(direction.y) * INV_PI;
    return texture(EnvironmentTexture, vec2(u, v)).rgb * Environment.Intensity;
}
uint Seed = uint((UV.x + UV.y * UV.x) * 549856.0) + SampleCount * 5458u;

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

void JitterRay(inout Ray ray)
{
    // Focal
    vec3 focalPoint = ray.Origin + ray.Direction * Camera.FocalDistance;
    vec2 focal = RandomVector2() * Camera.Aperture;
    ray.Origin += focal.x * CameraRight + focal.y * Camera.Up;
    ray.Direction = normalize(focalPoint - ray.Origin);

    // Blur
    vec2 blur = RandomVector2() * Camera.Blur;
    ray.Origin += blur.x * CameraRight + blur.y * Camera.Up;

    ray.InvDirection = 1 / ray.Direction;
}

vec4 NormalToColor(in vec3 normal)
{
    return vec4((normal + 1) / 2, 1);
}

vec4 DepthToColor(in float depth)
{
    float nonLinear = (1 / depth - 1 / MinRenderDistance) / (1 / MaxRenderDistance - 1 / MinRenderDistance);
    return vec4(vec3(nonLinear), 1);
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
    vec3 edge12 = v2.PositionU.xyz - v1.PositionU.xyz;
    vec3 edge13 = v3.PositionU.xyz - v1.PositionU.xyz;
    vec3 normal = cross(edge12, edge13);
    float det = -dot(ray.Direction, normal);

    if (abs(det) <= length(normal) * 0.01)
    {
        return false;
    }

    vec3 ao = ray.Origin - v1.PositionU.xyz;
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

    vec2 uv1 = vec2(v1.PositionU.w, v1.NormalV.w);
    vec2 uv2 = vec2(v2.PositionU.w, v2.NormalV.w);
    vec2 uv3 = vec2(v3.PositionU.w, v3.NormalV.w);
    
    vec2 edgeUV12 = uv2 - uv1;
    vec2 edgeUV13 = uv3 - uv1;
    float invDetUV = 1.0 / (edgeUV12.x * edgeUV13.y - edgeUV12.y * edgeUV13.x);

    manifold = CollisionManifold(
        dst,
        ray.Origin + ray.Direction * dst,
        uv1 * w + uv2 * u + uv3 * v,
        normalize(v1.NormalV.xyz * w + v2.NormalV.xyz * u + v3.NormalV.xyz * v),
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

bool MeshIntersection(in Ray ray, in Mesh mesh, in float minDistance, out CollisionManifold manifold)
{
    vec3 rayOrigin = ray.Origin;
    ray.Origin = Transform(ray.Origin, mesh.TransformInv, true);
    ray.Direction = normalize(Transform(ray.Direction, mesh.TransformInv, false));
    ray.InvDirection = 1 / ray.Direction;

    float localMinRenderDistance = length(Transform(ray.Direction * minDistance, mesh.TransformInv, false));
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

        Node node = BVH[ni];

        if (near > manifold.Depth) continue;

        if (node.RightOffset == 0)
        {
            for (int o = 0; o < node.PrimitiveCount; ++o)
            {
                Triangle triangle = Triangles[node.Start + o + mesh.TriangleOffset];

                Vertex v1 = Vertices[triangle.V1];
                Vertex v2 = Vertices[triangle.V2];
                Vertex v3 = Vertices[triangle.V3];

                CollisionManifold current;
                if (TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) && current.Depth < manifold.Depth && current.Depth >= localMinRenderDistance)
                {
                    manifold = current;
                }
            }
        }
        else
        {
            Node c0 = BVH[ni + 1];
            Node c1 = BVH[ni + node.RightOffset];

            bool hitc0 = AABBIntersection(ray, c0.BboxMin.xyz, c0.BboxMax.xyz, bbhits[0], bbhits[1]);
            bool hitc1 = AABBIntersection(ray, c1.BboxMin.xyz, c1.BboxMax.xyz, bbhits[2], bbhits[3]);

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

bool FindIntersection(in Ray ray, in float minDistance, out CollisionManifold manifold)
{
    manifold.Depth = MaxRenderDistance;

    int meshCount = Meshes.length();
    for (int meshId = 0; meshId < meshCount; meshId++)
    {
        Mesh mesh = Meshes[meshId];

        CollisionManifold current;
        if (MeshIntersection(ray, mesh, minDistance, current) && current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }

    return manifold.Depth < MaxRenderDistance;
}

bool CollisionReact(inout Ray ray, inout vec3 throughput, inout vec3 radiance, inout CollisionManifold manifold)
{
    Material material = Materials[manifold.MaterialId];

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

        radiance += material.EmissionColor * throughput;
        throughput *= material.FresnelColor;
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

        radiance += material.EmissionColor * throughput;
        throughput *= material.AlbedoColor;
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

        radiance += material.EmissionColor * throughput;
        throughput *= material.AlbedoColor;
        return true;
    }

    // Scatter
    ray.Origin = manifold.Point;
    ray.Direction = Slerp(specularDir, diffuseDir, material.Roughness);

    radiance += material.EmissionColor * throughput;
    throughput *= material.AlbedoColor;
    return true;
}

vec4 PathTrace(in Ray ray, out vec3 albedo, out vec3 normal, out float depth)
{
    albedo = normal = vec3(0);
    depth = 0;

    vec3 throughput = vec3(1);
    vec3 radiance = vec3(0);

    bool isFirstSolidHit = true;
    uint bounce = 0;
    while (bounce <= MaxBounceCount)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, isFirstSolidHit ? MinRenderDistance : 0, manifold))
        {
            radiance += GetEnvironment(ray) * throughput;
            if (bounce == 0)
            {
                albedo = radiance;
                normal = -ray.Direction;
                depth = MaxRenderDistance;
            }

            return vec4(radiance, bounce == 0 && Environment.Transparent ? 0 : 1);
        }

        if (CollisionReact(ray, throughput, radiance, manifold))
        {
            ray.InvDirection = 1 / ray.Direction;
            if (bounce == 0)
            {
                albedo = throughput;
                normal = manifold.Normal;
                depth += manifold.Depth;
            }

            bounce++;
        }
        else
        {
            ray.Origin = manifold.Point;
            if (bounce == 0)
            {
                depth += manifold.Depth;
            }
        }

        isFirstSolidHit = false;
    }

    return vec4(radiance, 1);
}

void main()
{
    if (any(lessThan(TexelCoord, UVlow)) || any(lessThanEqual(UVup, TexelCoord)))
    {
        return;
    }

    if (OnlyToneMapping)
    {
        vec4 accumColor = imageLoad(AccumulatorImage, TexelCoord);
        imageStore(ToneMapImage, TexelCoord, ToneMap(accumColor / (SampleCount), Gamma));
        return;
    }

    vec2 coord = (UV - vec2(.5)) * vec2(1, float(Size.y) / Size.x) * 2 * tan(Camera.FOV / 2);
    Ray ray = Ray(Camera.Position, normalize(Camera.Forward + CameraRight * coord.x + Camera.Up * coord.y), vec3(0));
    JitterRay(ray);

    vec3 albedo, normal;
    float depth;
    vec4 accumColor = PathTrace(ray, albedo, normal, depth) + imageLoad(AccumulatorImage, TexelCoord);

    imageStore(AccumulatorImage, TexelCoord, accumColor);
    imageStore(AlbedoImage, TexelCoord, ToneMap(vec4(albedo, 1), Gamma));
    imageStore(NormalImage, TexelCoord, NormalToColor(normal));
    imageStore(DepthImage, TexelCoord, DepthToColor(depth));
    imageStore(ToneMapImage, TexelCoord, ToneMap(accumColor / (SampleCount + 1), Gamma));
}

)";
