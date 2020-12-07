#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

#define true 1
#define false 0

typedef unsigned int boolean;

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
	//tri 1
	-0.75f, -0.5f, 0.0f,
	-0.25f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	
	//tri 2

	0.25f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
    };

    unsigned int vertex_indices_a[] = {
	0, 1, 2,
    };
    unsigned int vertex_indices_b[] = {
	1, 3, 4
    };

    //set aside some space in the GPU and send our vertices to it



    const char *vertex_shader_source = "#version 330 core\n"
	    "layout (location = 0) in vec3 aPos;\n"
	    "void main()\n"
	    "{\n"
	    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	    "}\0";

    const char *fragment_shader_source = "#version 330 core\n"
	    "out vec4 frag_color;\n"
	    "void main()\n"
	    "{\n"
	    "    frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	    "}\0";

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    
    int shader_success;
    char shader_info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_success);
    if (!shader_success) {
	glGetShaderInfoLog(vertex_shader, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return -1;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_success);
    if (!shader_success) {
	glGetShaderInfoLog(fragment_shader, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return -1;
    }


    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_success);
    if (!shader_success) {
	glGetProgramInfoLog(shader_program, 512, NULL, shader_info_log);
	printf("%s\n", shader_info_log);
	return -1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);



    unsigned int test_vaos[2];
    glGenVertexArrays(2, test_vaos);

    glBindVertexArray(test_vaos[0]);



    unsigned int test_vbos[2];
    glGenBuffers(2, test_vbos);
    glBindBuffer(GL_ARRAY_BUFFER, test_vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int test_ebos[2];
    glGenBuffers(2, test_ebos);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, test_ebos[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices_a), vertex_indices_a, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//0 refers to the position in the layout of the shader. its 'vertex' on nvidia cards?
    glEnableVertexAttribArray(0);//this is now stored in test_vao I think
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);//this 'unbinds' the vbo
    glBindVertexArray(0);//this 'unbinds' the vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//this 'unbinds' the vbo



    glBindVertexArray(test_vaos[1]);





    glBindBuffer(GL_ARRAY_BUFFER, test_vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, test_ebos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices_b), vertex_indices_b, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//0 refers to the position in the layout of the shader. its 'vertex' on nvidia cards?
    glEnableVertexAttribArray(0);//this is now stored in test_vao I think
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);//this 'unbinds' the vbo
    glBindVertexArray(0);//this 'unbinds' the vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//this 'unbinds' the vbo


    
    while (running) {
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

	glUseProgram(shader_program);
	glBindVertexArray(test_vaos[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(test_vaos[1]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	SDL_GL_SwapWindow(window);
    }
    
    
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
    return 0;
}
