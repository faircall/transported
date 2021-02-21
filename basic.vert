#version 330 core
layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 vn;
layout (location = 2) in vec3 vc;

uniform mat4 transform;
//this is actually combined model-view
//better i think
//to separate to model / view 

uniform mat4 perspective;

       
out vec3 vertex_color;
out vec3 frag_pos;
out vec3 vertex_normal;




void main()
{
    mat4 transform_perspective = perspective * transform;//whhhhhyyy
    vertex_color = vc;

    vec4 transformed_pos = transform * vec4(vp, 1.0f);
    frag_pos = vec3(transformed_pos);

    //the 'normal matrix' is used
    //note that transpose-inverse is commutative (i.e same as inverse-transpose)
    vertex_normal = normalize(mat3(transpose(inverse(transform)))*vn);

    gl_Position = transform_perspective * vec4(vp, 1.0f);    
}