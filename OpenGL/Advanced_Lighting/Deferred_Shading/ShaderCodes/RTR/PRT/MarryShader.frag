#version 330 core

in highp vec3 iFragColor;
out vec4 FragColor;

void main() {
	FragColor = vec4(iFragColor, 1.0f);
}