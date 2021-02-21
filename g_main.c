#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//todo: redo matrix code to be column-major rather than row-major, avoid having to transpose each frame


//todo:

//todo: quaternion rotation
//todo: geometric algebra support (rotors bivectors etc)

//todo: openAL for sound
//todo: editor (a level is just vertices and shit right?)


//todo: clouds. start with a texture and change it some ways?
//or use noise to make the clouds?
//done: basic alpha textures via discard (this doesn't allow varying transparency though
//note: we could have moving clouds inside our 'skybox' by translating them
//but not according to player movement. i guess that's how they're done
//in the old Unreal engine games?


//wrap some of the drawing code -semidone

//todo: assimp for mesh/animation imports - semidone

//todo: backface culling or discard or whatever


//todo: some basic lighting
//for this I think I'll need to send the transformation and perspective matrices in separately
//so we can calculate lighting in world space

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "t_math.h"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

#define true 1
#define false 0

#define DEBUG 1

//todo: clean up shader boilerplate

typedef unsigned int boolean;

typedef struct {
    float r;
    float g;
    float b;
} Color3;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color4;

void draw_sky(float angle_x, int sky_shader, int sky_vao, int sky_texture, int transform_loc, int time_ms_loc, float current_time_ms, Mat4 perspective)
{
    //so in this case I think we want to
    //translate then rotate
    //translate to 0, -0.94, -0.94 I think
    //rotation of like...-2.17 seems about right, but that's probably irrelevant?
    //need to remember to restrict the players ability to rubberneck

    
    //pretty much got this down now
    


    //draw it
    //how do you know where the texture is located?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sky_texture);
    
    glUseProgram(sky_shader);
    glUniform1f(time_ms_loc, current_time_ms);
    //glUniform4f(vertex_color_location, 0.3f, green_value, 0.3f, 1.0f);
    //glUniform3f(offset_location, green_value, red_value, 0.0f);

    //glUniformMatrix4fv(transform_loc, 1, GL_FALSE, rot_trans_out);
    glBindVertexArray(sky_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    
}

void draw_object(int model_shader, int model_vao, int vertex_count, Vec3 pos, Vec3 camera, Mat4 perspective, int transform_loc, int perspective_loc, float angle_x, float angle_y)
{
    //so in this case I think we want to
    //rotate THEN translate?
    //translate to 0, -0.94, -0.94 I think
    //rotation of like...-2.17 seems about right, but that's probably irrelevant?
    //need to remember to restrict the players ability to rubberneck

    //pretty much got this down now


    
    //create rotation matrix and transpose it
    Mat3 rotate_x = mat3_create_rotate_x(angle_x);

    Mat3 rotate_y = mat3_create_rotate_y(angle_y);

    //multiply rotation by translation (i.e rotate then translate)
    //mat4_mult(rotate_x_transpose, tmp_translate_transpose, rotate_translate);
    //then multiply by perspective matrix
    Mat3 rotate_x_y = mat3_mult(rotate_y, rotate_x);

    //order?
    //
    Mat4 model_rotation = mat4_from_mat3(rotate_x_y);

    Mat4 model_translation = mat4_create_translation(pos);

    Mat4 mrt = mat4_mult(model_translation, model_rotation);
    //Mat4 transformation = mat4_create_translation_rotation(rotate_x_y, camera);

    
    Mat4 camera_transformation = mat4_create_translation(camera);

    Mat4 transformation = mat4_mult(camera_transformation, mrt);
    
    //and send to the shader
    glUseProgram(model_shader);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transformation.elements);
    glUniformMatrix4fv(perspective_loc, 1, GL_FALSE, perspective.elements);

    glBindVertexArray(model_vao);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    
}

void draw_quad_model()
{
}

void draw_animated_quad(Vec3 camera, Vec3 model, float model_angle, GLuint shader, GLuint vao, int time_loc, int transform_loc, float current_time)
{

}

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
    *buffer = (char*)malloc(size_bytes+1);//space for terminator?
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

