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
};

struct Sphere
{
    vec3 Origin;
    float Radius;
    int MaterialId;
};

struct Triangle
{
    vec3 Pos1;
    vec3 Pos2;
    vec3 Pos3;
    
    vec3 Normal1;
    vec3 Normal2;
    vec3 Normal3;
    int MaterialId;
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
};

uniform sampler2D Texture;

uniform vec2 WindowSize;
uniform int RayPerFrameCount;
uniform int MaxBouceCount;
uniform int FrameCount;
uniform float FocusStrength;
uniform float FocalLength;
uniform bool EnableEnvironment;

uniform vec3 CameraUp;
uniform vec3 CameraPosition;
uniform vec3 CameraForward;

#if defined(MaterialCount)
uniform Material Materials[MaterialCount];
#else
Material Materials[1] = { Material(vec3(.5), 1.0, vec3(0), 0.0, vec3(0), 0.0, vec3(0), 0.0) };
#endif

#if defined(SphereCount)
uniform Sphere Spheres[SphereCount];
#endif

#if defined(TriangleCount)
uniform Triangle Triangles[TriangleCount];
#endif

#if defined(AABBCount)
uniform AABB AABBs[AABBCount];
#endif

const vec3 SkyColorHorizon = vec3(1, 1, 1);
const vec3 SkyColorZenith = vec3(.5, .5, 1);
const vec3 GroundColor = vec3(.5, .5, .5);
const vec3 SunColor = vec3(1, 1, 1);
const vec3 SunLightDirection = normalize(vec3(0, -1, -1));
const float SunFocus = 50;
const float SunIntensity = 2.0;

vec3 GetEnvironmentLight(Ray ray)
{    
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.Direction.y), 0.35);
    vec3 skyGradient = mix(SkyColorHorizon, SkyColorZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.Direction, -SunLightDirection)), SunFocus) * SunIntensity;
    
    float groundToSkyT = smoothstep(-0.01, 0.0, ray.Direction.y);
    float sunMask = groundToSkyT >= 1.0 ? 1.0 : 0.0;
    return mix(GroundColor, skyGradient, groundToSkyT) + SunColor * sun * sunMask;
}

