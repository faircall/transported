#version 330 core
layout (location = 0) in vec3 aPos;



uniform mat4 transform;
uniform mat4 perspective;

out vec3 our_color;


void main()
{
    gl_Position = perspective * transform * vec4(aPos, 1.0f);
    our_color = aPos;
}