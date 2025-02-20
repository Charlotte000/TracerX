```glsl
struct ScatterSampleRec
{
    vec3 L;
    vec3 f;
    float pdf;
};
```

```glsl
float Luminance(vec3 c)
{
    return 0.212671 * c.x + 0.715160 * c.y + 0.072169 * c.z;
}
```

```glsl
vec4 EvalEnvMap(Ray r)
{
    float theta = acos(clamp(r.direction.y, -1.0, 1.0));
    vec2 uv = vec2((PI + atan(r.direction.z, r.direction.x)) * INV_TWO_PI, theta * INV_PI) + vec2(envMapRot, 0.0);
    
    vec3 color = texture(envMapTex, uv).rgb;
    float pdf = Luminance(color) / envMapTotalSum;

    return vec4(color, (pdf * envMapRes.x * envMapRes.y) / (TWO_PI * PI * sin(theta)));
}
```

```glsl
float PowerHeuristic(float a, float b)
{
    float t = a * a;
    return t / (b * b + t);
}
```

```glsl
vec4 PathTrace(Ray r)
{
    vec3 radiance = vec3(0.0);
    vec3 throughput = vec3(1.0);
    State state;
    LightSampleRec lightSample;
    ScatterSampleRec scatterSample;

    // FIXME: alpha from material opacity/medium density
    float alpha = 1.0;

    // For medium tracking
    bool inMedium = false;
    bool mediumSampled = false;
    bool surfaceScatter = false;

    for (state.depth = 0;; state.depth++)
    {
// <--------------------------------------------------------------->
//         bool hit = ClosestHit(r, state, lightSample);
// 
//         if (!hit)
//         {
// #if defined(OPT_BACKGROUND) || defined(OPT_TRANSPARENT_BACKGROUND)
//             if (state.depth == 0)
//                 alpha = 0.0;
// #endif
// 
//             vec4 envMapColPdf = EvalEnvMap(r);
// 
//             float misWeight = 1.0;
// 
//             // Gather radiance from envmap and use scatterSample.pdf from previous bounce for MIS
//             if (state.depth > 0)
//                 misWeight = PowerHeuristic(scatterSample.pdf, envMapColPdf.w);
// 
// #if defined(OPT_MEDIUM) && !defined(OPT_VOL_MIS)
//             if(!surfaceScatter)
//                 misWeight = 1.0f;
// #endif
// 
//             if(misWeight > 0)
//                 radiance += misWeight * envMapColPdf.rgb * throughput * envMapIntensity;
//              break;
//         }
// 
// <------------------------------------------------------------- -->

        GetMaterial(state, r);

        // Gather radiance from emissive objects. Emission from meshes is not importance sampled
        radiance += state.mat.emission * throughput;

        // Gather radiance from light and use scatterSample.pdf from previous bounce for MIS
        if (state.isEmitter)
        {
            float misWeight = 1.0;

            if (state.depth > 0)
                misWeight = PowerHeuristic(scatterSample.pdf, lightSample.pdf);

#if defined(OPT_MEDIUM) && !defined(OPT_VOL_MIS)
            if(!surfaceScatter)
                misWeight = 1.0f;
#endif

            radiance += misWeight * lightSample.emission * throughput;

            break;
        }

        // Stop tracing ray if maximum depth was reached
        if(state.depth == maxDepth)
            break;

#ifdef OPT_MEDIUM

        mediumSampled = false;
        surfaceScatter = false;

        // Handle absorption/emission/scattering from medium
        // TODO: Handle light sources placed inside medium
        if(inMedium)
        {
            if(state.medium.type == MEDIUM_ABSORB)
            {
                throughput *= exp(-(1.0 - state.medium.color) * state.hitDist * state.medium.density);
            }
            else if(state.medium.type == MEDIUM_EMISSIVE)
            {
                radiance += state.medium.color * state.hitDist * state.medium.density * throughput;
            }
            else
            {
                // Sample a distance in the medium
                float scatterDist = min(-log(rand()) / state.medium.density, state.hitDist);
                mediumSampled = scatterDist < state.hitDist;

                if (mediumSampled)
                {
                    throughput *= state.medium.color;

                    // Move ray origin to scattering position
                    r.origin += r.direction * scatterDist;
                    state.fhp = r.origin;

                    // Transmittance Evaluation
                    radiance += DirectLight(r, state, false) * throughput;

                    // Pick a new direction based on the phase function
                    vec3 scatterDir = SampleHG(-r.direction, state.medium.anisotropy, rand(), rand());
                    scatterSample.pdf = PhaseHG(dot(-r.direction, scatterDir), state.medium.anisotropy);
                    r.direction = scatterDir;
                }
            }
        }

        // If medium was not sampled then proceed with surface BSDF evaluation
        if (!mediumSampled)
        {
#endif
            // Ignore intersection and continue ray based on alpha test
            if ((state.mat.alphaMode == ALPHA_MODE_MASK && state.mat.opacity < state.mat.alphaCutoff) ||
                (state.mat.alphaMode == ALPHA_MODE_BLEND && rand() > state.mat.opacity))
            {
                scatterSample.L = r.direction;
                state.depth--;
            }
            else
            {
                surfaceScatter = true;

                // Next event estimation
                radiance += DirectLight(r, state, true) * throughput;

                // Sample BSDF for color and outgoing direction
                scatterSample.f = DisneySample(state, -r.direction, state.ffnormal, scatterSample.L, scatterSample.pdf);
                if (scatterSample.pdf > 0.0)
                    throughput *= scatterSample.f / scatterSample.pdf;
                else
                    break;
            }

            // Move ray origin to hit point and set direction for next bounce
            r.direction = scatterSample.L;
            r.origin = state.fhp + r.direction * EPS;

#ifdef OPT_MEDIUM

            // Note: Nesting of volumes isn't supported due to lack of a volume stack for performance reasons
            // Ray is in medium only if it is entering a surface containing a medium
            if (dot(r.direction, state.normal) < 0 && state.mat.medium.type != MEDIUM_NONE)
            {
                inMedium = true;
                // Get medium params from the intersected object
                state.medium = state.mat.medium;
            }
            // FIXME: Objects clipping or inside a medium were shaded incorrectly as inMedium would be set to false.
            // This hack works for now but needs some rethinking
            else if(state.mat.medium.type != MEDIUM_NONE)
                inMedium = false;
        }
#endif

// <--------------------------------------------------------------->
// #ifdef OPT_RR
//         // Russian roulette
//         if (state.depth >= OPT_RR_DEPTH)
//         {
//             float q = min(max(throughput.x, max(throughput.y, throughput.z)) + 0.001, 0.95);
//             if (rand() > q)
//                 break;
//             throughput /= q;
//         }
// #endif
// <--------------------------------------------------------------->

    }

    return vec4(radiance, alpha);
}
```