#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D grass_texture;

void main() {
	FragColor = texture(grass_texture, UV);
}