layout(binding=1) uniform sampler2D AccumulatorTexture;
layout(binding=2) uniform sampler2D EnvironmentTexture;
layout(binding=3) uniform samplerBuffer Vertices;
layout(binding=4) uniform samplerBuffer Normals;
layout(binding=5) uniform samplerBuffer UVCoords;
layout(binding=6) uniform isamplerBuffer Triangles;
layout(binding=7) uniform sampler2DArray Textures;
layout(binding=8) uniform samplerBuffer Meshes;
layout(binding=9) uniform samplerBuffer Materials;
layout(binding=10) uniform samplerBuffer BVH;

uniform float EnvironmentIntensity;
uniform int MaxBouceCount;
uniform int FrameCount;
uniform Cam Camera;


Triangle GetTriangle(int index)
{
    ivec4 data1 = texelFetch(Triangles, index * 4 + 0);
    ivec4 data2 = texelFetch(Triangles, index * 4 + 1);
    ivec4 data3 = texelFetch(Triangles, index * 4 + 2);
    ivec4 data4 = texelFetch(Triangles, index * 4 + 3);
    return Triangle(data1.xyz, data2.xyz, data3.xyz, data4.x);
}

Vertex GetVertex(ivec3 index, mat4 transform)
{
    vec3 position = texelFetch(Vertices, index.x).rgb;
    position = (transform * vec4(position, 1)).xyz;

    vec3 normal = texelFetch(Normals, index.y).rgb;
    normal = (transform * vec4(normal, 0)).xyz;

    vec2 uv = texelFetch(UVCoords, index.z).xy;

    return Vertex(position, normal, uv);
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
