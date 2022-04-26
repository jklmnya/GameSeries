#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

void main() {
	TexCoords = aPos;
	gl_Position = (ProjectionMat * ViewMat * vec4(aPos, 1.0f)).xyww;
}