float RandomValue(inout uint seed)
{
    seed = seed * 747796405u + 2891336453u;
    uint result = ((seed >> ((seed >> 28) + 4u)) ^ seed) * 277803737u;
    result = (result >> 22) ^ result;
    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution(inout uint seed)
{
    float theta = 2.0 * 3.1415926 * RandomValue(seed);
    float rho = sqrt(-2.0 * log(RandomValue(seed)));
    return rho * cos(theta);
}

vec2 RandomVector2(inout uint seed)
{
    float angle = RandomValue(seed) * 2.0 * 3.1415926;
    return vec2(cos(angle), sin(angle)) * sqrt(RandomValue(seed));
}

vec3 RandomVector3(inout uint seed)
{
    float x = RandomValueNormalDistribution(seed);
    float y = RandomValueNormalDistribution(seed);
    float z = RandomValueNormalDistribution(seed);
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

    float depth = -b - sqrt(discriminant);
    if (depth < 0.0)
    {
        return false;
    }

    vec3 point = ray.Origin + ray.Direction * depth;
    manifold = CollisionManifold(
        depth,
        point,
        normalize(point - sphere.Origin),
        sphere.MaterialId);
    return true;
}

bool TriangleIntersection(in Ray ray, in Triangle tri, out CollisionManifold manifold)
{
    vec3 edge12 = tri.Pos2 - tri.Pos1;
    vec3 edge13 = tri.Pos3 - tri.Pos1;
    vec3 normal = cross(edge12, edge13);
    float det = -dot(ray.Direction, normal);
    if (det <= 0.0)
    {
        return false;
    }

    vec3 ao = ray.Origin - tri.Pos1;
    vec3 dao = cross(ao, ray.Direction);

    float invDet = 1.0 / det;
    
    float dst = dot(ao, normal) * invDet;
    float u = dot(edge13, dao) * invDet;
    float v = -dot(edge12, dao) * invDet;
    float w = 1.0 - u - v;
        
    if (dst < 0.0 || u < 0.0 || v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    manifold = CollisionManifold(
        dst,
        ray.Origin + ray.Direction * dst,
        normalize(tri.Normal1 * w + tri.Normal2 * u + tri.Normal3 * v),
        tri.MaterialId);
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
    if (tN > tF || tF < 0.0 || tN < 0.0)
    {
        return false;
    }

    vec3 normal = -sign(ray.Direction) * step(vec3(tN), t1);
    manifold = CollisionManifold(
        tN,
        ray.Origin + ray.Direction * tN,
        normal,
        aabb.MaterialId);
    return true;
}

bool FindIntersection(in Ray ray, out CollisionManifold manifold)
{
    manifold.Depth = 1.0 / 0.0;

#if defined(SphereCount)
    for (int i = 0; i < SphereCount; i++)
    {
        CollisionManifold current;
        if (SphereIntersection(ray, Spheres[i], current) && (current.Depth < manifold.Depth))
        {
            manifold = current;
        }
    }
#endif

#if defined(TriangleCount)
    for (int i = 0; i < TriangleCount; i++)
    {
        CollisionManifold current;
        if (TriangleIntersection(ray, Triangles[i], current) && (current.Depth < manifold.Depth))
        {
            manifold = current;
        }
    }
#endif

#if defined(AABBCount)
    for (int i = 0; i < AABBCount; i++)
    {
        CollisionManifold current;
        if (AABBIntersection(ray, AABBs[i], current) && (current.Depth < manifold.Depth))
        {
            manifold = current;
        }
    }
#endif

    return !isinf(manifold.Depth);
}

void CollisionReact(inout Ray ray, in CollisionManifold manifold, inout uint seed)
{
    vec3 diffuseDir = normalize(RandomVector3(seed) + manifold.Normal);
    vec3 reflectedDir = reflect(ray.Direction, manifold.Normal);
            
    Material material = Materials[manifold.MaterialId];

    float fresnelValue = 1.0 - pow(clamp(0.0, 1.0, dot(manifold.Normal, -ray.Direction)), material.FresnelStrength);
    bool isFresnelBounce = fresnelValue >= RandomValue(seed);
    bool isSpecularBounce = material.Metalness >= RandomValue(seed);
    bool isBounce = isFresnelBounce || isSpecularBounce;

    ray.Direction = mix(diffuseDir, reflectedDir, isBounce ? 1.0 - material.Roughness : 0.0);
    ray.Origin = manifold.Point;
    ray.IncomingLight += material.EmissionColor * material.EmissionStrength * ray.Color;
    ray.Color *= mix(
        material.AlbedoColor,
        clamp(vec3(0), vec3(1), material.MetalnessColor * vec3(isSpecularBounce) + material.FresnelColor * vec3(isFresnelBounce)),
        isBounce ? 1.0 : 0.0);
}

void SendRay(inout Ray ray, inout uint seed)
{
    for (int i = 0; i <= MaxBouceCount; i++)
    {
        CollisionManifold manifold;
        if (!FindIntersection(ray, manifold))
        {
            if (EnableEnvironment)
            {
                ray.IncomingLight += GetEnvironmentLight(ray) * ray.Color;
            }

            break;
        }

        CollisionReact(ray, manifold, seed);
    }
}

vec4 CollectColor(in Ray ray, inout uint seed)
{
    vec3 cameraRight = cross(CameraForward, CameraUp);
    vec3 resultLight = vec3(0);
    for (int i = 0; i < RayPerFrameCount; i++)
    {
        vec2 jitter = RandomVector2(seed) * FocusStrength;
        vec3 jitterOrigin = ray.Origin + jitter.x * cameraRight + jitter.y * CameraUp;

        vec3 focalPoint = ray.Origin + ray.Direction * FocalLength;
        Ray jitterRay = Ray(jitterOrigin, normalize(focalPoint - jitterOrigin), ray.Color, ray.IncomingLight);
        SendRay(jitterRay, seed);
        resultLight += jitterRay.IncomingLight;
    }
    
    return vec4(resultLight / float(RayPerFrameCount), 1);
}

void main()
{
    float aspectRatio = WindowSize.x / WindowSize.y;
    vec2 coord = vec2((gl_TexCoord[0].x - .5) * aspectRatio, gl_TexCoord[0].y - .5);
    vec3 right = cross(CameraForward, CameraUp);
    Ray ray = Ray(CameraPosition, normalize(CameraForward + right * coord.x + CameraUp * coord.y), vec3(1), vec3(0));

    uint seed = uint((WindowSize.x * gl_TexCoord[0].x + WindowSize.y * gl_TexCoord[0].y * gl_TexCoord[0].x) * 549856.0) + uint(FrameCount) * 5458u;

    vec4 newColor = CollectColor(ray, seed);
    vec4 oldColor = texture2D(Texture, gl_TexCoord[0].xy);
    float weight = 1.0 / (float(FrameCount) + 1.0);
    gl_FragColor = mix(oldColor, newColor, weight);
}