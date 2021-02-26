#version 410 core

in float opacity;

out vec4 frag_color;

vec4 particle_color = vec4(0.5, 0.5, 0.8, 0.8);

void main()
{
    frag_color.a = opacity * particle_color.a;
    frag_color.rgb = particle_color.rgb; //check this later
}