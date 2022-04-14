#version 330 core

out vec4 FragColor;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D HeightMap;

in VS_OUT {
	vec3 FragPos;
	vec2 UV;
	vec3 LightPos;
	vec3 ViewPos;
} fs_in;

vec2 ParallaxMapping(vec2 UV, vec3 ViewDir);

void main() {
	vec3 ViewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
	vec2 UV = ParallaxMapping(fs_in.UV, ViewDir);

	vec3 BaseColor = texture(DiffuseMap, UV).rgb;
	vec3 Normal = texture(NormalMap, UV).rgb;
	Normal = normalize(Normal * 2.0f - 1.0f);

	vec3 Ambient = 0.1f * BaseColor;

	vec3 LightDir = normalize(fs_in.LightPos - fs_in.FragPos);
	vec3 Diffuse = BaseColor * max(0.0f, dot(Normal, LightDir));

	vec3 HalfVec = normalize(ViewDir + LightDir);
	vec3 Specular = vec3(0.2f) * pow(max(0.0f, dot(HalfVec, Normal)), 32.f);

	FragColor = vec4(Ambient + Diffuse + Specular, 1.0f);
}

vec2 ParallaxMapping(vec2 UV, vec3 ViewDir) {
	const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), ViewDir)));  
	float layerDepth = 1.0f / numLayers;
	float currentDepth = 0.0f;

	vec2 dir = ViewDir.xy / ViewDir.z * 0.1f;
	vec2 deltaUV = dir / numLayers;

	vec2 currentUV = UV;
	float currentDepthMap = texture(HeightMap, currentUV).r;

	while (currentDepth < currentDepthMap) {
		currentUV -= deltaUV;
		currentDepthMap = texture(HeightMap, currentUV).r;
		currentDepth += layerDepth;
	}

	vec2 lastUV = currentUV + deltaUV;
	float depth_before_occ = texture(HeightMap, lastUV).r - currentDepth + layerDepth;
	float depth_occ_after = currentDepth - currentDepthMap;

	float weight = depth_occ_after / (depth_occ_after + depth_before_occ);

	return lastUV * weight + currentUV * (1.0f - weight);
}