#version 330 core
out vec4 FragColor;

uniform vec3 ViewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 uv;

// Diffuse and Ambient use same vec3
struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    sampler2D Emission;
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
    vec3 AmbientPart = texture(material.Diffuse, uv).rgb * light.Ambient;

    vec3 normal = normalize(Normal);
    vec3 LightDir = normalize(light.Position - FragPos);
    vec3 DiffusePart = texture(material.Diffuse, uv).rgb * light.Diffuse * max(0.0f, dot(Normal, LightDir));

    vec3 ViewDir = normalize(ViewPos - FragPos);
    vec3 ReflectDir = reflect(-LightDir, normal);
    vec3 SpecularPart = texture(material.Specular, uv).rgb * light.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir)), material.Shininess);

    vec3 EmissionPart = texture(material.Emission, uv).rgb;

    FragColor = vec4(AmbientPart + DiffusePart + SpecularPart + EmissionPart, 1.0f);
}