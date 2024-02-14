#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

layout(binding=0) uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, TexCoords);
}
