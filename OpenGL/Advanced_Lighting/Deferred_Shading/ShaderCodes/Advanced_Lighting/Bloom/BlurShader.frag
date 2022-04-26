#version 330 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D ImageMap;
uniform bool bHorizontal;

const float Weight[5] = float[] (0.2270270270, 0.1945945946, 
					0.1216216216, 0.0540540541, 0.0162162162);

void main() {
	vec2 Texture_Offset = 1.0f / textureSize(ImageMap, 0);
	vec3 Result = texture(ImageMap, UV).rgb * Weight[0];
	if (bHorizontal) {
		for (int i = 1;i<5;++i) {
			vec2 Offset = vec2(Texture_Offset.x, 0.0f) * i;
			Result += texture(ImageMap, UV + Offset).rgb * Weight[i];
			Result += texture(ImageMap, UV - Offset).rgb * Weight[i];
		}
	} else {
		for (int i = 1;i<5;++i) {
			vec2 Offset = vec2(0.0f, Texture_Offset.y) * i;
			Result += texture(ImageMap, UV + Offset).rgb * Weight[i];
			Result += texture(ImageMap, UV - Offset).rgb * Weight[i];
		}
	}
	FragColor = vec4(Result, 1.0f);
}