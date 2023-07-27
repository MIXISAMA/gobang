#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D normals;
    float     shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material uMaterial;
uniform Light    uLight;

in VS_OUT {
    vec3 fragPos;
    vec2 texCoords;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} fs_in;

out vec4 oFragColor;

void main()
{
    vec3 normal = texture(uMaterial.normals, fs_in.texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // ambient
    vec3 color   = texture(uMaterial.diffuse, fs_in.texCoords).rgb;
    vec3 ambient = uLight.ambient * color;

    // Diffuse
    vec3  lightDir = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
    float diff     = max(dot(lightDir, normal), 0.0);
    vec3  diffuse  = uLight.diffuse * diff * color;

    // Specular
    vec3  viewDir    = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
    vec3  reflectDir = reflect(-lightDir, normal);
    vec3  halfwayDir = normalize(lightDir + viewDir);  
    float spec       = pow(max(dot(normal, halfwayDir), 0.0), uMaterial.shininess);
    vec3  specular   = uLight.specular * spec;

    oFragColor = vec4(ambient + diffuse + specular, 1.0f);
}
