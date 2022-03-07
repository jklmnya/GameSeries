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
    float Shininess;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    float CutOff;
    float OutCutOff;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Kc;
    float Kl;
    float Kq;
};

struct DirectLight {
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct PointLight {
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Kc;
    float Kl;
    float Kq;
};

uniform Material material;
uniform DirectLight directLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

void main()
{

    vec3 normal = normalize(Normal);
    vec3 ViewDir = normalize(ViewPos - FragPos);

    // Direct Light
    vec3 Ambient1 = texture(material.Diffuse, uv).rgb * directLight.Ambient;
    vec3 LightDir1 = normalize(-directLight.Direction);
    vec3 Diffuse1 = texture(material.Diffuse, uv).rgb * directLight.Diffuse * max(0.0f, dot(normal, LightDir1));
    vec3 ReflectDir1 = reflect(-LightDir1, normal);
    vec3 Specular1 = texture(material.Specular, uv).rgb * directLight.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir1)), material.Shininess);
       
    // Point Light
    float distance1 = length(pointLight.Position - FragPos);
    float att2      = 1.0f / (pointLight.Kc + pointLight.Kl * distance1 + pointLight.Kq * distance1 * distance1);
    vec3 Ambient2 = texture(material.Diffuse, uv).rgb * pointLight.Ambient;
    vec3 LightDir2 = normalize(pointLight.Position - FragPos);
    vec3 Diffuse2 = texture(material.Diffuse, uv).rgb * pointLight.Diffuse * max(0.0f, dot(normal, LightDir2));
    vec3 ReflectDir2 = reflect(-LightDir2, normal);
    vec3 Specular2 = texture(material.Specular, uv).rgb * pointLight.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir2)), material.Shininess);

    // Spot Light
    vec3 LightDir3 = normalize(spotLight.Position - FragPos);
    float theta    = dot(LightDir3, normalize(-spotLight.Direction));

    vec3 Ambient3 = texture(material.Diffuse, uv).rgb * spotLight.Ambient;
    vec3 Diffuse3 = texture(material.Diffuse, uv).rgb * spotLight.Diffuse * max(0.0f, dot(normal, LightDir3));

    vec3 ReflectDir3 = reflect(-LightDir3, normal);
    vec3 Specular3 = texture(material.Specular, uv).rgb * spotLight.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir3)), material.Shininess);

    float distance2 = length(spotLight.Position - FragPos);
    float att3      = 1.0f / (spotLight.Kc + spotLight.Kl * distance2 + spotLight.Kq * distance2 * distance2);
    float epsilon   = spotLight.CutOff - spotLight.OutCutOff;
    float intensity = clamp((theta - spotLight.OutCutOff) / epsilon, 0.0f, 1.0f);

    FragColor = vec4(Ambient1 + Diffuse1 + Specular1 + 
        Ambient2 + att2 * (Diffuse2 + Specular2) + Ambient3 + intensity * att3 * (Diffuse3 + Specular3), 1.0f);
}