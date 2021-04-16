#version 330 core
layout (location = 0) in vec3 pos;



uniform mat4 transform;
uniform mat4 perspective;


out vec3 color_pos;

void main()
{
    gl_Position = perspective * transform * vec4(pos, 1.0f);
    color_pos = pos;
    
}

