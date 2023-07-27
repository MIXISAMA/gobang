#version 330 core

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light uLight;

uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;
uniform vec3 uViewPos;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoords;

out VS_OUT {
    vec3 fragPos;
    vec2 texCoords;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} vs_out;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0f);
    vs_out.fragPos = vec3(uModel * vec4(aPosition, 1.0));   
    vs_out.texCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);    
    
    mat3 TBN = transpose(mat3(T, B, N));  
    vs_out.tangentLightPos = TBN * uLight.position;
    vs_out.tangentViewPos  = TBN * uViewPos;
    vs_out.tangentFragPos  = TBN * vs_out.fragPos;
}
