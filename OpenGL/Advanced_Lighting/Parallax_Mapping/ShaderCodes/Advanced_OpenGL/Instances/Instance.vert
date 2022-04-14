#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aOffset;

out vec3 Color;

void main() {
	vec2 Pos = aPos * (gl_InstanceID / 100.f);
	Color = aColor;
	gl_Position = vec4(Pos + aOffset, 0.0f, 1.0f);
}