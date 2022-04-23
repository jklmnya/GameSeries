#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 UV;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

void main() {
	UV = aPos;
	gl_Position = ProjectionMat * mat4(mat3(ViewMat)) * ModelMat * vec4(aPos, 1.0f);
}