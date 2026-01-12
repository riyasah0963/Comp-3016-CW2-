#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 camPos;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

struct Light {
    vec3 position;
    vec3 color;
};
uniform Light light;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 albedo = texture(albedoMap, TexCoord).rgb;
    float metallic = texture(metallicMap, TexCoord).r;
    float roughness = texture(roughnessMap, TexCoord).r;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - FragPos);
    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = pow(max(dot(N, H), 0.0), (1.0 - roughness) * 128.0);
    float G = max(dot(N, L), 0.0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * G + 0.001;

    vec3 specular = numerator / denominator;
    vec3 kD = (1.0 - F) * (1.0 - metallic);

    vec3 color = (kD * albedo / PI + specular) * light.color * G;
    FragColor = vec4(color, 1.0);
}
