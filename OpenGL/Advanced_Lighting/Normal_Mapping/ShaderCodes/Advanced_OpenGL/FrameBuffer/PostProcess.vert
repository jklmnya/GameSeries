#version 330 core

layout(location = 0) in vec2 aNDC;
layout(location = 1) in vec2 aUV;

out vec2 UV;

void main() {
	gl_Position = vec4(aNDC.x, aNDC.y, 0.0f, 1.0f);
	UV = aUV;
}