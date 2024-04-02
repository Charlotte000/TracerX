struct Ray
{
    vec3 Origin;
    vec3 Direction;
    vec3 InvDirection;
    vec3 Color;
    vec3 IncomingLight;
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
    int EmissionTexureId;
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
    int MeshId;
};

struct Mesh
{
    mat4 Transform;
    int MaterialId;
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
