#version 330 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D HDRMap;
uniform bool bUseHDR;
uniform float Exposure;

void main() {
	const float Gamma = 2.2f;
	vec3 HdrColor = texture(HDRMap, UV).rgb;
	if (bUseHDR) {
		vec3 Result = vec3(1.0f) - exp(-HdrColor * Exposure);
		// vec3 Result = HdrColor / (HdrColor + vec3(1.0f));
		Result = pow(Result, vec3(1.0f / Gamma));
		FragColor = vec4(Result, 1.0f);
	} else {
		vec3 Result = pow(HdrColor, vec3(1.0f / Gamma));
		FragColor = vec4(Result, 1.0f);
	}
}