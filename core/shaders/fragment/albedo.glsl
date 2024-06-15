#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

#include common/structs.glsl
#include common/uniforms.glsl
#include common/random.glsl
#include common/transforms.glsl
#include common/intersection.glsl

vec4 SendRay(in Ray ray)
{
    CollisionManifold manifold;
    if (!FindIntersection(ray, true, manifold))
    {
        return vec4(min(vec3(1), GetEnvironment(ray)), 1);
    }

    Material material = GetMaterial(manifold.MaterialId);
    if (material.AlbedoTextureId >= 0)
    {
        material.AlbedoColor *= texture(Textures, vec3(manifold.TextureCoordinate, material.AlbedoTextureId)).rgb;
    }

    return vec4(min(vec3(1), material.AlbedoColor), 1);
}

vec3 CameraRight = cross(Camera.Forward, Camera.Up);

void main()
{
    vec2 size = textureSize(AccumulatorTexture, 0);
    vec2 coord = (TexCoords - vec2(.5)) * vec2(1, size.y / size.x) * 2 * tan(Camera.FOV / 2);
    Ray ray = Ray(Camera.Position, normalize(Camera.Forward + CameraRight * coord.x + Camera.Up * coord.y), vec3(0), vec3(1), vec3(0));

    vec4 pixelColor = SendRay(ray);
    FragColor = pixelColor;
}
