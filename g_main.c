#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//todo: test math library (not bad so far)
//todo: perspective matrix
//todo: openAL 
//todo: editor (a level is just vertices and shit right?)
//todo: projection * view * model * vec4(aPos, 1.0)


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"

#include "t_math.h"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

#define true 1
#define false 0

#define DEBUG 1

//todo: clean up shader boilerplate

typedef unsigned int boolean;

int read_file_to_buffer(char **buffer, char *filename)
{
    //why are we 4 bytes over the end?
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
	return -1;
    }
    int size_bytes = 0;
    int i = 0;
    fseek(fp, 0, SEEK_END);
    size_bytes = ftell(fp);
    //size_bytes -= 4;
    rewind(fp);
    *buffer = (char*)malloc(size_bytes);
    while (i < size_bytes) {//wtf
	char c = fgetc(fp);	
	(*buffer)[i] = c;
	i++;
    }
    (*buffer)[i] = '\0';//make it a string
    return 0;
}

int load_shader(unsigned int *shader, char *filename, GLenum shadertype)
{
    char *shader_source;
    read_file_to_buffer(&shader_source, filename);
    *shader = glCreateShader(shadertype);
    glShaderSource(*shader, 1, &shader_source, NULL);
    glCompileShader(*shader);
    int shader_success;
    char shader_info_log[512];
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &shader_success);
    if (!shader_success) {
	glGetShaderInfoLog(*shader, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return -1;
    }
    //free(shader_source);//why can i not free this??
    return 0;
    
}

int load_shader_program(unsigned int *shader_program, char *vertex_filename, char *fragment_filename)
{
    unsigned int vertex_shader;
    load_shader(&vertex_shader, vertex_filename, GL_VERTEX_SHADER); 
    
    unsigned int fragment_shader;
    load_shader(&fragment_shader, fragment_filename, GL_FRAGMENT_SHADER);

    *shader_program = glCreateProgram();
    glAttachShader(*shader_program, vertex_shader);
    glAttachShader(*shader_program, fragment_shader);
    glLinkProgram(*shader_program);

    int shader_success;
    char shader_info_log[512];
    
    glGetProgramiv(*shader_program, GL_LINK_STATUS, &shader_success);
    if (!shader_success) {
	glGetProgramInfoLog(*shader_program, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return -1;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return 0;
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
	return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Transported", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_OPENGL);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
	return -1;
    }
    
    boolean running = true;

    float vertices[] = {
	//vertex 1, color 1, texture 1
	-0.75f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	//vertex 2, color 2, texture 2
	-0.25f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0, 0.0,
	//vertex 3, color 3, texture 3
	-0.5f,   -0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5, 0.5
    };

    unsigned int vertex_indices_a[] = {
	0, 1, 2,
    };

    float tex_coords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.5f, 1.0f//check these against my actual vertices
    };

    //set aside some space in the GPU and send our vertices to it


    unsigned int shader_program;

    load_shader_program(&shader_program, "test.vert", "test.frag");

    unsigned int test_vaos[1];
    glGenVertexArrays(1, test_vaos);

    glBindVertexArray(test_vaos[0]);


    //todo: wrap this
    unsigned int test_vbos[1];
    glGenBuffers(1, test_vbos);
    glBindBuffer(GL_ARRAY_BUFFER, test_vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int test_ebos[1];
    glGenBuffers(1, test_ebos);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, test_ebos[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices_a), vertex_indices_a, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//0 refers to the position in the layout of the shader. its 'vertex' on nvidia cards?
    glEnableVertexAttribArray(0);//this is now stored in test_vao I think
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 *sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);//this 'unbinds' the vbo
    glBindVertexArray(0);//this 'unbinds' the vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//this 'unbinds' the vbo




#if DEBUG
    int n_attributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attributes);
    printf("max attribs is %d\n", n_attributes);
