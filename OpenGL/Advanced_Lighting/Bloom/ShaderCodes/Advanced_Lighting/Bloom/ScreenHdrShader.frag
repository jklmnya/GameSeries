#version 330 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D HdrMap;
uniform sampler2D BlurMap;

void main() {
	const float Gamma = 2.2f;
	const float Exp = 1.0f;
	vec3 HdrColor = texture(HdrMap, UV).rgb;
	vec3 BlurColor = texture(BlurMap, UV).rgb;
	HdrColor += BlurColor;
	vec3 Result = vec3(1.0f) - exp(-HdrColor * Exp);
	// vec3 Result = HdrColor / (HdrColor + vec3(1.0f));
	Result = pow(Result, vec3(1.0f / Gamma));
	FragColor = vec4(Result, 1.0f);
}