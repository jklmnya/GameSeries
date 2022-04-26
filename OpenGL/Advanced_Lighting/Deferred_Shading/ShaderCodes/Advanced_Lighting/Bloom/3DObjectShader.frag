#version 330 core
#define LIGHT_NUM 4

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
	vec2 UV;
	vec3 Normal;
	vec3 FragPos;
} fs_in;

struct Light {
	vec3 Pos;
	vec3 Color;
};

uniform sampler2D diffuseMap;
uniform Light Lights[LIGHT_NUM];
uniform vec3 ViewPos;

void main() {
	vec3 BaseColor = texture(diffuseMap, fs_in.UV).rgb;
	vec3 Res;
	vec3 Normal = normalize(fs_in.Normal);
	for (int i = 0;i<LIGHT_NUM;++i) {
		vec3 LightDir = normalize(Lights[i].Pos - fs_in.FragPos);
		vec3 Diffuse = BaseColor * Lights[i].Color * max(0.0f, dot(Normal, LightDir));
		float Distance = length(fs_in.FragPos - Lights[i].Pos);
		Res += Diffuse / (Distance * Distance);
	}
	FragColor = vec4(Res, 1.0f);
	float BrightNess = dot(Res, vec3(0.2126, 0.7152, 0.0722));
	if (BrightNess > 1.0f)
		BrightColor = vec4(Res, 1.0f);
	else
		BrightColor = vec4(vec3(0.0f), 1.0f);
}