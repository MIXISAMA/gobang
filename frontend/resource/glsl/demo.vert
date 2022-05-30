#version 330 core

layout(location = 0) in vec3 aPosition;

layout (std140) uniform Camera
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

// uniform float pointSize;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    gl_PointSize = 50 / gl_Position.z;
}
