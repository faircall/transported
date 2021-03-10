#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 transform;
uniform mat4 perspective;

void main()
{
    gl_PointSize = 10.0f;
    gl_Position = perspective * transform * vec4(position, 1.0f);
}