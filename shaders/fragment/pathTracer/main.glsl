#version 430 core

const float MIN_RENDER_DISTANCE = 0.0001;
const float MAX_RENDER_DISTANCE = 1000000.0;

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

vec3 GetEnvironmentLight(in Ray ray)
{
    float theta = acos(ray.Direction.y);
    float phi = atan(ray.Direction.z, ray.Direction.x) + PI;
    return texture(EnvironmentTexture, vec2(phi * INV_TWO_PI, theta * INV_PI)).rgb * EnvironmentIntensity;
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

    // Absorb
    if (material.IOR > 0.0)
    {
        vec3 refractedDir = refract(ray.Direction, manifold.Normal, manifold.IsFrontFace ? 1.0 / material.IOR : material.IOR);

        // Density
        if (material.Density > 0.0)
        {
            float depth = -log(RandomValue()) / material.Density;
            if (manifold.IsFrontFace || depth >= manifold.Depth)
            {
                ray.Origin = manifold.Point;
                ray.Direction = Slerp(specularDir, refractedDir, material.Roughness);
                return;
            }

            ray.Origin += ray.Direction * depth;
            ray.Direction = RandomVector3();

            ray.IncomingLight += material.EmissionColor * ray.Color;
            ray.Color *= material.AlbedoColor;
            return;
        }
        
        ray.Origin = manifold.Point;
        ray.Direction = Slerp(specularDir, refractedDir, material.Roughness);

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

vec3 SendRay(in Ray ray)
{
    for (int i = 0; i <= MaxBouceCount; i++)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, manifold) || (i == 0 && manifold.Depth < MIN_RENDER_DISTANCE))
        {
            ray.IncomingLight += GetEnvironmentLight(ray) * ray.Color;
            break;
        }

        CollisionReact(ray, manifold);
        ray.InvDirection = 1 / ray.Direction;
    }

    return ray.IncomingLight;
}

vec3 CameraRight = cross(Camera.Forward, Camera.Up);

vec3 PathTrace(in Ray ray)
{
    vec3 focalPoint = ray.Origin + ray.Direction * Camera.FocalDistance;

    vec2 jitter = RandomVector2() * Camera.Aperture;
    vec3 jitterOrigin = ray.Origin + jitter.x * CameraRight + jitter.y * Camera.Up;
    vec3 jitterDirection = normalize(focalPoint - jitterOrigin);
    vec3 resultLight = SendRay(Ray(jitterOrigin, jitterDirection, 1 / jitterDirection, ray.Color, ray.IncomingLight));

    return resultLight;
}

void main()
{
    vec2 coord = (TexCoords - vec2(.5)) / vec2(1, Camera.AspectRatio) * 2 * tan(Camera.FOV / 2);
    Ray ray = Ray(Camera.Position, normalize(Camera.Forward + CameraRight * coord.x + Camera.Up * coord.y), vec3(0), vec3(1), vec3(0));

    vec3 pixelColor = PathTrace(ray);

    // Accumulate
    vec3 accumColor = texture(AccumulatorTexture, TexCoords).rgb;
    FragColor = vec4(pixelColor + accumColor, 1);
}
