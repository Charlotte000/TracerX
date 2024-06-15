void swap(inout float a, inout float b)
{
    float tmp = a;
    a = b;
    b = tmp;
}

void swap(inout int a, inout int b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

bool TriangleIntersection(in Ray ray, in Vertex v1, in Vertex v2, in Vertex v3, in int materialId, out CollisionManifold manifold)
{
    vec3 edge12 = v2.Position - v1.Position;
    vec3 edge13 = v3.Position - v1.Position;
    vec3 normal = cross(edge12, edge13);
    float det = -dot(ray.Direction, normal);

    if (abs(det) <= length(normal) * 0.01)
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

    if (dst <= 0.001 || u < 0.0 || v < 0.0 || w < 0.0)
    {
        return false;
    }

    vec2 edgeUV12 = v2.TextureCoordinate - v1.TextureCoordinate;
    vec2 edgeUV13 = v3.TextureCoordinate - v1.TextureCoordinate;
    float invDetUV = 1.0 / (edgeUV12.x * edgeUV13.y - edgeUV12.y * edgeUV13.x);

    manifold = CollisionManifold(
        dst,
        ray.Origin + ray.Direction * dst,
        v1.TextureCoordinate * w + v2.TextureCoordinate * u + v3.TextureCoordinate * v,
        normalize(v1.Normal * w + v2.Normal * u + v3.Normal * v),
        normalize((edge12 * edgeUV13.y - edge13 * edgeUV12.y) * invDetUV),
        normalize((edge13 * edgeUV12.x - edge12 * edgeUV13.x) * invDetUV),
        materialId,
        det >= 0);
    return true;
}

bool AABBIntersection(in Ray ray, in vec3 boxMin, in vec3 boxMax, out float tNear, out float tFar)
{
    vec3 tMin = (boxMin - ray.Origin) * ray.InvDirection;
    vec3 tMax = (boxMax - ray.Origin) * ray.InvDirection;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    tNear = max(max(t1.x, t1.y), t1.z);
    tFar = min(min(t2.x, t2.y), t2.z);
    return tNear <= tFar && tFar >= 0;
}

bool MeshIntersection(in Ray ray, in Mesh mesh, in bool firstHit, out CollisionManifold manifold)
{
    vec3 rayOrigin = ray.Origin;
    ray.Origin = Transform(ray.Origin, mesh.TransformInv, true);
    ray.Direction = normalize(Transform(ray.Direction, mesh.TransformInv, false));
    ray.InvDirection = 1 / ray.Direction;

    float localMinRenderDistance = length(Transform(ray.Direction * MinRenderDistance, mesh.TransformInv, false));
    float localMaxRenderDistance = length(Transform(ray.Direction * MaxRenderDistance, mesh.TransformInv, false));
    manifold.Depth = localMaxRenderDistance;

    float bbhits[4];

    vec2 todo[64];
    int stackptr = 0;

    todo[stackptr] = vec2(mesh.NodeOffset, -1);

    while (stackptr >= 0)
    {
        int ni = int(todo[stackptr].x);
        float near = todo[stackptr].y;
        stackptr--;

        Node node = GetNode(ni);

        if (near > manifold.Depth) continue;

        if (node.RightOffset == 0)
        {
            for (int o = 0; o < node.PrimitiveCount; ++o)
            {
                Triangle triangle = GetTriangle(node.Start + o + mesh.TriangleOffset);

                Vertex v1 = GetVertex(triangle.V1);
                Vertex v2 = GetVertex(triangle.V2);
                Vertex v3 = GetVertex(triangle.V3);

                CollisionManifold current;
                if (TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) && current.Depth < manifold.Depth && (!firstHit || current.Depth >= localMinRenderDistance))
                {
                    manifold = current;
                }
            }
        }
        else
        {
            Node c0 = GetNode(ni + 1);
            Node c1 = GetNode(ni + node.RightOffset);

            bool hitc0 = AABBIntersection(ray, c0.BboxMin, c0.BboxMax, bbhits[0], bbhits[1]);
            bool hitc1 = AABBIntersection(ray, c1.BboxMin, c1.BboxMax, bbhits[2], bbhits[3]);

            if (hitc0 && hitc1)
            {
                int closer = ni + 1;
                int other = ni + node.RightOffset;

                if (bbhits[2] < bbhits[0])
                {
                    swap(bbhits[0], bbhits[2]);
                    swap(bbhits[1], bbhits[3]);
                    swap(closer, other);
                }

                todo[++stackptr] = vec2(other, bbhits[2]);
                todo[++stackptr] = vec2(closer, bbhits[0]);
            }
            else if (hitc0)
            {
                todo[++stackptr] = vec2(ni + 1, bbhits[0]);
            }
            else if (hitc1)
            {
                todo[++stackptr] = vec2(ni + node.RightOffset, bbhits[2]);
            }
        }
    }

    if (manifold.Depth < localMaxRenderDistance)
    {
        manifold.Point = Transform(manifold.Point, mesh.Transform, true);
        manifold.Depth = length(manifold.Point - rayOrigin);
        manifold.Normal = normalize(Transform(manifold.Normal, mesh.Transform, false));
        manifold.Tangent = normalize(Transform(manifold.Tangent, mesh.Transform, false));
        manifold.Bitangent = normalize(Transform(manifold.Bitangent, mesh.Transform, false));
        return true;
    }

    return false;
}

bool FindIntersection(in Ray ray, in bool firstHit, out CollisionManifold manifold)
{
    manifold.Depth = MaxRenderDistance;

    int meshCount = GetMeshCount();
    for (int meshId = 0; meshId < meshCount; meshId++)
    {
        Mesh mesh = GetMesh(meshId);

        CollisionManifold current;
        if (MeshIntersection(ray, mesh, firstHit, current) && current.Depth < manifold.Depth)
        {
            manifold = current;
        }
    }

    return manifold.Depth < MaxRenderDistance;
}
