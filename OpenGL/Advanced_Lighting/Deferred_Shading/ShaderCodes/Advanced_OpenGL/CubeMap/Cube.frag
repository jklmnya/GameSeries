#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPosition;

uniform vec3 ViewPosition;
uniform samplerCube texture1;

void main() {
	vec3 I = normalize(FragPosition - ViewPosition);
	vec3 R = reflect(I, normalize(Normal));
	FragColor = vec4(texture(texture1, R).rgb, 1.0f);
}