vec3 Slerp(in vec3 a, in vec3 b, float t)
{
    float angle = acos(dot(a, b));
    return isnan(angle) || angle == 0 ? b : (sin((1 - t) * angle) * a + sin(t * angle) * b) / sin(angle);
}

vec3 Transform(in vec3 v, in mat4 matrix, in bool translate)
{
    return (matrix * vec4(v, translate ? 1 : 0)).xyz;
}