#version 330 core

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light uLight;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
} vs_out;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vs_out.fragPos = aPos;
    vs_out.normal = aNormal;
}
