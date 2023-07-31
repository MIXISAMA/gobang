#version 330 core

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light uLight;
uniform vec3  uColor;
uniform vec3  uViewPos;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
} fs_in;

out vec4 oFragColor;

void main()
{
    // ambient
    vec3 ambient = uLight.ambient * uColor;

    // Diffuse
    vec3  lightDir = normalize(uLight.position - fs_in.fragPos);
    float diff     = max(dot(lightDir, fs_in.normal), 0.0);
    vec3  diffuse  = uLight.diffuse * diff * uColor;

    // Specular
    vec3  viewDir    = normalize(uViewPos - fs_in.fragPos);
    vec3  reflectDir = reflect(-lightDir, fs_in.normal);
    vec3  halfwayDir = normalize(lightDir + viewDir);  
    float spec       = pow(max(dot(fs_in.normal, halfwayDir), 0.0), 16);
    vec3  specular   = uLight.specular * spec;

    oFragColor = vec4(ambient + diffuse + specular, 1.0f);
}
