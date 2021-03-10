#version 410 core

layout (location = 0) in vec3 point; //initial velocity


uniform mat4 transform;
uniform mat4 perspective;





void main()
{
    gl_Position = perspective * transform * vec4(point, 1.0f);
    gl_PointSize = 5.0f;
    
}