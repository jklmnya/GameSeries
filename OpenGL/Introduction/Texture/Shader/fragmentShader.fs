#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 textureCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float texture2Alpha;

void main()
{
    FragColor = mix(texture(texture1, textureCoord), texture(texture2, vec2(1 - textureCoord.x, textureCoord.y)), texture2Alpha);
}