int load_model_file(char *file_name, char *ply_file_name, GLuint *vao, float **model_vertices, float **model_normals, float **model_colors, int *model_vertex_count)
{
    //VERY dumb workaround for colors:
    //pull the colors from the ply file
    //pull the vertices and normals from the
    //fbx file



    
    const struct aiScene *scene = aiImportFile(file_name, aiProcess_Triangulate);
    
    const struct aiScene *ply_scene = aiImportFile(ply_file_name, aiProcess_Triangulate);
    if (!scene) {
	fprintf(stderr, "Error reading mesh");
	return -1;
    }
    if (!ply_scene) {
	fprintf(stderr, "Error reading mesh");
	return -1;
    }

    const struct aiMesh *mesh = scene->mMeshes[0];
    const struct aiMesh *ply_mesh = ply_scene->mMeshes[0];
    
    printf("%d vertices in the mesh\n", mesh->mNumVertices);
    int vertices = mesh->mNumVertices;
    int faces = mesh->mNumFaces;

    boolean has_vertices = false;
    boolean has_normals = false;
    boolean has_faces = false;
    boolean has_colors = false;
    
    *model_vertex_count = vertices;
    (*model_vertices) = (float*)malloc(sizeof(float) * vertices * 3);
    (*model_normals) = (float*)malloc(sizeof(float) * vertices * 3);
    (*model_colors) = (float*)malloc(sizeof(float) * vertices * 3);


    if (mesh->mVertices != NULL) {
	has_vertices = true;
	for (int i = 0; i < vertices; i++) {
	    const struct aiVector3D *vp = &(mesh->mVertices[i]);
	
	    (*model_vertices)[i * 3] = (float)vp->x;
	    (*model_vertices)[(i * 3) + 1] = (float)vp->y;
	    (*model_vertices)[(i * 3) + 2] = (float)vp->z;
	}
    }

    if (mesh->mNormals != NULL) {
	has_normals = true;
	for (int i = 0; i < vertices; i++) {
	    const struct aiVector3D *vn = &(mesh->mNormals[i]);
	
	    (*model_normals)[i * 3] = (float)vn->x;
	    (*model_normals)[(i * 3) + 1] = (float)vn->y;
	    (*model_normals)[(i * 3) + 2] = (float)vn->z;
	}
    }

    if (mesh->mFaces != NULL) {
	has_faces = true;
	for (int i = 0; i < faces; i++) {
	    const struct aiFace *face = &(mesh->mFaces[i]);
	    int index1 =  face->mIndices[0];
	    int index2 = face->mIndices[1];
	    int index3 = face->mIndices[2];
	    float dummy = 0;
	    continue;
	}
    }

    
    //what about indices?
    //kinda need to have averaged normals to make the smooth
    //look work

    //turns out there is an mFaces
    //member of the struct

    

    if (&(ply_mesh->mColors[0][0]) != NULL) {
	has_colors = true;
    }
    
    for (int i = 0; i < vertices; i++) {
	if (has_colors) {
	    const struct aiColor4D *vc = &(ply_mesh->mColors[0][i]);//careful here
	    //we could come back here for alphas later
	    (*model_colors)[i * 3] = (float)vc->r;
	    (*model_colors)[(i * 3) + 1] = (float)vc->g;
	    (*model_colors)[(i * 3) + 2] = (float)vc->b;
	} else {
	    (*model_colors)[i * 3] = 0.5f;
	    (*model_colors)[(i * 3) + 1] = 0.5f;
	    (*model_colors)[(i * 3) + 2] = 0.5f;
	}
    }
    

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    if (has_vertices) {
	GLuint vbo_vertices;
	//get sent to the GPU? or is stored in the VAO?

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(float), *model_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	free(*model_vertices);
    }

    if (has_normals) {
	GLuint vbo_normals; 

	glGenBuffers(1, &vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(float), *model_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	free(*model_normals);
    }


    GLuint vbo_colors; 

    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(float), *model_colors, GL_STATIC_DRAW);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    free(*model_colors);


    aiReleaseImport(scene);
    aiReleaseImport(ply_scene);
    
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

    //OpenGL init stuff
    glEnable(GL_DEPTH_TEST);

    
    boolean running = true;

    //note we seem to be drawing upside down
    float vertices[] = {
	//vertex 1, color 1, texture 1 bottom left
	-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	//vertex 2, color 2, texture 2 bottom right
	1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0, 0.0,
	//vertex 3, color 3, texture 3 top left
	-1.0f,   1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0, 1.0,
	//vertex 4, color 4, texture 4 top right
	1.0f,   1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0, 1.0
    };

    //this should be a quad
    //add texture coords to this to render a sprite


    //I think we need some normals
    //then we translate/rotate those?

    int model_vertex_count;
    float *model_vertices = NULL;
    float *model_normals = NULL;
    float *model_colors = NULL;
    GLuint model_vao;
    int model_load_failure;

    // rudy_ply.ply
    model_load_failure = load_model_file("art/rudy_dust.fbx", "art/rudy_ply.ply", &model_vao, &model_vertices, &model_normals, &model_colors, &model_vertex_count);
    if (model_load_failure) {
	printf("Hold on a model didn't load\n");
	return -1;
    }
    
    unsigned int vertex_indices_a[] = {
	0, 1, 2,
	2, 1, 3
    };
    
#if 0
    float tex_coords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.5f, 1.0f//check these against my actual vertices
    };
