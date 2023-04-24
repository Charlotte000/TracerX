struct Ray
{
    vec3 Origin;
    vec3 Direction;
    vec3 Color;
    vec3 IncomingLight;
};

struct Material
{
    vec3 AlbedoColor;
    float Roughness;
    vec3 MetalnessColor;
    float Metalness;
    vec3 EmissionColor;
    float EmissionStrength;
    vec3 FresnelColor;
    float FresnelStrength;
    bool IsTransparent;
    float RefractionFactor;
};

struct Sphere
{
    vec3 Origin;
    float Radius;
    int MaterialId;
};

struct Vertex
{
    vec3 Position;
    vec3 Normal;
};

struct Mesh
{
    int IndicesStart;
    int IndicesLength;
    int MaterialId;
    vec3 BoxMin;
    vec3 BoxMax;
};

struct AABB
{
    vec3 Origin;
    vec3 Size;
    int MaterialId;
};

struct CollisionManifold
{
    float Depth;
    vec3 Point;
    vec3 Normal;
    int MaterialId;
    bool isFrontFace;
};

uniform sampler2D Texture;

uniform vec2 WindowSize;
uniform int SampleCount;
uniform int MaxBouceCount;
uniform int FrameCount;
uniform float FocusStrength;
uniform float FocalLength;

uniform vec3 CameraUp;
uniform vec3 CameraPosition;
uniform vec3 CameraForward;

#ifdef MaterialCount
uniform Material Materials[MaterialCount];
#endif

#ifdef SphereCount
uniform Sphere Spheres[SphereCount];
#endif

#ifdef VertexCount
uniform Vertex Vertices[VertexCount];
#endif

#ifdef IndexCount
uniform int Indices[IndexCount];
#endif

#ifdef MeshCount
uniform Mesh Meshes[MeshCount];
#endif

#ifdef AABBCount
uniform AABB AABBs[AABBCount];
#endif

const float SmallNumber = 0.001;
const vec3 CameraRight = cross(CameraForward, CameraUp);
uint Seed = uint((WindowSize.x * gl_TexCoord[0].x + WindowSize.y * gl_TexCoord[0].y * gl_TexCoord[0].x) * 549856.0) + uint(FrameCount) * 5458u;

uniform struct
{
    vec3 SkyColorHorizon;
    vec3 SkyColorZenith;
    vec3 GroundColor;
    vec3 SunColor;
    vec3 SunDirection;
    float SunFocus;
    float SunIntensity;
    bool Enabled;
} Environment;

vec3 GetEnvironmentLight(Ray ray)
{    
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.Direction.y), 0.35);
    vec3 skyGradient = mix(Environment.SkyColorHorizon, Environment.SkyColorZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.Direction, -Environment.SunDirection)), Environment.SunFocus) * Environment.SunIntensity;
    
    float groundToSkyT = smoothstep(-0.01, 0.0, ray.Direction.y);
    float sunMask = groundToSkyT >= 1.0 ? 1.0 : 0.0;
    return mix(Environment.GroundColor, skyGradient, groundToSkyT) + Environment.SunColor * sun * sunMask;
}

float RandomValue()
{
    Seed = Seed * 747796405u + 2891336453u;
    uint result = ((Seed >> ((Seed >> 28) + 4u)) ^ Seed) * 277803737u;
    result = (result >> 22) ^ result;
    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution()
{
    float theta = 2.0 * 3.1415926 * RandomValue();
    float rho = sqrt(-2.0 * log(RandomValue()));
    return rho * cos(theta);
}

vec2 RandomVector2()
{
    float angle = RandomValue() * 2.0 * 3.1415926;
    return vec2(cos(angle), sin(angle)) * sqrt(RandomValue());
}

vec3 RandomVector3()
{
    float x = RandomValueNormalDistribution();
    float y = RandomValueNormalDistribution();
    float z = RandomValueNormalDistribution();
    return normalize(vec3(x, y, z));
}

bool SphereIntersection(in Ray ray, in Sphere sphere, out CollisionManifold manifold)
{
    vec3 offset = ray.Origin - sphere.Origin;
    float b = dot(offset, ray.Direction);
    float c = dot(offset, offset) - sphere.Radius * sphere.Radius;
    
    float discriminant = b * b - c;
    if (discriminant < 0.0)
    {
        return false;
    }

    discriminant = sqrt(discriminant);

    float tN = -b - discriminant;
    float tF = -b + discriminant;

    if (tN < SmallNumber && tF < SmallNumber)
    {
        return false;
    }

    bool isFrontFace = tN > SmallNumber;
    float depth = isFrontFace ? tN : tF;
    vec3 point = ray.Origin + ray.Direction * depth;
    manifold = CollisionManifold(
        depth,
        point,
        normalize(point - sphere.Origin) * (isFrontFace ? 1.0 : -1.0),
        sphere.MaterialId,
        isFrontFace);
    return true;
}

bool TriangleIntersection(in Ray ray, in Vertex v1, in Vertex v2, in Vertex v3, in int materialId, out CollisionManifold manifold)
{
    vec3 edge12 = v2.Position - v1.Position;
    vec3 edge13 = v3.Position - v1.Position;
    vec3 normal = cross(edge12, edge13);
    float det = -dot(ray.Direction, normal);

    bool isFrontFace = det > SmallNumber;
    if (abs(det) < SmallNumber)
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
        
    if (dst < SmallNumber || u < 0.0 || v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    manifold = CollisionManifold(
        dst,
        ray.Origin + ray.Direction * dst,
        normalize(v1.Normal * w + v2.Normal * u + v3.Normal * v) * (isFrontFace ? 1.0 : -1.0),
        materialId,
        isFrontFace);
    return true;
}

bool AABBIntersection(in Ray ray, in AABB aabb, out CollisionManifold manifold)
{
    vec3 m = 1.0 / ray.Direction;
    vec3 n = m * (ray.Origin - aabb.Origin);
    vec3 k = abs(m) * aabb.Size / 2.0;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);
    if (tN > tF || tF < SmallNumber)
    {
        return false;
    }

    bool isFrontFace = tN > SmallNumber;

    vec3 normal = -sign(ray.Direction) * (isFrontFace ?  step(vec3(tN), t1) : step(t2, vec3(tF)));
    float depth = isFrontFace? tN : tF;
    manifold = CollisionManifold(
        depth,
        ray.Origin + ray.Direction * depth,
        normal,
        aabb.MaterialId,
        isFrontFace);
    return true;
}

bool AABBIntersection(in Ray ray, in vec3 invRayDir, in vec3 boxMin, in vec3 boxMax)
{
    vec3 tMin = (boxMin - ray.Origin) * invRayDir;
    vec3 tMax = (boxMax - ray.Origin) * invRayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);
    return tN <= tF && tF >= SmallNumber;
}

