#version 330 core
layout (location = 0) in vec3 pos;



uniform mat4 view;
uniform mat4 model;
uniform mat4 perspective;


out vec3 color_pos;

void main()
{
    mat4 transform = model;
    gl_Position = perspective * transform * vec4(pos, 1.0f);

    color_pos = vec3(model * vec4(pos,1.0f));
    
}

