#version 430 core

in vec2 TexCoords;
layout(location=0) out vec4 AccumulatorColor;
layout(location=1) out vec4 AlbedoColor;
layout(location=2) out vec4 NormalColor;

#include common/structs.glsl
#include common/uniforms.glsl
#include common/random.glsl
#include common/transforms.glsl
#include common/intersection.glsl

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
