#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D texture1;

float near	= 0.1f;
float far	= 100.f;

void main() {
	float z = gl_FragCoord.z * 2 - 1;
	z = (2.0f * near * far) / (far + near - z * (far - near));
	FragColor = vec4(vec3(z / far), 1.0f);
}