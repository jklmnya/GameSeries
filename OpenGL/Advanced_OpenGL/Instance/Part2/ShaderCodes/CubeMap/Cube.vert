#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPosition;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

void main() {
	Normal = mat3(transpose(inverse(ModelMat))) * aNormal;
	FragPosition = vec3(ModelMat * vec4(aPos, 1.0f));
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
}