#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

out vec2 UV;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

void main() {
	UV = aUV;
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
}