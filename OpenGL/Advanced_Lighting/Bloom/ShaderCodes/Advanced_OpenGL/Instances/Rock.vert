#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 ModelMat;

out vec2 UV;

uniform mat4 ViewMat;
uniform mat4 ProjectionMat;
// uniform mat4 ModelMat;

void main() {
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
	UV = aUV;
}