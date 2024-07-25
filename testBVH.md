```c++
size_t prim_id = invalid_id;
float u, v;

// index: first_id(), prim_count()
// node: min, max, index

// == traverse_top_down ==
stack.push(bvh.get_root().index);
restart:
while (!stack.is_empty())
{
    auto top = stack.pop();
    while (top.prim_count() == 0)
    {
        auto& left  = nodes[top.first_id()];
        auto& right = nodes[top.first_id() + 1];

        // == inner_fn ==
        std::pair<Scalar, Scalar> intr_left, intr_right;
        intr_left  =  left.intersect_fast(ray, inv_dir, inv_org, octant); // AABBIntersection -> hit_left and (t0, t1)
        intr_right = right.intersect_fast(ray, inv_dir, inv_org, octant); // AABBIntersection -> hit_right and (t0, t1)

        bool hit_left = intr_left.first <= intr_left.second;
        bool hit_right = intr_right.first <= intr_right.second;
        bool should_swap = intr_left.first > intr_right.first;
        // == inner_fn ==

        if (hit_left)
        {
            auto near_index = left.index;
            if (hit_right)
            {
                auto far_index = right.index;
                if (should_swap)
                    std::swap(near_index, far_index);
                stack.push(far_index);
            }

            top = near_index;
        }
        else if (hit_right)
        {
            top = right.index;
        }
        else [[unlikely]]
        {
            goto restart;
        }
    }

    // == leaf_fn ==
    size_t begin = top.first_id();
    size_t end = top.first_id() + top.prim_count()
    for (size_t i = begin; i < end; ++i)
    {
        if (auto hit = precomputed_tris[i].intersect(ray)) // TriangleIntersection
        {
            prim_id = i;
            std::tie(u, v) = *hit;
        }
    }

    bool was_hit = prim_id != invalid_id;
    // == leaf_fn ==

    if constexpr (IsAnyHit) {
        if (was_hit) return;
    }
}

// == traverse_top_down ==

if (prim_id != invalid_id) {
    std::cout
        << "Intersection found\n"
        << "  primitive: " << prim_id << "\n"
        << "  distance: " << ray.tmax << "\n"
        << "  barycentric coords.: " << u << ", " << v << std::endl;
    return 0;
} else {
    std::cout << "No intersection found" << std::endl;
    return 1;
}
```

```glsl
bool FindIntersection_orig(in Ray ray, in bool firstHit, out CollisionManifold manifold)
{
    manifold.Depth = MaxRenderDistance;

    ivec2 stack[64];
    int stackPtr = 0;

    stack[stackPtr] = ivec2(1, 0);

    while (stackPtr >= 0)
    {
        ivec2 top = stack[stackPtr];
        stackPtr--;

        bool breakFlag = false;
        while (top.y == 0)
        {
            Node left = GetNode(top.x);
            ivec2 leftIndex = ivec2(left.FirstId, left.PrimCount);
            Node right = GetNode(top.x + 1);
            ivec2 rightIndex = ivec2(right.FirstId, right.PrimCount);

            float leftNear, leftFar, rightNear, rightFar;
            bool hitLeft = AABBIntersection(ray, left.BboxMin, left.BboxMax, leftNear, leftFar);
            bool hitRight = AABBIntersection(ray, right.BboxMin, right.BboxMax, rightNear, rightFar);
            bool shouldSwap = leftNear > rightNear;

            if (hitLeft)
            {
                ivec2 nearIndex = leftIndex;
                if (hitRight)
                {
                    ivec2 farIndex = rightIndex;
                    if (shouldSwap)
                    {
                        swap(nearIndex, farIndex);
                    }

                    stackPtr++;
                    stack[stackPtr] = farIndex;
                }

                top = nearIndex;
                continue;
            }

            if (hitRight)
            {
                top = rightIndex;
                continue;
            }

            breakFlag = true;
            break;
        }

        if (breakFlag)
        {
            continue;
        }

        int begin = top.x;
        int end = top.x + top.y;
        for (int i = begin; i < end; i++)
        {
            Triangle triangle = GetTriangle(i);
            Mesh mesh = GetMesh(triangle.MeshId);

            Vertex v1 = GetVertex(triangle.V1, mesh.Transform);
            Vertex v2 = GetVertex(triangle.V2, mesh.Transform);
            Vertex v3 = GetVertex(triangle.V3, mesh.Transform);

            CollisionManifold current;
            if (TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) && current.Depth < manifold.Depth && (!firstHit || current.Depth >= MinRenderDistance))
            {
                manifold = current;
            }
        }
    }

    return manifold.Depth < MaxRenderDistance;
}
```

