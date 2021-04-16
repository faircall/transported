#version 330 core

uniform vec2 resolution;

in vec3 color_pos;

out vec4 frag_color;

uniform int side;

void main()
{
	vec2 st = gl_FragCoord.xy / resolution.xy;
	vec3 side_color = vec3(0.0f, 0.2f, 0.7f);

	if (side == 1) {
	   side_color = vec3(st.x, st.y*0.3, 0.7f);
	}
	if (side == 2) {
	   side_color = vec3(st.x*0.01, st.y*0.5f, 0.7f);
	}
	if (side == 3) {
	   side_color = vec3(st.x*0.1f, st.y*0.6f, 0.7f);
	}
	if (side == 4) {
	   side_color = vec3(st.y*0.01f, st.x*0.7f, 0.7f);
	}
	if (side == 5) {
	   side_color = vec3(sin(st.x), st.y*0.8f, 0.7f);
	}
	frag_color = vec4(side_color, 1.0f);
}