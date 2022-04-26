#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aN;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aT;
layout (location = 4) in vec3 aB;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

uniform vec3 LightPos;
uniform vec3 ViewPos;

out VS_OUT {
	vec3 FragPos;
	vec2 UV;
	vec3 LightPos;
	vec3 ViewPos;
} vs_out;

void main() {
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
	vec3 T = normalize(mat3(ModelMat) * aT);
	vec3 B = normalize(mat3(ModelMat) * aB);
	vec3 N = normalize(mat3(ModelMat) * aN);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.UV = aUV;
	vs_out.LightPos = TBN * LightPos;
	vs_out.ViewPos = TBN * ViewPos;
	vs_out.FragPos = TBN * vec3(ModelMat * vec4(aPos, 1.0f));
}