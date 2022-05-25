#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// uniform float pointSize;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    gl_PointSize = 5 / gl_Position.z;
}
