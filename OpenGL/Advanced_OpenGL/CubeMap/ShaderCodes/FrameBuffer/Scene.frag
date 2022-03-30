#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
	vec3 fColor = texture(texture1, UV).rgb;
	float Average = 0.2126 * fColor.r + 0.7152 * fColor.g + 0.0722 * fColor.b;
	FragColor = vec4(Average, Average, Average, 1.0f);
}