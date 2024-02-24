layout(binding=1) uniform sampler2D AccumulatorTexture;
layout(binding=2) uniform sampler2D EnvironmentTexture;
layout(binding=3) uniform samplerBuffer Vertices;
layout(binding=4) uniform isamplerBuffer Triangles;
layout(binding=5) uniform sampler2DArray Textures;
layout(binding=6) uniform samplerBuffer Meshes;
layout(binding=7) uniform samplerBuffer Materials;
layout(binding=8) uniform samplerBuffer BVH;

uniform float EnvironmentIntensity;
uniform int MaxBouceCount;
uniform int FrameCount;
uniform Cam Camera;


Triangle GetTriangle(int index)
{
    ivec4 data = texelFetch(Triangles, index);
    return Triangle(data.x, data.y, data.z, data.w);
}

Vertex GetVertex(int index, mat4 transform)
{
    vec4 data1 = texelFetch(Vertices, index * 3 + 0);
    vec4 data2 = texelFetch(Vertices, index * 3 + 1);
    vec4 data3 = texelFetch(Vertices, index * 3 + 2);
    return Vertex((transform * vec4(data1.xyz, 1)).xyz, (transform * vec4(data2.xyz, 0)).xyz, data3.xy);
}

Mesh GetMesh(int index)
{
    vec4 data1 = texelFetch(Meshes, index * 5 + 0);
    vec4 data2 = texelFetch(Meshes, index * 5 + 1);
    vec4 data3 = texelFetch(Meshes, index * 5 + 2);
    vec4 data4 = texelFetch(Meshes, index * 5 + 3);
    vec4 data5 = texelFetch(Meshes, index * 5 + 4);
    return Mesh(mat4(data1, data2, data3, data4), int(data5.x));
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
