#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main() {
	FragColor = texture(texture_diffuse1, UV);
}