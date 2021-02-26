#version 410 core

layout (location = 0) in vec3 v_i; //initial velocity
layout (location = 1) in float start_time;

uniform mat4 transform;
uniform mat4 perspective;
uniform vec3 emitter_position_world;
uniform float time_elapsed;

out float opacity;

void main()
{
    float t = time_elapsed - start_time;
    t = mod(t, 3.0f);
    opacity = 0.0f;

    vec3 position = emitter_position_world;

    vec3 acceleration = vec3(0.0f, -1.0f, 0.0f);

    //newtonian eqn of motion
    position += v_i * t + 0.5 * acceleration * t * t;

    opacity = 1.0f - (t / 3.0f);

    gl_Position = perspective * transform * vec4(position, 1.0f);
    gl_PointSize = 15.0f;
    
}