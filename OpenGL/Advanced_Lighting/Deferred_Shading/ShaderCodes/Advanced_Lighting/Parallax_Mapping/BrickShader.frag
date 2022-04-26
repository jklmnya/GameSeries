#version 330 core

out vec4 FragColor;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D HeightMap;

in VS_OUT {
	vec3 FragPos;
	vec3 LightPos;
	vec3 ViewPos;
	vec2 UV;
} fs_in;

vec2 ParallaxMapping(vec2 UV, vec3 ViewDir);

void main() {
	vec3 ViewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
	// vec2 UV = fs_in.UV;
	vec2 UV = ParallaxMapping(fs_in.UV, ViewDir);
	if (UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0)
        discard;

	vec3 BaseColor = texture(DiffuseMap, UV).rgb;
	vec3 Normal = texture(NormalMap, UV).rgb;
	Normal = normalize(Normal * 2.0f - 1.0f);

	vec3 Ambient = BaseColor * 0.1f;

	vec3 LightDir = normalize(fs_in.LightPos - fs_in.FragPos);
	vec3 Diffuse = BaseColor * max(0.0f, dot(Normal, LightDir));

	vec3 HalfVec = normalize(ViewDir + LightDir);
	vec3 Specluar = vec3(0.2f) * pow(max(0.0f, dot(Normal, HalfVec)), 32.f);

	FragColor = vec4(Ambient + Diffuse + Specluar, 1.0f);
}

vec2 ParallaxMapping(vec2 UV, vec3 ViewDir) {
	float height = texture(HeightMap, UV).r;
	return UV - ViewDir.xy / ViewDir.z * (height * 0.1f);
}