#endif
    //set aside some space in the GPU and send our vertices to it


    unsigned int shader_program;
    unsigned int basic_shader_program;
    int shader_fault = 0;
    shader_fault = load_shader_program(&shader_program, "test.vert", "test.frag");
    if (shader_fault) {
	printf("your shader has a problem homie\n");
	return -1;
    }
    shader_fault = load_shader_program(&basic_shader_program, "basic.vert", "basic.frag");
    if (shader_fault) {
	printf("your shader has a problem homie\n");
	return -1;
    }




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
    unsigned char *imdata = stbi_load("art/crap_cloud.png", &imwidth, &imheight, &nchannels, 0);

    unsigned int test_texture;
    glGenTextures(1, &test_texture);

    glBindTexture(GL_TEXTURE_2D, test_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imwidth, imheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imdata);
    glGenerateMipmap(GL_TEXTURE_2D);


    //set our sampler uniforms
    int tex1location = glGetUniformLocation(shader_program, "ourTexture");    
    int res_location = glGetUniformLocation(shader_program, "u_resolution");
    int time_ms_location = glGetUniformLocation(shader_program, "time_ms");
    
    
    
    glUniform1i(tex1location, 0);
    

    //int tex2location = glGetUniformLocation(shader_program, "ourTexture2");
    //glUniform1i(tex2location, 1);

    
    
    
    stbi_image_free(imdata);

    SDL_GL_SetSwapInterval(0);//using vsync seems to be kinda shitty?

   




    Vec3 camera = vec3_init(0.0f, 0.0f, 0.0f);
    Vec3 triangle_pos = vec3_init(0.0f, 0.0f, 0.0f);
    int transform_loc = glGetUniformLocation(shader_program, "transform");
    int basic_transform_loc = glGetUniformLocation(basic_shader_program, "transform");
    int basic_perspective_loc = glGetUniformLocation(basic_shader_program, "perspective");
    
    

    float rot_angle = 0.0f;

    Mat4 perspective = mat4_create_perspective(60.0f, 1280.0/720.0f, 0.1f, 100.0f);
    

    //
    float angle_x = 0.0f;
    float angle_y = 0.0f;
    float angle_z = 0.0f;


    float resolution[2] = {(float)SCREENWIDTH, (float)SCREENHEIGHT};
    glUniform2f(res_location, resolution[0], resolution[1]); 
    
    while (running) {
	current_time = SDL_GetTicks();
	float current_time_ms = current_time/1000.0f;
	time_elapsed = current_time - last_time;
	last_time = current_time;	
	float dt = time_elapsed/1000.0f;//dt is inconsistent, should be noted

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP]) {

	    camera.y += dt;

	}
	if (keys[SDL_SCANCODE_DOWN]) {

	    camera.y -= dt;

	}
	if (keys[SDL_SCANCODE_LEFT]) {
	    camera.x -= dt;
	}
	if (keys[SDL_SCANCODE_RIGHT]) {
	    camera.x += dt;
	}

	if (keys[SDL_SCANCODE_SPACE]) {
	    camera.z += dt;//does eventually disappear on horizon
	}
	if (keys[SDL_SCANCODE_LCTRL]) {
	    camera.z -= dt;//does eventually disappear on horizon
	}

	if (keys[SDL_SCANCODE_W]) {

	    triangle_pos.y -= dt;

	}
	if (keys[SDL_SCANCODE_S]) {

	    triangle_pos.y += dt;

	}
	if (keys[SDL_SCANCODE_A]) {
	    triangle_pos.x += dt;
	}
	if (keys[SDL_SCANCODE_D]) {
	    triangle_pos.x -= dt;
	}

	if (keys[SDL_SCANCODE_Q]) {
	    angle_x += 100*dt;
	}
	if (keys[SDL_SCANCODE_E]) {
	    angle_x -= 100*dt;
	}

	if (keys[SDL_SCANCODE_Z]) {
	    angle_y += 100*dt;
	}
	if (keys[SDL_SCANCODE_X]) {
	    angle_y -= 100*dt;
	}

	//printf("x rotation is: %f degrees \n", angle_x);
	//printf("Camera x, y, z is : %f %f %f \n", camera.x, camera.y, camera.z);
	//printf("triangle x, y, z is : %f %f %f \n", triangle_pos.x, triangle_pos.y, triangle_pos.z);
	




	

	
	
	
 	

	
	total_time_ms += dt;
	

	float green_value = (sin(total_time_ms)/2.0f) + 0.5f;
	float red_value = (sin(total_time_ms/2.0f)/2.0f) + 0.5f;
	//int vertex_color_location = glGetUniformLocation(shader_program, "our_color");
	int offset_location = glGetUniformLocation(shader_program, "offset");
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_QUIT) {
		running = false;
		break;
	    }
	}

	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
	glClearColor(0.01f, 0.01f, 0.15f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//angle_x, sky_shader, sky_vao, sky_texture, transform_loc, time_ms_loc,
	//current_time_ms, perspective

	
	//draw_sky(angle_x, shader_program, test_vaos[0], test_texture, transform_loc, time_ms_location, current_time_ms, perspective);
	//glUseProgram(basic_shader_program);
	//glUniformMatrix4fv(basic_transform_loc, 1, GL_FALSE, trans_rot_out);
	//glBindVertexArray(model_vao);
	//glDrawArrays(GL_TRIANGLES, 0, model_vertex_count);
	
	draw_object(basic_shader_program, model_vao, model_vertex_count, triangle_pos, camera, perspective, basic_transform_loc, basic_perspective_loc, angle_x, angle_y);


	SDL_GL_SwapWindow(window);
    }
    
    
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
    return 0;
}
