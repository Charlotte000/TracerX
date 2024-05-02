#version 430 core

layout(binding=0) uniform sampler2D Accumulator;

uniform int FrameCount;
uniform float Gamma;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
    vec4 pixel = texture(Accumulator, TexCoords) / FrameCount;
    vec3 color = pixel.rgb;
    
    // Reinhard tone mapping
    vec3 mapped = color / (color + vec3(1));

    // Gamma correction
    mapped = pow(mapped, vec3(1 / Gamma));

    FragColor = vec4(mapped, pixel.a);
}
