#version 330 core

out vec4 FragColor;

in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 ViewPos;

struct DirectLight {
	vec3 Direction;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

struct PointLight {
	vec3 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Kc;
	float Kl;
	float Kq;
};

#define POINT_LIGHT_NUM 2
uniform PointLight pointLights[POINT_LIGHT_NUM];
uniform DirectLight directLight;

vec3 CalcPointLight(PointLight PL, vec3 Normal, vec3 ViewDir);
vec3 CalcDirectLight(DirectLight DL, vec3 Normal, vec3 ViewDir);

void main() {
	vec3 Result = vec3(0.0f);
	vec3 NNormal = normalize(Normal);
	vec3 ViewDir = normalize(ViewPos - FragPos);
	Result += CalcPointLight(pointLights[0], NNormal, ViewDir);
	Result += CalcPointLight(pointLights[1], NNormal, ViewDir);
	Result += CalcDirectLight(directLight, NNormal, ViewDir);
	FragColor = vec4(Result, 1.0f);
}

vec3 CalcPointLight(PointLight PL, vec3 Normal, vec3 ViewDir) {
	vec3 LightDir = normalize(PL.Position - FragPos);

	vec3 AmbientPart = PL.Ambient * texture(texture_diffuse1, UV).rgb;

	vec3 DiffusePart = PL.Diffuse * texture(texture_diffuse1, UV).rgb * max(0.0f, dot(Normal, LightDir));
	
	vec3 ReflectDir = normalize(reflect(-LightDir, Normal));
	vec3 SpecularPart = PL.Specular * texture(texture_specular1, UV).rgb * pow(max(0.0f, dot(ReflectDir, ViewDir)), 64.f);

	float Distance    = length(FragPos - PL.Position);
    float Attenuation = 1.0f / (PL.Kc + PL.Kl * Distance + PL.Kq * Distance * Distance);
    return Attenuation * (AmbientPart + DiffusePart + SpecularPart);
}

vec3 CalcDirectLight(DirectLight DL, vec3 Normal, vec3 ViewDir) {
	vec3 LightDir = normalize(-DL.Direction);

	vec3 AmbientPart = DL.Ambient * texture(texture_diffuse1, UV).rgb;
	vec3 DiffusePart = DL.Diffuse * texture(texture_diffuse1, UV).rgb * max(0.0f, dot(Normal, LightDir));
	vec3 ReflectDir = normalize(reflect(-LightDir, Normal));
	vec3 SpecularPart = DL.Specular * texture(texture_specular1, UV).rgb * pow(max(0.0f, dot(ReflectDir, ViewDir)), 64.f);

	return AmbientPart + DiffusePart + SpecularPart;
}