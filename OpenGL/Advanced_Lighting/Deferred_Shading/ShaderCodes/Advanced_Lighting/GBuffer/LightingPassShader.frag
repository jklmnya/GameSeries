#version 330 core
#define LIGHT_NUM 32

out vec4 FragColor;
in vec2 UV;

struct Light {
	vec3 LightPos;
	vec3 LightColor;
};

uniform sampler2D FragPosMap;
uniform sampler2D NormalMap;
uniform sampler2D AlbedoSpecMap;

uniform Light Lights[LIGHT_NUM];

uniform vec3 ViewPos;

const float Kl = 0.7f;
const float Kq = 1.8f;

void main() {
	vec3 FragPos = texture(FragPosMap, UV).rgb;
	vec3 Normal = texture(NormalMap, UV).rgb;
	vec3 BaseColor = texture(AlbedoSpecMap, UV).rgb;
	float Spec = texture(AlbedoSpecMap, UV).a;

	vec3 ViewDir = normalize(ViewPos - FragPos);
	vec3 Color = 0.1f * BaseColor;

	for (int i = 0;i<LIGHT_NUM;++i) {
		vec3 LightDir = normalize(Lights[i].LightPos - FragPos);
		vec3 Diffuse = Lights[i].LightColor * BaseColor * max(0.0f, dot(Normal, LightDir));

		vec3 HalfVec = normalize(LightDir + ViewDir);
		vec3 Specular = Lights[i].LightColor * Spec * pow(max(0.0f, dot(Normal, HalfVec)), 16.0f);

		float Distance = length(FragPos - Lights[i].LightPos);
		float Attenuation = 1.0f / (1.0f + Kl * Distance + Kq * Distance * Distance);

		Color += Attenuation * (Diffuse + Specular);
	}
	FragColor = vec4(Color, 1.0f);
}