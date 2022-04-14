#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aT;
layout (location = 4) in vec3 aB;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

uniform vec3 ViewPos;
uniform vec3 LightPos;

out VS_OUT {
	vec3 FragPos;
	vec3 LightPos;
	vec3 ViewPos;
	vec2 UV;
} vs_out;

void main() {
	
	mat3 normalMatrix = transpose(inverse(mat3(ModelMat)));
    vec3 T = normalize(normalMatrix * aT);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.FragPos = TBN * vec3(ModelMat * vec4(aPos, 1.0f));
	vs_out.UV = aUV;
	vs_out.LightPos = TBN * LightPos;
	vs_out.ViewPos = TBN * ViewPos;
	
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
}