#version 430 core

const float PI         = 3.14159265358979323;
const float INV_PI     = 0.31830988618379067;
const float TWO_PI     = 6.28318530717958648;
const float INV_TWO_PI = 0.15915494309189533;

in vec2 TexCoords;
out vec4 FragColor;

#include structs.glsl
#include uniforms.glsl
#include random.glsl
#include intersection.glsl


vec3 Slerp(in vec3 a, in vec3 b, float t)
{
    float angle = acos(dot(a, b));
    return isnan(angle) || angle == 0 ? b : (sin((1 - t) * angle) * a + sin(t * angle) * b) / sin(angle);
}

void CollisionReact(inout Ray ray, in CollisionManifold manifold)
{
    Material material = GetMaterial(manifold.MaterialId);

    if (material.MetalnessTextureId >= 0)
    {
        material.Metalness *= texture(Textures, vec3(manifold.TextureCoordinate, material.MetalnessTextureId)).b;
    }

    if (material.RoughnessTextureId >= 0)
    {
        material.Roughness *= texture(Textures, vec3(manifold.TextureCoordinate, material.RoughnessTextureId)).g;
    }

    if (material.AlbedoTextureId >= 0)
    {
        material.AlbedoColor *= texture(Textures, vec3(manifold.TextureCoordinate, material.AlbedoTextureId)).rgb;
    }

    material.EmissionColor *= material.EmissionStrength;
    if (material.EmissionTexureId >= 0)
    {
        material.EmissionColor *= texture(Textures, vec3(manifold.TextureCoordinate, material.EmissionTexureId)).rgb;
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
        return;
    }

    // Density
    if (material.Density > 0.0)
    {
        float depth = -log(RandomValue()) / material.Density;
        if (manifold.IsFrontFace || depth >= manifold.Depth)
        {
            ray.Origin = manifold.Point;
            return;
        }

        ray.Origin += ray.Direction * depth;
        ray.Direction = RandomVector3();

        ray.IncomingLight += material.EmissionColor * ray.Color;
        ray.Color *= material.AlbedoColor;
        return;
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
        return;
    }

    // Scatter
    ray.Origin = manifold.Point;
    ray.Direction = Slerp(specularDir, diffuseDir, material.Roughness);

    ray.IncomingLight += material.EmissionColor * ray.Color;
    ray.Color *= material.AlbedoColor;
}

vec4 SendRay(in Ray ray)
{
    bool isBackground = false;
    for (uint i = 0; i <= MaxBouceCount; i++)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, i == 0, manifold))
        {
            ray.IncomingLight += GetEnvironment(ray) * ray.Color;
            if (i == 0)
            {
                isBackground = true;
            }

            break;
        }

        CollisionReact(ray, manifold);
        ray.InvDirection = 1 / ray.Direction;
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
    FragColor = pixelColor + accumColor;
}