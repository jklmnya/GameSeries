#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube texture_cube;

void main() {
	FragColor = texture(texture_cube, TexCoords);
}
