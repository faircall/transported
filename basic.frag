#version 330 core
out vec4 frag_color;
in vec3 vertex_color;
in vec3 vertex_normal;
in vec3 frag_pos;



uniform mat4 transform;

vec3 light_pos = vec3(1.0f, 0.0f, 1.0f);

void main()
{
    vec3 light_pos_transformed = vec3(transform * vec4(light_pos, 1.0f));
    //vec3 light_pos_transformed = light_pos;
    vec3 light_direction = light_pos - frag_pos;
    light_direction = normalize(light_direction);
    //need to account for distance
    float inner_product = max(0.0f, dot(light_direction, vertex_normal));
    vec3 shaded_color = vertex_color * inner_product;
    frag_color = vec4(shaded_color, 0.0f);
}