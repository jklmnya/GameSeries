#version 330 core
out vec4 FragColor;

uniform vec3 ViewPos;

in vec3 Normal;
in vec3 FragPos;

struct Material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

struct Light {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform Material material;
uniform Light light;

void main()
{
    vec3 AmbientPart = material.Ambient * light.Ambient;

    vec3 normal = normalize(Normal);
    vec3 LightDir = normalize(light.Position - FragPos);
    vec3 DiffusePart = material.Diffuse * light.Diffuse * max(0.0f, dot(Normal, LightDir));

    vec3 ViewDir = normalize(ViewPos - FragPos);
    vec3 ReflectDir = reflect(-LightDir, normal);
    vec3 SpecularPart = material.Specular * light.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir)), material.Shininess);

    FragColor = vec4(AmbientPart + DiffusePart + SpecularPart, 1.0f);
}