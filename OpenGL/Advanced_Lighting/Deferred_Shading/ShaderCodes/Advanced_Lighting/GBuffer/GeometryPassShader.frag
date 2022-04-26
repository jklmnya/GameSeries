#version 330 core

layout (location = 0) out vec3 gPos;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 UV;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() {
	gPos = fs_in.FragPos;
	gNormal = normalize(fs_in.Normal);
	gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.UV).rgb;
	gAlbedoSpec.a = texture(texture_specular1, fs_in.UV).r;
}