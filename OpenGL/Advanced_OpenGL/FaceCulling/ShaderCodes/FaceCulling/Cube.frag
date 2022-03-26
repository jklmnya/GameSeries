#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D cube_texture;

void main() {
	FragColor = texture(cube_texture, UV);
}