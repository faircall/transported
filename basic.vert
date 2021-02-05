#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vn;
layout (location = 2) in vec3 vc;

uniform mat4 transform;
       
out vec3 vertex_color;

void main()
{
    vertex_color = vc;
    gl_Position = transform * vec4(aPos, 1.0f);
}