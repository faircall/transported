#version 330 core

#define PI 3.141596

uniform vec2 resolution;

in vec3 color_pos;



out vec4 frag_color;

uniform int side;

uniform float viewing_angle_x;
uniform float viewing_angle_y;

void main()
{
	vec4 ndc;
	ndc.x = ((gl_FragCoord.x / resolution.x) - 0.5f) * 2.0f;
	ndc.y = ((gl_FragCoord.y / resolution.y) - 0.5f) * 2.0f;	
	ndc.z = (gl_FragCoord.z  - 0.5f) * 2.0f;
	ndc.w = 1.0f;

	vec2 st;
	//issue with these is they give us screen space
	//but we want world space somehow
	st.x = gl_FragCoord.x / (resolution.x);
	st.y = gl_FragCoord.y / resolution.y;

	float angle_x = viewing_angle_x;
	float angle_y = viewing_angle_y;

	float centre_x = 0.5;
	float dist_from_centre = sqrt((centre_x - st.x) * (centre_x - st.x));


	vec3 side_color = vec3(color_pos.x/resolution.x, color_pos.y/resolution.y, 0.0f);
#if 0
	if (side == 1) {
	   side_color = vec3(1.0 - dist_from_centre*2.0f, 0.0f, 0.0);
	}
	if (side == 2) {
	   side_color = vec3(0.2 + 0.2*cos(angle_x), 0.0f, 0.0);		
	}
	if (side == 3) {
	   side_color = vec3(0.2 + 0.2*cos(angle_x), 0.0f, 0.0);		

	}
	if (side == 4) {
	   side_color = vec3(0.2 + 0.2*cos(angle_x), 0.0f, 0.0);		

	}
	if (side == 5) {
	
	   side_color = vec3(0.0f, 0.2*sin(angle_y), 0.0f);
	}
#endif
	frag_color = vec4(side_color, 1.0f);
}