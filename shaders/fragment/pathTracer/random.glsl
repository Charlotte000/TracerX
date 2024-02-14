uint Seed = uint((gl_FragCoord.x + gl_FragCoord.y * gl_FragCoord.x / 1000) * 549856.0) + uint(FrameCount) * 5458u;

float RandomValue()
{
    Seed = Seed * 747796405u + 2891336453u;
    uint result = ((Seed >> ((Seed >> 28) + 4u)) ^ Seed) * 277803737u;
    result = (result >> 22) ^ result;
    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution()
{
    float theta = TWO_PI * RandomValue();
    float rho = sqrt(-2.0 * log(RandomValue()));
    return rho * cos(theta);
}

vec2 RandomVector2()
{
    float angle = RandomValue() * TWO_PI;
    return vec2(cos(angle), sin(angle)) * sqrt(RandomValue());
}

vec3 RandomVector3()
{
    float x = RandomValueNormalDistribution();
    float y = RandomValueNormalDistribution();
    float z = RandomValueNormalDistribution();
    return normalize(vec3(x, y, z));
}
