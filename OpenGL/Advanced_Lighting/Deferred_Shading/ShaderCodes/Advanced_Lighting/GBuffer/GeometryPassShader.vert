#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 UV;
} vs_out;

void main() {
	vs_out.UV = aUV;
	vs_out.FragPos = vec3(ModelMat * vec4(aPos, 1.0f));

	mat3 NormalMat = transpose(inverse(mat3(ModelMat)));
	vs_out.Normal = NormalMat * aNormal;

	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
}