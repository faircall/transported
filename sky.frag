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
	//first:
	//draw a 'sun' in the lower-center of the screen
	//then 'shift' its center according to
	//angle of the player?
	//we could actually draw this is an actual
	//object (sphere or whatever)
	//and pass it in at a constant translation,
	//whilst the rotation matrix would solve the correct positioning

	float cx = 0.5f;
	float cy = 0.5f;
	float radius = 0.1f;



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
	vec3 side_color = vec3(0.0f, 0.0f, 0.0f);


	float rotx = (st.x - cos(viewing_angle_x*0.5f)); //between 0 and 2?

	//issue is this doesn't account for how
	//far away it is
	float roty = 1.0f - (st.y + sin(viewing_angle_y));
	
	if ((rotx - cx) * (rotx - cx) + (roty - cy)*(roty - cy) < radius*radius) {
	   side_color = vec3(1.0f, 1.0f, 0.0f);
	} else {
	   side_color = vec3(1.0f - min(1.0f, (rotx - cx)*(rotx - cx)), 0.0f, 0.0f);
	}
	


	frag_color = vec4(side_color, 1.0f);
}