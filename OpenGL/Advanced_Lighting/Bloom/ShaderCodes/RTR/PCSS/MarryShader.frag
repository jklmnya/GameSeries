#version 330 core

in vec3 Normal;
in vec2 UV;
in vec4 PositionFromLight;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D ShadowMap;

uniform vec3 ViewPos;

struct DirectLight {
	vec3 Direction;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

uniform DirectLight uDirectLight;

out vec4 FragColor;

vec3 CalcDLAmbient(DirectLight DL, vec3 BaseColor);
vec3 CalcDLDiffuse(DirectLight DL, vec3 BaseColor, vec3 NNormal);
vec3 CalcDLSpecular(DirectLight DL, vec3 BaseColor, vec3 NNormal);

float HardShadow(vec3 Coords) {
	float ShadowDepth = texture(ShadowMap, Coords.xy).r;
	float CurrentDepth = Coords.z;
	if (ShadowDepth + 0.1f < CurrentDepth)
		return 0.0f;
	else
		return 1.0f;
}

void main() {

	vec3 BaseColor = texture(texture_diffuse1, UV).rgb;
	vec3 NNormal = normalize(Normal);

	vec3 Coords = PositionFromLight.xyz;
	Coords = Coords * 0.5f + 0.5f;
	float Visibility = HardShadow(Coords);

	vec3 FinalColor = CalcDLAmbient(uDirectLight, BaseColor) +
		Visibility * (
		CalcDLDiffuse(uDirectLight, BaseColor, NNormal) + 
			CalcDLSpecular(uDirectLight, BaseColor, NNormal));

	FragColor = vec4(FinalColor, 1.0f);
}

vec3 CalcDLAmbient(DirectLight DL, vec3 BaseColor) {
	return 0.6f * DL.Ambient * BaseColor;
}

vec3 CalcDLDiffuse(DirectLight DL, vec3 BaseColor, vec3 NNormal) {
	return 3.2f * DL.Diffuse * BaseColor * max(0.0f, dot(NNormal, normalize(-DL.Direction)));
}

vec3 CalcDLSpecular(DirectLight DL, vec3 BaseColor, vec3 NNormal) {
	vec3 ViewDir = normalize(ViewPos - FragPos);
	vec3 HalfVector = normalize(ViewDir + normalize(-DL.Direction));
	return 0.2f * DL.Specular * BaseColor * pow(max(0.0f, dot(NNormal, HalfVector)), 16.f);
}