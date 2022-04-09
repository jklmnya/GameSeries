#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 Normal;
out vec2 UV;
out vec3 FragPos;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

void main() {
	Normal = aNormal;
	UV = aUV;
	FragPos = aPos;
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
}