#version 330 core
out vec4 FragColor;

in vec2 textureCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, textureCoord), texture(texture2, textureCoord), 0.2f);
}