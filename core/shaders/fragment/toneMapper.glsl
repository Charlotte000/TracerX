#version 430 core

layout(binding=0) uniform sampler2D Accumulator;

uniform uint FrameCount;
uniform float Gamma;

in vec2 TexCoords;
layout(location=3) out vec4 ToneMapColor;

#include common/transforms.glsl

void main()
{
    vec4 pixel = texture(Accumulator, TexCoords) / FrameCount;
    ToneMapColor = ToneMap(pixel, Gamma);
}
