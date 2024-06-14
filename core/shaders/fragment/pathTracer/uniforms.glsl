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

Triangle GetTriangle(int index)
{
    ivec4 data = texelFetch(Triangles, index);
    return Triangle(data.x, data.y, data.z);
}

Vertex GetVertex(int index)
{
    vec4 data1 = texelFetch(Vertices, index * 3 + 0);
    vec4 data2 = texelFetch(Vertices, index * 3 + 1);
    vec4 data3 = texelFetch(Vertices, index * 3 + 2);
    return Vertex(data1.xyz, data2.xyz, data3.xy);
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
