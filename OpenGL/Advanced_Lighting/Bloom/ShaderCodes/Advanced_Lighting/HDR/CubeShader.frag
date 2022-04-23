#version 330 core

#define LIGHT_NUM 4

out vec4 FragColor;

in VS_OUT {
	vec2 UV;
	vec3 Normal;
	vec3 FragPos;
} fs_in;

struct Light {
	vec3 Pos;
	vec3 Color;
};

uniform sampler2D DiffuseMap;
uniform vec3 ViewPos;
uniform Light Lights[LIGHT_NUM];

void main() {
	vec3 BaseColor = texture(DiffuseMap, fs_in.UV).rgb;
	vec3 Result = vec3(0.0f);
	vec3 Normal = normalize(fs_in.Normal);
	for (int i = 0;i<LIGHT_NUM;++i) {
		vec3 LightDir = normalize(Lights[i].Pos - fs_in.FragPos);
		vec3 Diffuse = Lights[i].Color * BaseColor * max(0.0f, dot(Normal, LightDir));
		float Distance = length(fs_in.FragPos - Lights[i].Pos);
		Result += Diffuse / (Distance * Distance);
	}
	FragColor = vec4(Result, 1.0f);
}