#endif
    unsigned int last_time = 0, current_time;
    unsigned int time_elapsed;

    float total_time_ms = 0.0f;
    /*********************
     *
     *
     * DANGER: modiyfing global state here!
     * 
     * ************ */
    glUseProgram(shader_program);


    //load image
    int imwidth, imheight, nchannels;
    unsigned char *imdata = stbi_load("container.jpg", &imwidth, &imheight, &nchannels, 0);

    unsigned int test_texture;
    glGenTextures(1, &test_texture);

    glBindTexture(GL_TEXTURE_2D, test_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imwidth, imheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imdata);
    glGenerateMipmap(GL_TEXTURE_2D);


    unsigned int test_texture_2;

    glGenTextures(1, &test_texture_2);
    imdata = stbi_load("awesomeface.png", &imwidth, &imheight, &nchannels, 0);
    glBindTexture(GL_TEXTURE_2D, test_texture_2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imwidth, imheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imdata);
    glGenerateMipmap(GL_TEXTURE_2D);

    //set our sampler uniforms
    int tex1location = glGetUniformLocation(shader_program, "ourTexture");
    glUniform1i(tex1location, 0);
    int tex2location = glGetUniformLocation(shader_program, "ourTexture2");
    glUniform1i(tex2location, 1);

    
    
    
    stbi_image_free(imdata);

    SDL_GL_SetSwapInterval(0);//using vsync seems to be kinda shitty?

   
    Mat4 test_rot_x;
    mat4_create_x_rotation(test_rot_x, 0.0f);
    Mat4 test_translate;
    mat4_create_identity(test_translate);
    Vec3 v3pos = vec3_init(0.0f, 0.0f, 0.0f);
    int transform_loc = glGetUniformLocation(shader_program, "transform");
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, test_rot_x);

    float rot_angle = 0.0f;

    Mat4 perspective;
    mat4_create_perspective(perspective, 80.0f, 1280.0/720.0f, 0.1f, 100.0f);
    
    while (running) {
	current_time = SDL_GetTicks();
	time_elapsed = current_time - last_time;
	last_time = current_time;	
	float dt = time_elapsed/1000.0f;//dt is inconsistent, should be noted

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP]) {
	    	v3pos.x += dt;
		v3pos.y += dt;
		v3pos.z += dt;//does eventually disappear on horizon
	}
	if (keys[SDL_SCANCODE_DOWN]) {
	    	v3pos.x -= dt;
		v3pos.y -= dt;
		v3pos.z -= dt;//does eventually disappear on horizon
	}
	if (keys[SDL_SCANCODE_LEFT]) {
	}
	if (keys[SDL_SCANCODE_RIGHT]) {
	}
	
	rot_angle += dt;

	mat4_create_x_rotation(test_rot_x, rot_angle);

	//this requires an identity matrix to work
	mat4_create_identity(test_translate);
	mat4_create_translation(test_translate, v3pos);
	Mat4 trans_rot;
	Mat4 identity;
	mat4_create_identity(identity);
	mat4_mult(perspective, test_translate, trans_rot);
	Mat4 trans_rot_out;
	mat4_mult(trans_rot, identity, trans_rot_out);
	
	total_time_ms += dt;
	printf("dt is %f\n", dt);

	float green_value = (sin(total_time_ms)/2.0f) + 0.5f;
	float red_value = (sin(total_time_ms/2.0f)/2.0f) + 0.5f;
	int vertex_color_location = glGetUniformLocation(shader_program, "our_color");
	int offset_location = glGetUniformLocation(shader_program, "offset");
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_QUIT) {
		running = false;
		break;
	    }
	}

	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
	glClearColor(1.0f, 0.0, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, test_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, test_texture_2);

	glUniform4f(vertex_color_location, 0.3f, green_value, 0.3f, 1.0f);
	glUniform3f(offset_location, green_value, red_value, 0.0f);
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, trans_rot_out);
	glBindVertexArray(test_vaos[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	#if 0
	glUseProgram(shader_program_b);
	glBindVertexArray(test_vaos[1]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	#endif
	SDL_GL_SwapWindow(window);
    }
    
    
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
    return 0;
}
