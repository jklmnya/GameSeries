#version 330 core

out vec4 FragColor;

in VS_OUT {
	vec3 Normal;
	vec2 UV;
	vec3 FragPos;
} fs_in;

uniform vec3 ViewPos;
uniform vec3 LightPos;
uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;

void main() {
	vec3 Normal = normalize(fs_in.Normal);

	vec3 BaseColor = texture(DiffuseMap, fs_in.UV).rgb;
	vec3 Ambient = BaseColor * 0.2f;

	vec3 LightDir = normalize(LightPos - fs_in.FragPos);
	vec3 Diffuse = BaseColor * max(0.0f, dot(LightDir, Normal));

	vec3 ViewDir = normalize(ViewPos - fs_in.FragPos);
    vec3 ReflectDir = reflect(-LightDir, Normal);
    vec3 HalfwayDir = normalize(LightDir + ViewDir);  
    vec3 Specular = vec3(0.3f) * pow(max(dot(Normal, HalfwayDir), 0.0), 32.0);

	FragColor = vec4(Ambient + Diffuse + Specular, 1.0f);
}