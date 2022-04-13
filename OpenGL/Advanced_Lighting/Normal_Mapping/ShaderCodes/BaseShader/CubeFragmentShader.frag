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

#define POINT_LIGHT_NUM 4

uniform Material material;
uniform DirectLight directLight;
uniform PointLight pointLight[POINT_LIGHT_NUM];
uniform SpotLight spotLight;

vec3 CalcDirectLight(DirectLight directLight, vec3 Normal, vec3 ViewDir);
vec3 CalcPointLight(PointLight pointLight, vec3 Normal, vec3 ViewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 Normal, vec3 ViewDir);

void main()
{
    // Get Normal and View Direction
    vec3 normal  = normalize(Normal);
    vec3 ViewDir = normalize(ViewPos - FragPos);
    vec3 Result  = CalcDirectLight(directLight, normal, ViewDir); 
    for (int i = 0;i<POINT_LIGHT_NUM;++i) {
        Result  += CalcPointLight(pointLight[i], normal, ViewDir);
    }
    Result      += CalcSpotLight(spotLight, normal, ViewDir);
    FragColor    = vec4(Result, 1.0f);
}

vec3 CalcDirectLight(DirectLight directLight, vec3 Normal, vec3 ViewDir) {
    // Ambient
    vec3 Ambient    = texture(material.Diffuse, uv).rgb * directLight.Ambient;
    vec3 LightDir   = normalize(-directLight.Direction);
    // Diffuse
    vec3 Diffuse    = texture(material.Diffuse, uv).rgb * directLight.Diffuse * max(0.0f, dot(LightDir, Normal));
    // Specular
    vec3 ReflectDir = reflect(-LightDir, Normal);
    vec3 Specular   = texture(material.Specular, uv).rgb * directLight.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir)), material.Shininess);
    return Ambient + Diffuse + Specular;
}

vec3 CalcPointLight(PointLight pointLight, vec3 Normal, vec3 ViewDir) {
    // Ambient
    vec3 Ambient      = texture(material.Diffuse, uv).rgb * pointLight.Ambient;
    vec3 LightDir     = normalize(pointLight.Position - FragPos);
    // Diffuse
    vec3 Diffuse      = texture(material.Diffuse, uv).rgb * pointLight.Diffuse * max(0.0f, dot(LightDir, Normal));
    // Specular
    vec3 ReflectDir   = reflect(-LightDir, Normal);
    vec3 Specular     = texture(material.Specular, uv).rgb * pointLight.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir)), material.Shininess);
    // Attenuation 
    float Distance    = length(FragPos - pointLight.Position);
    float Attenuation = 1.0f / (pointLight.Kc + pointLight.Kl * Distance + pointLight.Kq * Distance * Distance);
    return Attenuation * (Ambient + Diffuse + Specular);
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 Normal, vec3 ViewDir) {
    vec3 LightDir     = normalize(spotLight.Position - FragPos);
    // Check if Can Shader
    float theta       = dot(normalize(spotLight.Direction), -LightDir);
    if (theta < spotLight.OutCutOff)
        return vec3(0.0f);
    // Ambient
    vec3 Ambient      = texture(material.Diffuse, uv).rgb * spotLight.Ambient;
    // Diffuse
    vec3 Diffuse      = texture(material.Diffuse, uv).rgb * spotLight.Diffuse * max(0.0f, dot(LightDir, Normal));
    // Specular
    vec3 ReflectDir   = reflect(-LightDir, Normal);
    vec3 Specular     = texture(material.Specular, uv).rgb * spotLight.Specular * pow(max(0.0f, dot(ViewDir, ReflectDir)), material.Shininess);
    // Attenuation
    float Distance    = length(FragPos - spotLight.Position);
    float Attenuation = 1.0f / (spotLight.Kc + spotLight.Kl * Distance + spotLight.Kq * Distance * Distance);
    // Lerp
    float Epsilon     = spotLight.CutOff - spotLight.OutCutOff;
    float Intensity   = clamp((theta - spotLight.OutCutOff) / Epsilon, 0.0f, 1.0f);
    return Intensity * Attenuation * (Ambient + Diffuse + Specular);
}