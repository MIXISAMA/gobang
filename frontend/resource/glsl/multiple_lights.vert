#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
// out vec2 TexCoords;

uniform mat4 model;
layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};
uniform float nouse[5];

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    // TexCoords = aTexCoords;

    float gg = nouse[0] + nouse[4];
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
