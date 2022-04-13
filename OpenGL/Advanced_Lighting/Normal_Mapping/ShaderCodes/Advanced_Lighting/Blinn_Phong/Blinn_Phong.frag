#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
in vec3 FragPos;

uniform int bUsePhong;
uniform sampler2D texture1;
uniform vec3 LightPos;
uniform vec3 ViewPos;

void main() {
	vec3 Color = texture(texture1, UV).rgb;

	vec3 Ambient = Color * 0.05;

	vec3 LightDir = normalize(LightPos - FragPos);
	vec3 Diffuse = Color * max(0.0f, dot(LightDir, normalize(Normal)));

	float spec;
	vec3 ViewDir = normalize(ViewPos - FragPos);
	if (bUsePhong == 1) {
		vec3 ReflectDir = reflect(-LightDir, normalize(Normal));
		spec = pow(max(0.0f, dot(ViewDir, ReflectDir)), 8.0f);
	} else {
		vec3 HalfVec = normalize(LightDir + ViewDir);
		spec = pow(max(0.0f, dot(HalfVec, normalize(Normal))), 32.0f);
	}
	vec3 Specular = vec3(0.3f) * spec;
	FragColor = vec4(Ambient + Diffuse + Specular, 1.0f);
}