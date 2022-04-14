#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;

out vec2 UV;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

void main() {
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
	UV = aUV;
}