```glsl
bool FindIntersection_FastBVH_Like(in Ray ray, in bool firstHit, out CollisionManifold manifold)
{
    manifold.Depth = MaxRenderDistance;

    ivec2 stack[64];
    int stackPtr = 0;

    stack[stackPtr] = ivec2(1, 0);

    while (stackPtr >= 0)
    {
        ivec2 top = stack[stackPtr];
        stackPtr--;

        if (top.y != 0)
        {
            int begin = top.x;
            int end = top.x + top.y;
            for (int i = begin; i < end; i++)
            {
                Triangle triangle = GetTriangle(i);
                Mesh mesh = GetMesh(triangle.MeshId);

                Vertex v1 = GetVertex(triangle.V1, mesh.Transform);
                Vertex v2 = GetVertex(triangle.V2, mesh.Transform);
                Vertex v3 = GetVertex(triangle.V3, mesh.Transform);

                CollisionManifold current;
                if (TriangleIntersection(ray, v1, v2, v3, mesh.MaterialId, current) && current.Depth < manifold.Depth && (!firstHit || current.Depth >= MinRenderDistance))
                {
                    manifold = current;
                }
            }
        }
        else
        {
            Node left = GetNode(top.x);
            ivec2 leftIndex = ivec2(left.FirstId, left.PrimCount);
            Node right = GetNode(top.x + 1);
            ivec2 rightIndex = ivec2(right.FirstId, right.PrimCount);

            float leftNear, leftFar, rightNear, rightFar;
            bool hitLeft = AABBIntersection(ray, left.BboxMin, left.BboxMax, leftNear, leftFar);
            bool hitRight = AABBIntersection(ray, right.BboxMin, right.BboxMax, rightNear, rightFar);
            bool shouldSwap = leftNear > rightNear;

            if (hitLeft && hitRight)
            {
                ivec2 nearIndex, farIndex;
                if (shouldSwap)
                {
                    nearIndex = rightIndex;
                    farIndex = leftIndex;
                }
                else
                {
                    nearIndex = leftIndex;
                    farIndex = rightIndex;
                }

                stackPtr++;
                stack[stackPtr] = farIndex;

                stackPtr++;
                stack[stackPtr] = nearIndex;
            }
            else if (hitLeft)
            {
                stackPtr++;
                stack[stackPtr] = leftIndex;
            }
            else if (hitRight)
            {
                stackPtr++;
                stack[stackPtr] = rightIndex;
            }
        }
    }

    return manifold.Depth < MaxRenderDistance;
}

```


## AABB
```c++
auto inv_dir = ray.template get_inv_dir<!IsRobust>();
auto inv_org = -inv_dir * ray.org;
auto octant = ray.get_octant();

left.intersect_fast(ray, inv_dir, inv_org, octant);
{
    auto tmin = get_min_bounds(octant) * inv_dir + inv_org;
    auto tmax = get_max_bounds(octant) * inv_dir + inv_org;
    return make_intersection_result(ray, tmin, tmax);
    {
        auto t0 = ray.tmin;
        auto t1 = ray.tmax;
        static_for<0, Dim>([&] (size_t i) {
            t0 = robust_max(tmin[i], t0);
            t1 = robust_min(tmax[i], t1);
        });
        return std::pair<T, T> { t0, t1 };
    }
}

```

```glsl
vec3 inv_dir = 1.0 / ray.Direction; // ToDo SafeInverse
vec3 inv_org = -inv_dir * ray.Origin;
bvec3 octant = bvec3(ray.Direction.x < 0, ray.Direction.y < 0, ray.Direction.z < 0);

bool AABBIntersection(in Ray ray, in vec3 boxMin, in vec3 boxMax, out float tNear, out float tFar)
{
    vec3 minBounds = vec3(octant.x ? boxMax.x : boxMin.x, octant.y ? boxMax.y : boxMin.y, octant.z ? boxMax.z : boxMin.z);
    vec3 maxBounds = vec3(octant.x ? boxMin.x : boxMax.x, octant.y ? boxMin.y : boxMax.y, octant.z ? boxMin.z : boxMax.z);

    vec3 tmin = minBounds * inv_dir + inv_org;
    vec3 tmax = maxBounds * inv_dir + inv_org;

    tNear = max(max(tmin.x, tmin.y), tmin.z);
    tFar = min(min(tmax.x, tmaax.y), tmax.z);
    return tNear <= tFar && tFar >= 0;
}

```

ajax (544_566 triangles) (position 0 0.5 -0.5)
|        | bvh     | my_bvh  | FastBVH |
|--------|---------|---------|---------|
| render | 45 ms   | 47 ms   | 26 ms   |
| nodes  | 611_331 | 611_331 | 36_6573 |