bool FindIntersection(in Ray ray, out CollisionManifold manifold)
{
    manifold.Depth = 1.0 / 0.0;

    vec3 invRayDir = 1.0 / ray.Direction;

#ifdef SphereCount
    for (int i = 0; i < SphereCount; i++)
    {
        CollisionManifold current;
        vec3 radius = vec3(Spheres[i].Radius);
        vec3 boxMin = Spheres[i].Origin - radius;
        vec3 boxMax = Spheres[i].Origin + radius;
        if (AABBIntersection(ray, invRayDir, boxMin, boxMax) &&
            SphereIntersection(ray, Spheres[i], current) &&
            current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }
#endif

#ifdef MeshCount
    for (int i = 0; i < MeshCount; i++)
    {
        Mesh mesh = Meshes[i];

        for (int index = mesh.IndicesStart; index < mesh.IndicesStart + mesh.IndicesLength; index += 3)
        {
            CollisionManifold current;
            Vertex v1 = Vertices[Indices[index]];
            Vertex v2 = Vertices[Indices[index + 1]];
            Vertex v3 = Vertices[Indices[index + 2]];
            if (AABBIntersection(ray, invRayDir, mesh.BoxMin, mesh.BoxMax) &&
                TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) &&
                current.Depth < manifold.Depth)
            {
                manifold = current;
            }
        }
    }
#endif

#ifdef AABBCount
    for (int i = 0; i < AABBCount; i++)
    {
        CollisionManifold current;
        if (AABBIntersection(ray, AABBs[i], current) && current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }
#endif

    return !isinf(manifold.Depth);
}

void CollisionReact(inout Ray ray, in CollisionManifold manifold)
{
    Material material = Materials[manifold.MaterialId];

    vec3 reactedDir = material.IsTransparent ?
        refract(ray.Direction, manifold.Normal, manifold.isFrontFace ? material.RefractionFactor : 1.0 / material.RefractionFactor) :
        normalize(RandomVector3() + manifold.Normal);
    vec3 reflectedDir = reflect(ray.Direction, manifold.Normal);

    float fresnelValue = material.FresnelStrength > 0.0 ? 1.0 - pow(dot(manifold.Normal, -ray.Direction), material.FresnelStrength) : 0.0;
    float fresnelMask = fresnelValue >= RandomValue() ? 0.0 : 1.0;

    ray.Origin = manifold.Point;
    ray.Direction = normalize(mix(reflectedDir, reactedDir, material.Roughness * fresnelMask));

    ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
    ray.Color *= material.FresnelStrength > 0.0 && fresnelMask < 1.0 ? 
        material.FresnelColor : 
        material.Metalness >= RandomValue() ? 
        material.MetalnessColor : 
        material.AlbedoColor;
}

vec3 SendRay(in Ray ray)
{
    for (int i = 0; i <= MaxBouceCount; i++)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, manifold))
        {
            if (Environment.Enabled)
            {
                ray.IncomingLight += GetEnvironmentLight(ray) * ray.Color;
            }

            break;
        }

        CollisionReact(ray, manifold);
    }

    return ray.IncomingLight;
}

vec3 SendRayFlow(in Ray ray)
{
    vec3 focalPoint = ray.Origin + ray.Direction * FocalLength;
    
    vec3 resultLight = vec3(0);
    for (int i = 0; i < SampleCount; i++)
    {
        vec2 jitter = RandomVector2() * FocusStrength;
        vec3 jitterOrigin = ray.Origin + jitter.x * CameraRight + jitter.y * CameraUp;
        vec3 jitterDirection = normalize(focalPoint - jitterOrigin);
        resultLight += SendRay(Ray(jitterOrigin, jitterDirection, ray.Color, ray.IncomingLight));
    }
    
    return resultLight / float(SampleCount);
}

void main()
{
    float aspectRatio = WindowSize.x / WindowSize.y;
    vec2 coord = vec2((gl_TexCoord[0].x - .5) * aspectRatio, gl_TexCoord[0].y - .5);
    Ray ray = Ray(CameraPosition, normalize(CameraForward + CameraRight * coord.x + CameraUp * coord.y), vec3(1), vec3(0));

    vec3 newColor = SendRayFlow(ray);

    // Tone mapping
    const float white = 4.0;
    const float exposure = 16.0;
    newColor *= white * exposure;
    newColor = (newColor * (1.0 + newColor / white / white)) / (1.0 + newColor);

    // Progressive rendering mixing
    vec3 oldColor = texture2D(Texture, gl_TexCoord[0].xy).rgb;
    float weight = 1.0 / float(FrameCount);
    gl_FragColor = vec4(mix(oldColor, newColor, weight), 1.0);
}