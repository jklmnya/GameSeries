#version 330 core

in vec4 PositionFromLight;
out vec4 FragColor;

uniform sampler2D ShadowMap;

// Shadow map related variables
#define NUM_SAMPLES 50
#define BIAS 0.002f
#define W_LIGHT 1.0f

vec2 SampleCoords[NUM_SAMPLES];

void Sample(vec2 UV, int Len) {
	vec2 TexelBias = 1.0f / textureSize(ShadowMap, 0);
	int cnt = 0, t = Len / 2;
	for (int i = -t;i<=t;++i) {
		for (int j = -t;j<=t;++j) {
			SampleCoords[cnt++] = UV + vec2(i, j) * TexelBias;
		}
	}
}

float HardShadow(vec3 Coords) {
	float ShadowDepth = texture(ShadowMap, Coords.xy).r;
	float CurrentDepth = Coords.z;
	// Avoid Self Occlusion, add a bias 
	// If Z_shadow_map + bias < Z_current_depth -> will cause occlusion
	if (ShadowDepth + BIAS < CurrentDepth)
		return 0.0f;
	else
		return 1.0f;
}

float PCF(vec3 Coords) {
	int BlockedNum = 0;
	Sample(Coords.xy, 5);
	// sample range [Len * Len] to get average "visibility"
	for (int i = 0;i<5 * 5;++i) {
		float ShadowDepth = texture(ShadowMap, SampleCoords[i]).r;
		if (ShadowDepth + BIAS < Coords.z)
			++BlockedNum;
	}
	return float(5 * 5 - BlockedNum) / (5 * 5);
}

float CalaAvgBlockerDepth(vec3 Coords) {
	float Depths = 0.0f;
	int BlockedNum = 0;
	Sample(Coords.xy, 5);
	for (int i = 0;i<NUM_SAMPLES;++i) {
		float ShadowDepth = texture(ShadowMap, SampleCoords[i]).r;
		if (ShadowDepth + BIAS < Coords.z) {
			++BlockedNum;
			Depths += ShadowDepth;
		}
	}
	if (BlockedNum == 0)
		return 2.0f;
	return BlockedNum == 0 ? 1.0f : Depths / float(BlockedNum);
}

float PCSS(vec3 Coords) {
	float BlockerSize = CalaAvgBlockerDepth(Coords);
	if (BlockerSize > 1.0f)
		return 1.0f;
	float WPenumbar = (Coords.z - BlockerSize) * W_LIGHT / BlockerSize;
	int Len;
	if (WPenumbar < 0.25f)
		Len = 1;
	else if (WPenumbar < 0.5f)
		Len = 3;
	else if (WPenumbar < 0.75f)
		Len = 5;
	else
		Len = 7;
	Sample(Coords.xy, Len);
	int BlockedNum = 0;
	for (int i = 0;i<Len * Len;++i) {
		float ShadowDepth = texture(ShadowMap, SampleCoords[i]).r;
		if (ShadowDepth + BIAS < Coords.z)
			++BlockedNum;
	}
	return float(Len * Len - BlockedNum) / (Len * Len);
}

void main() {
	vec3 BaseColor = vec3(0.4f);

	vec3 Coords = PositionFromLight.xyz;
	Coords = Coords * 0.5f + 0.5f;

	// float Visibility = HardShadow(Coords);
	float Visibility = PCF(Coords);
	// float Visibility = PCSS(Coords);

	FragColor = vec4(BaseColor * Visibility, 1.0f);
}