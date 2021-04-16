#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//todo: geometric algebra support (rotors bivectors etc)

//todo: openAL for sound

//todo: clouds. start with a texture and change it some ways?
//or use noise to make the clouds?
//done: basic alpha textures via discard (this doesn't allow varying transparency though
//note: we could have moving clouds inside our 'skybox' by translating them
//but not according to player movement. i guess that's how they're done
//in the old Unreal engine games?


//wrap some of the drawing code -semidone

//todo: backface culling or discard or whatever






//TODO: animation system...
//getting close
//theres one issues
//I can't get blender to export the Rudy animation probably (this is a Blender knowledge issue)
//I may have to re-rig it or something, since I winged it first time through

//PRIORITY
//todo:mouselook
//ok got this working with basic angle and rotations,
//will shift later to quaternions I think
//todo: editor (a level is just vertices and shit right?)
//(eventually via quaternions??)
//some basic refactoring/cleanup

//TODAY:
//-start making a LEVEL
//make a skybox
//make better ground

//call create ground, load ground, load associated shader

//sky box as a shader
//render quads with gradients for 


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
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

#define DEBUG_FLYTHROUGH 1

#define PARTICLE_COUNT 300

#define MAX_BONES 32 //enough?




//todo: clean up shader boilerplate

typedef unsigned int boolean;
typedef unsigned int uint;

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

typedef struct {
    float *vertices;
    float *normals;
    float *colors;
    Mat4 *bone_offset_matrices;
} Model_Info;

typedef struct Skeleton_Node_t {
    struct Skeleton_Node_t *children[MAX_BONES];

    Vec3 *position_keys;
    Quaternion *rotation_keys;    //technically these are versors
    Vec3 *scale_keys;
    
    float *position_key_times;
    float *rotation_key_times;
    float *scale_key_times;

    int num_position_keys;
    int num_rotation_keys;
    int num_scale_keys;
    
    //existing data
    char name[64];
    int num_children;
    int bone_index;
} Skeleton_Node;

typedef struct {
    Vec3 position;
    //should these be a quaternion?
    float angle_x;
    float angle_y;
    float angle_z;
} Camera;

float *create_ground_grid(int width, int length, float height, float scale, int *size_of_ground)
{
    //return an array of x,y,z co-ords for some ground
    //kinda like constructing a mesh
    //we'll start 'flat', in the sense of making a 2D grid with various heights
    //each point has 3 components
    //we'll need normals later -> this will just take pointers and fill them out
    //rather than returning ground

    //we actually need to make triangles, not just vertices
    *size_of_ground =  3 * length * width * sizeof(float);
    float *ground = (float*)malloc(*size_of_ground);
    //what we actually need to do...
    // the grid
    // * * * *
    // * * * *
    

    //this just makes a level 2d grid
    for (int i = 0; i < length; i++) {
	for (int j = 0; j < width * 3 ; j+=3) {
	    
	    //maybe draw this out to check
	    float x = (float) j / (float) (width * 3);
	    float y = height * sin(i + j);
	    float z = (float) i / (float) length;;
	    //printf("insetring %f, %f, %f\n", x, y, z);
	    //printf("at indices %d, %d, %d", (i * width*3) + j, (i * width*3) + j + 1, (i * width*3) + j + 2);
	    ground[(i * width*3) + j] = x * scale;
	    ground[(i * width*3) + j + 1] = y;
	    ground[(i * width*3) + j + 2] = z * scale;
	}
    }
    
    return ground;
}

uint *create_ground_indices(int width, int length, int *size_of_elements, int *amount_of_elements)
{

    //a1....a2/b1
    //.   .  .
    //. .    .
    //..     .
    //.      .
    //a3/b3..b2
    *size_of_elements = (width-1) * (length-1) * 6 * sizeof(uint);
    *amount_of_elements = (width-1) * (length-1) * 6;
    uint *indices = (uint*)malloc(*size_of_elements);
    #if 0
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 1;
    indices[4] = 3;
    indices[5] = 2;
    #endif
    #if 1
    for (int i = 0; i < (length - 1); i++) {
	for (int j = 0; j < ((width - 1) ); j++) {
	    int a1 = (i * width) + j;
	    int a2 = (i * width) + j + 1;
	    int a3 = ((i+1) * width) + j;

	    int b1 = (i * width) + j + 1;
	    int b2 = ((i+1) * width) + j + 1;
	    int b3 = ((i+1) * width) + j;
	    indices[(i * (width-1)*6) + j*6] = a1;
	    indices[(i * (width-1)*6) + j*6 + 1] = a2;
	    indices[(i * (width-1)*6) + j*6 + 2] = a3;

	    indices[(i * (width-1)*6) + j*6 + 3] = b1;
	    indices[(i * (width-1)*6) + j*6 + 4] = b2;
	    indices[(i * (width-1)*6) + j*6 + 5] = b3;
	}
    }
    #endif
    return indices;
}

uint load_ground(float *ground, uint *elements, int size_of_ground, int size_of_elements)
{
    //use GLuint?
    uint vao;
    uint vbo;
    uint ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size_of_ground, ground, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_of_elements, elements, GL_STATIC_DRAW);
    

    //do we have to unbind?
    //may as well
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    

    return vao;
}

boolean import_skeleton_node(struct aiNode *assimp_node, Skeleton_Node **skeleton_node, int bone_count, char bone_names[][64])//this might be cpp nonsense
{
    //function will work like this:
    //we give it the assimp root node (scene)
    //a null-pointer to skeleton root node (allocate internally)
    //and total bone count

    Skeleton_Node *temp = (Skeleton_Node*)malloc(sizeof(Skeleton_Node));

    strcpy(temp->name, assimp_node->mName.data);//this may fuck us up

    //printf("-node name = %s\n", temp->name);
    temp->num_children = 0;
    //bunch of other stuff to init
    temp->position_keys = NULL;
    temp->rotation_keys = NULL;
    temp->scale_keys = NULL;

    temp->position_key_times = NULL;
    temp->rotation_key_times = NULL;
    temp->scale_key_times = NULL;

    temp->num_position_keys = 0;
    temp->num_rotation_keys = 0;
    temp->num_scale_keys = 0;
    
    
    //printf("node has %d children\n", (int)assimp_node->mNumChildren);
    temp->bone_index = -1;
    for (int i = 0; i < MAX_BONES; i++) {
	temp->children[i] = NULL;//init to zero
    }
    boolean has_bone = false;
    for (int i = 0; i < bone_count; i++) {
	if (strcmp(bone_names[i], temp->name) == 0) {
	    //printf("node uses bone %i\n", i);
	    temp->bone_index = i;
	    has_bone = true;
	    break;
	}
    }

    if (!has_bone) {
	//printf("No bone found\n");
    } else {
	//printf("found a BONE, ey?\n");
    }

    boolean has_useful_child = false;

    for (int i = 0; i < (int)assimp_node->mNumChildren; i++) {
	if (import_skeleton_node(assimp_node->mChildren[i],
				 &temp->children[temp->num_children],
				 bone_count,
				 bone_names)) {
	    has_useful_child = true;
	    temp->num_children++;
	} else {
	    //printf("useless child culled\n");
	}
    }

    if (has_useful_child || has_bone) {
	*skeleton_node = temp;
	return true;
    }

    free(temp);
    temp = NULL; //why do this
    return false;
}

Mat4 mat4_from_assimp_mat4(struct aiMatrix4x4 m)
{
    Mat4 result;
    //meta-programming would make this way less painful
    //column major?

    //THIS MATRIX MAY HAVE ROTATION DATA THAT WE DON'T WANT
    mat4(result, 0, 0) = m.a1;
    mat4(result, 0, 1) = m.b1;
    mat4(result, 0, 2) = m.c1;
    mat4(result, 0, 3) = m.d1;

    mat4(result, 1, 0) = m.a2;
    mat4(result, 1, 1) = m.b2;
    mat4(result, 1, 2) = m.c2;
    mat4(result, 1, 3) = m.d2;

    mat4(result, 2, 0) = m.a3;
    mat4(result, 2, 1) = m.b3;
    mat4(result, 2, 2) = m.c3;
    mat4(result, 2, 3) = m.d3;

    mat4(result, 3, 0) = m.a4;
    mat4(result, 3, 1) = m.b4;
    mat4(result, 3, 2) = m.c4;
    mat4(result, 3, 3) = m.d4;

    return result;
}

Mat4 mat4_trans_from_assimp_mat4(struct aiMatrix4x4 m)
{
    Mat4 result;
    //meta-programming would make this way less painful
    //column major?

    //THIS MATRIX MAY HAVE ROTATION DATA THAT WE DON'T WANT
    mat4(result, 0, 0) = 1.0f;
    mat4(result, 0, 1) = 0.0f;
    mat4(result, 0, 2) = 0.0f;
    mat4(result, 0, 3) = 0.0f;

    mat4(result, 1, 0) = 0.0f;
    mat4(result, 1, 1) = 1.0f;
    mat4(result, 1, 2) = 0.0f;
    mat4(result, 1, 3) = 0.0f;

    mat4(result, 2, 0) = 0.0f;
    mat4(result, 2, 1) = 0.0f;
    mat4(result, 2, 2) = 1.0f;
    mat4(result, 2, 3) = 0.0f;

    mat4(result, 3, 0) = m.a4;
    mat4(result, 3, 1) = m.b4;
    mat4(result, 3, 2) = m.c4;
    mat4(result, 3, 3) = m.d4;

    return result;
}

Skeleton_Node *find_node_in_skeleton(Skeleton_Node *root, const char *node_name)
{
    //this should be an assert
    assert(root);

    if (strcmp(node_name, root->name) == 0) {
	printf("found the root\n");
	return root;
    }

    //recurse to children
    for (int i = 0; i < root->num_children; i++) {
	Skeleton_Node *child = find_node_in_skeleton(root->children[i], node_name);
	if (child != NULL) {
	    printf("found a child\n");
	    return child;
	}
    }
    return NULL;
}
    

void skeleton_animate(Skeleton_Node *node, float animation_time, Mat4 parent_matrix, Mat4 *bone_offset_matrices,
    Mat4 *bone_animation_matrices)
{
    //I *think* maybe
    //the issue is with the timers
    assert(node);

    Mat4 our_matrix = parent_matrix;

    Mat4 local_animation = mat4_create_identity();

    Mat4 node_translation = mat4_create_identity();
    Mat4 node_rotation = mat4_create_identity();
    Mat4 node_scale = mat4_create_identity();
    if (node->num_position_keys > 0) {
	int prev_key = 0;
	int next_key = 0;
	//careful of the off-by-one trap
	for (int i = 0; i < node->num_position_keys - 1; i++) {
	    prev_key = i;
	    next_key = i + 1;
	    if (node->position_key_times[next_key] >= animation_time) {
		//printf("doing no translation\n");
		break;
	    }
	}
	float total_time = node->position_key_times[next_key] - node->position_key_times[prev_key];
	float frame_time = (animation_time - node->position_key_times[prev_key]) / total_time;
	if (frame_time > 1.0f || frame_time < 0.0f) {
	    printf("we have %f translation t\n", frame_time);
	}
	//translation
	Vec3 v_initial = node->position_keys[prev_key];
	Vec3 v_final = node->position_keys[next_key];
	Vec3 v_lerp = vec3_add(vec3_scale(v_initial, 1.0f - frame_time), vec3_scale(v_final,frame_time));
	//I shouldn't assume this is all working properly...
	//maybe check this
	node_translation = mat4_create_translation(v_lerp);
	//node_translation = mat4_transpose(node_translation);
	//node_translation = mat4_create_identity();

	
	
    }
    if (node->num_rotation_keys > 0) {
	int prev_key = 0;
	int next_key = 0;
	for (int i = 0; i < node->num_rotation_keys - 1; i++) {
	    prev_key = i;
	    next_key = i + 1;
	    if (node->rotation_key_times[next_key] >= animation_time) {
		printf("no rotation because animation time was %f and next key time was %f\n", animation_time, node->rotation_key_times[next_key]);
		break;

	    }
	}
	printf("doing rotating\n");
	float total_time = node->rotation_key_times[next_key] - node->rotation_key_times[prev_key];
	float frame_time = (animation_time - node->rotation_key_times[prev_key]) / total_time;
	Quaternion q_initial = node->rotation_keys[prev_key];
	//float check_init_mag = quaternion_mag(q_initial);
	Quaternion q_final = node->rotation_keys[next_key];
	//float check_final_mag = quaternion_mag(q_final);
	if (frame_time < 0.0f) {
	    printf("we have %f translation t\n", frame_time);
	}
	Quaternion q_lerp = quaternion_slerp(q_initial, q_final, frame_time);
	node_rotation = mat4_from_quaternion(q_lerp);
	//node_rotation = mat4_transpose(node_rotation);
	//node_rotation = mat4_create_identity();
    }


    if (node->num_scale_keys > 0) {
	int prev_key = 0;
	int next_key = 0;
	//careful of the off-by-one trap
	for (int i = 0; i < node->num_scale_keys - 1; i++) {
	    prev_key = i;
	    next_key = i + 1;
	    if (node->scale_key_times[next_key] >= animation_time) {
		break;
	    }
	}
	float total_time = node->scale_key_times[next_key] - node->scale_key_times[prev_key];
	float frame_time = (animation_time - node->scale_key_times[prev_key]) / total_time;
	//translation
	Vec3 v_initial = node->scale_keys[prev_key];
	Vec3 v_final = node->scale_keys[next_key];
	Vec3 v_lerp = vec3_add(vec3_scale(v_initial, 1.0f - frame_time), vec3_scale(v_final,frame_time));
	//I shouldn't assume this is all working properly...
	//maybe check this
	node_scale = mat4_create_scale_from_vec(v_lerp);
	//node_translation = mat4_transpose(node_translation);
	//node_translation = mat4_create_identity();
	
    }

    //later add scaling here?

    node_scale = mat4_create_identity();
    local_animation = mat4_mult(node_scale, mat4_mult(node_translation, node_rotation));
    
    int bone_index = node->bone_index;

    //where do we change bone animation matrices??
    
    if (bone_index > -1) {
	Mat4 bone_offset = bone_offset_matrices[bone_index];
	bone_offset = mat4_transpose(bone_offset);
	//Mat4 bone_offset = mat4_create_identity();
	Mat4 inv_bone_offset = mat4_inverse(bone_offset);
	our_matrix = mat4_mult(parent_matrix, local_animation);
	Mat4 matrix_to_add = mat4_mult(mat4_mult(parent_matrix, local_animation), mat4_mult(bone_offset, mat4_create_identity()));
	bone_animation_matrices[bone_index] = matrix_to_add;
    } else {
	//printf("animation did nothing to thsi node because bone index was %d\n", bone_index);
	//our_matrix = parent_matrix;
	//bone_animation_matrices[bone_index] = our_matrix;
    }

    for (int i = 0; i < node->num_children; i++) {
	skeleton_animate(node->children[i], animation_time, our_matrix, bone_offset_matrices, bone_animation_matrices);
    }
}

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

void draw_animated_object(int model_shader, int model_vao, int vertex_count, Vec3 pos, Vec3 camera, Mat4 perspective, Mat4 *bone_animation_matrices, int transform_loc, int perspective_loc, int *bone_matrices_locations, float model_angle_x, float model_angle_y, float camera_angle_x, float camera_angle_y, float animation_angle_x, float animation_angle_y, float animation_angle_z)
{
    //so in this case I think we want to
    //rotate THEN translate?
    //translate to 0, -0.94, -0.94 I think
    //rotation of like...-2.17 seems about right, but that's probably irrelevant?
    //need to remember to restrict the players ability to rubberneck

    //pretty much got this down now


    
    //create rotation matrix and transpose it
    Mat3 model_rotate_x = mat3_create_rotate_x(model_angle_x);

    Mat3 model_rotate_y = mat3_create_rotate_y(model_angle_y);

    //Mat3 animation_rotate_x = mat3_create_rotate_x(

    Mat3 camera_rotate_x = mat3_create_rotate_x(camera_angle_x);
    Mat3 camera_rotate_y = mat3_create_rotate_y(camera_angle_y);

    Mat3 animation_rotate_x = mat3_create_rotate_x(animation_angle_x);
    Mat3 animation_rotate_y = mat3_create_rotate_y(animation_angle_y);

    //multiply rotation by translation (i.e rotate then translate)
    //mat4_mult(rotate_x_transpose, tmp_translate_transpose, rotate_translate);
    //then multiply by perspective matrix
    Mat3 model_rotate_x_y = mat3_mult(model_rotate_y, model_rotate_x);

    Mat3 animation_rotate_x_y = mat3_mult(animation_rotate_x, animation_rotate_y);

    //THIS was acting weird
    Mat4 camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    Mat4 animation_rotation = mat4_from_mat3(animation_rotate_x_y);
    //order?
    //
    Mat4 model_rotation = mat4_from_mat3(model_rotate_x_y);

    Mat4 model_translation = mat4_create_translation(pos);

    Mat4 mrt = mat4_mult(model_translation, model_rotation);
    //Mat4 transformation = mat4_create_translation_rotation(rotate_x_y, camera);

    //ORDER matters here for an FPS, it's translate THEN rotate
    //which means mult rotate*translate
    Mat4 camera_transformation = mat4_create_translation(camera);
    camera_transformation = mat4_mult(camera_rotate, camera_transformation);

    Mat4 transformation = mat4_mult(camera_transformation, mrt);
    
    //and send to the shader
    glUseProgram(model_shader);
    //glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transformation.elements);

    ///it'sss wooooorking

    int bone_number = 1;
    
    //this rotated everything, which we don't want

    Mat4 inverse_offset = mat4_inverse(bone_animation_matrices[bone_number]);

    //Mat4 animation_matrix = mat4_mult(mat4_mult(bone_animation_matrices[bone_number], animation_rotation), inverse_offset);
    #if 0
    glUniformMatrix4fv(bone_matrices_locations[3], 1, GL_FALSE, bone_animation_matrices[3].elements);
#endif

	#if 1
    for (int i = 0; i < MAX_BONES; i++) {
	
	glUniformMatrix4fv(bone_matrices_locations[i], 1, GL_FALSE, bone_animation_matrices[i].elements);
    }
#endif
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transformation.elements);
    glUniformMatrix4fv(perspective_loc, 1, GL_FALSE, perspective.elements);

    glBindVertexArray(model_vao);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    
}

void draw_particles(int particle_shader, int particle_vao, int particle_count, float current_time_ms, Vec3 emitter_pos, Vec3 camera, Mat4 perspective, int transform_loc, int perspective_loc, int particle_time_loc, int particle_emitter_loc)
{
    
    Mat4 camera_transformation = mat4_create_translation(camera);

    
    //and send to the shader
    glUseProgram(particle_shader);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, camera_transformation.elements);
    glUniformMatrix4fv(perspective_loc, 1, GL_FALSE, perspective.elements);

    
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform1f(particle_time_loc, current_time_ms);
    //more uniforms to go

    glUniform3f(particle_emitter_loc, emitter_pos.x, emitter_pos.y, emitter_pos.z);

	
    glBindVertexArray(particle_vao);
    glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);

}

void draw_quad_model()
{
}

void draw_skybox(uint shader, uint vao, float camera_angle_x, float camera_angle_y, float camera_angle_z, Vec3 camera, int transform_loc, int perspective_loc, Mat4 perspective)
{
    //basically the idea is to make a rotated quad, then translate back, to some constant
    // 'horizon' distance, and render there
    //the distance should be large enough that distant objects won't have their z-ordering
    //messed up
    //ideally we'd do an infinite draw distance,
    //which we *can* do, but we'll just need to adjust our perspective matrix for that

    //the rotate/translate deserves an explanation as
    //it's a little weird
    Mat3 camera_rotate_x = mat3_create_rotate_x(camera_angle_x);
    Mat3 camera_rotate_y = mat3_create_rotate_y(camera_angle_y);

    Mat4 camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    Mat4 camera_translate = mat4_create_translation(vec3_init(0.0f, 0.0f, 100.0f));

    
    
    Mat4 skybox_transformation = mat4_mult(camera_rotate, camera_translate);

    glUseProgram(shader);

    int side_loc = glGetUniformLocation(shader, "side");
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, skybox_transformation.elements);
    glUniformMatrix4fv(perspective_loc, 1, GL_FALSE, perspective.elements);

    glUniform1i(side_loc, 1);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //now draw another side

    
    glUniform1i(side_loc, 2);
    camera_rotate_y = mat3_create_rotate_y(camera_angle_y + 90.0f);
    

    camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    camera_translate = mat4_create_translation(vec3_init(0.0f, 0.0f, 100.0f));
    skybox_transformation = mat4_mult(camera_rotate, camera_translate);
    
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, skybox_transformation.elements);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(side_loc, 3);
    camera_rotate_y = mat3_create_rotate_y(camera_angle_y + 180.0f);
    
    camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    camera_translate = mat4_create_translation(vec3_init(0.0f, 0.0f, 100.0f));
    
    skybox_transformation = mat4_mult(camera_rotate, camera_translate);
    
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, skybox_transformation.elements);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(side_loc, 4);

    
    camera_rotate_y = mat3_create_rotate_y(camera_angle_y + 270);
    
    camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    camera_translate = mat4_create_translation(vec3_init(0.0f, 0.0f, 100.0f));
    

    skybox_transformation = mat4_mult(camera_rotate, camera_translate);
    
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, skybox_transformation.elements);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    //this one is a puzzle
    //basically you have to consider that once you rotate,
    //your new translation is relative to new axis
    //so pushing along z actually pushes us up y!
    //in all these cases we push along z, but
    //it's a 'new' z (change of basis)
    glUniform1i(side_loc, 5);
    //camera_rotate_y = mat3_create_rotate_y(camera_angle_y);
    Mat3 camera_rotate_z = mat3_create_rotate_z(camera_angle_y);
    camera_rotate_y = mat3_create_identity();
    camera_rotate_x = mat3_create_rotate_x(camera_angle_x - 90.0f);
    camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_z));

    camera_translate = mat4_create_translation(vec3_init(0.0f, 0.0f, 100.0f));
    
    
    skybox_transformation = mat4_mult(camera_rotate, camera_translate);
    
    
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, skybox_transformation.elements);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    
}

void draw_ground(int quad_shader, int quad_vao, int element_count, float camera_angle_x, float camera_angle_y, float camera_angle_z, Vec3 ground_pos, Vec3 camera, int quad_texture, int quad_texture_loc, int transform_loc, int perspective_loc, Mat4 perspective)
{
    Mat3 camera_rotate_x = mat3_create_rotate_x(camera_angle_x);
    Mat3 camera_rotate_y = mat3_create_rotate_y(camera_angle_y);

    Mat4 camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    float ground_rotate_x = 90.0f;
    float ground_rotate_y = 0.0f;

    Mat3 model_rotate_x = mat3_create_rotate_x(ground_rotate_x);
    Mat3 model_rotate_y = mat3_create_rotate_y(ground_rotate_y);
    Mat3 model_rotate_x_y = mat3_mult(model_rotate_x, model_rotate_y);
    Mat4 model_rotation = mat4_from_mat3(model_rotate_x_y);
    Mat4 model_translation = mat4_create_translation(ground_pos);
    Mat4 model_scale = mat4_create_xyz_scale(100.0f, 1.0f, 100.0f);
    //model_translation = mat4_embedded_scale(model_translation, 10.0f);
    //we COULD scale here?
    Mat4 mrt = mat4_mult(mat4_mult(model_scale, model_translation), model_rotation);
    //Mat4 mrt = mat4_mult(model_translation, model_rotation);

    Mat4 camera_transformation = mat4_create_translation(camera);
    camera_transformation = mat4_mult(camera_rotate, camera_transformation);

    //
    Mat4 transformation = mat4_mult(camera_transformation, mrt);
    //transformation = mat4_scale(transformation, 0.5f);

    glUseProgram(quad_shader);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transformation.elements);
    glUniformMatrix4fv(perspective_loc, 1, GL_FALSE, perspective.elements);
    
    //how do we know it's texture 0?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);
    
    
    glBindVertexArray(quad_vao);//this also binds the EBO
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
}

void draw_untextured_ground(int quad_shader, int quad_vao, int element_count, float camera_angle_x, float camera_angle_y, float camera_angle_z, Vec3 ground_pos, Vec3 camera, int transform_loc, int perspective_loc, Mat4 perspective)
{
    Mat3 camera_rotate_x = mat3_create_rotate_x(camera_angle_x);
    Mat3 camera_rotate_y = mat3_create_rotate_y(camera_angle_y);

    Mat4 camera_rotate = mat4_from_mat3(mat3_mult(camera_rotate_x, camera_rotate_y));

    float ground_rotate_x = 0.0f;
    float ground_rotate_y = 0.0f;

    Mat3 model_rotate_x = mat3_create_rotate_x(ground_rotate_x);
    Mat3 model_rotate_y = mat3_create_rotate_y(ground_rotate_y);
    Mat3 model_rotate_x_y = mat3_mult(model_rotate_x, model_rotate_y);
    Mat4 model_rotation = mat4_from_mat3(model_rotate_x_y);
    Mat4 model_translation = mat4_create_translation(ground_pos);
    //Mat4 model_scale = mat4_create_xyz_scale(100.0f, 1.0f, 100.0f);
    //model_translation = mat4_embedded_scale(model_translation, 10.0f);
    //we COULD scale here?
    Mat4 mrt = mat4_mult(model_translation, model_rotation);
    //Mat4 mrt = mat4_mult(model_translation, model_rotation);

    Mat4 camera_transformation = mat4_create_translation(camera);
    camera_transformation = mat4_mult(camera_rotate, camera_transformation);

    //
    Mat4 transformation = mat4_mult(camera_transformation, mrt);
    //transformation = mat4_scale(transformation, 0.5f);

    glUseProgram(quad_shader);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transformation.elements);
    glUniformMatrix4fv(perspective_loc, 1, GL_FALSE, perspective.elements);
    
    //how do we know it's texture 0?


    
    
    glBindVertexArray(quad_vao);
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
}

void draw_quad(float angle_x, float angle_y, float angle_z, Vec3 camera, int quad_shader, int quad_vao, int quad_texture, int transform_loc, int perspective_loc, Mat4 perspective)
{
    //how do we know it's texture 0?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);
    
    glUseProgram(quad_shader);
    //glUniform1f(time_ms_loc, current_time_ms);
    //glUniform4f(vertex_color_location, 0.3f, green_value, 0.3f, 1.0f);
    //glUniform3f(offset_location, green_value, red_value, 0.0f);

    //glUniformMatrix4fv(transform_loc, 1, GL_FALSE, rot_trans_out);
    glBindVertexArray(quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

int load_model_file(char *file_name, char *ply_file_name, GLuint *vao, float **model_vertices, float **model_normals, float **model_colors, Mat4 **bone_offset_matrices, GLint **bone_ids, int *model_vertex_count, int *bone_count, Skeleton_Node **skeleton_root_node, double *animation_duration)
{
    //VERY dumb workaround for colors:
    //pull the colors from the ply file
    //pull the vertices and normals from the
    //fbx file

    //what about the bones?? also from the fbx file?

    //maybe faceplam question: why are these NULL pointers valid
    //when I assign them values via a de-reference?

    //a: it's not de-referencing it per-se
    //when you have e.g *mesh, that's de-referencing
    //the **, and we passed in **, so it
    //is actually just setting the pointer
    //which exists outside this scope

    
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

    //it seems like the fbx one
    //contains the bones
    const struct aiMesh *mesh = scene->mMeshes[0];//fbx one
    const struct aiMesh *ply_mesh = ply_scene->mMeshes[0];
    int animations = scene->mNumAnimations;
    printf("%d vertices in the mesh\n", mesh->mNumVertices);
    int vertices = mesh->mNumVertices;
    int faces = mesh->mNumFaces;
    int bones = mesh->mNumBones;
    
    (*bone_count) = bones;

    boolean has_vertices = false;
    boolean has_normals = false;
    boolean has_faces = false;
    boolean has_colors = false;
    boolean has_bones = false;

    
    
    
    *model_vertex_count = vertices;
    //should these be conditional? probably
    (*model_vertices) = (float*)malloc(sizeof(float) * vertices * 3);
    (*model_normals) = (float*)malloc(sizeof(float) * vertices * 3);
    (*model_colors) = (float*)malloc(sizeof(float) * vertices * 3);
    (*bone_offset_matrices) = (Mat4*)malloc(sizeof(Mat4) * bones);

    for (int i = 0; i < bones; i++) {
	(*bone_offset_matrices)[i] = mat4_create_identity();
    }




    if (bones != 0) {
	has_bones = true;
	(*bone_ids) = (int*)malloc(sizeof(int) * vertices);//because each vertex has a bone id
	//max 256 bones
	//max 64 chars in length
	char bone_names[256][64];

	
	for (int i = 0; i < bones; i++) {
	    //mName
	    //mNumweights
	    //mWeights
	    //mOffsetMatrix

	    //we want ids / names?
	    //weights
	    const struct aiBone *bone = mesh->mBones[i];

	    strcpy(bone_names[i], bone->mName.data);
	    printf("Bone names %i is %s\n", i, bone_names[i]); 

	    (*bone_offset_matrices)[i] = mat4_trans_from_assimp_mat4(bone->mOffsetMatrix);

	    //get weights here later

	    uint num_weights = bone->mNumWeights;

	    //
	    for (int j = 0; j < num_weights; j++) {
		struct aiVertexWeight weight = bone->mWeights[j];
		int vertex_id = (int)weight.mVertexId;
		if (weight.mWeight >= 0.5f) {
		    (*bone_ids)[vertex_id] = i;//is this the bone ID? prolly
		}
	    }
	    

	}
	//get the root node
	struct aiNode *assimp_node = scene->mRootNode;

	if (!import_skeleton_node(assimp_node, skeleton_root_node, bones, bone_names)) {
	    //handle the error
	    printf("Skeleton loading didn't work homie\n");
	}

    }



    if (animations > 0) {
	struct aiAnimation *animation = scene->mAnimations[0];
	*animation_duration = animation->mDuration;
	/* we will get keys here */
	for (int i = 0; i < (int)animation->mNumChannels; i++) {
	    struct aiNodeAnim *channel = animation->mChannels[i];
	    Skeleton_Node *node = find_node_in_skeleton(*skeleton_root_node, channel->mNodeName.data);
	    assert(node);

	    //mallocs
	    node->num_position_keys = channel->mNumPositionKeys;
	    node->num_rotation_keys = channel->mNumRotationKeys;
	    node->num_scale_keys = channel->mNumScalingKeys;

	    node->position_key_times = (float*)malloc(sizeof(float) * node->num_position_keys);
	    node->rotation_key_times = (float*)malloc(sizeof(float) * node->num_rotation_keys);
	    node->scale_key_times = (float*)malloc(sizeof(float) * node->num_scale_keys);

	    node->position_keys = (Vec3*)malloc(sizeof(Vec3) * node->num_position_keys);
	    node->rotation_keys = (Quaternion*)malloc(sizeof(Quaternion) * node->num_rotation_keys);
	    node->scale_keys = (Vec3*)malloc(sizeof(Vec3) * node->num_scale_keys);

	    //add position keys
	    for (int j = 0; j < node->num_position_keys; j++) {
		struct aiVectorKey key = channel->mPositionKeys[j];
		node->position_keys[j].x = key.mValue.x;
		node->position_keys[j].y = key.mValue.y;
		node->position_keys[j].z = key.mValue.z;
		node->position_key_times[j] = key.mTime;
	    }
	    //add rotation keys
	    for (int j = 0; j < node->num_rotation_keys; j++) {
		struct aiQuatKey key = channel->mRotationKeys[j];
		node->rotation_keys[j].w = key.mValue.w;
		node->rotation_keys[j].x = key.mValue.x;
		node->rotation_keys[j].y = key.mValue.y;
		node->rotation_keys[j].z = key.mValue.z;
		node->rotation_key_times[j] = key.mTime;
	    }
	    for (int j = 0; j < node->num_scale_keys; j++) {
		struct aiVectorKey key = channel->mScalingKeys[j];
		node->scale_keys[j].x = key.mValue.x;
		node->scale_keys[j].y = key.mValue.y;
		node->scale_keys[j].z = key.mValue.z;
		node->scale_key_times[j] = key.mTime;
		
	    }
	}

	
	
    }
    
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

    if (has_bones) {
	GLuint vbo_bone_ids;
	glGenBuffers(1, &vbo_bone_ids);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_bone_ids);
	
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(GLint), *bone_ids, GL_STATIC_DRAW);

	glVertexAttribIPointer(3, 1, GL_INT, 0, (void*)0);//this is different!
	glEnableVertexAttribArray(3);
	free(*bone_ids);
    }

    if (has_colors) {
	GLuint vbo_colors; 

	glGenBuffers(1, &vbo_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(float), *model_colors, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	free(*model_colors);
    }

    aiReleaseImport(scene);
    aiReleaseImport(ply_scene);
    
    return 0;
}

int load_points(GLuint *vao, Mat4 *bone_matrices, int bone_count)
{
    float *points;

    points = (float*)malloc(sizeof(float) * bone_count * 3);

    //extract the poitns from the bone matrices here?
    for (int i = 0; i < bone_count; i++) {
	//check this
	Mat4 m = bone_matrices[i];
	//want the x y z translation componenents
	float x = mat4(m, 3, 0);
	float y = mat4(m, 3, 1);
	float z = mat4(m, 3, 2);
	//something off here?
	//or are 'bone positions'
	//related to the pose?
	points[i*3] = -x;
	points[i*3 + 1] = -y;
	points[i*3 + 2] = -z;
    }
    //

    GLuint vbo_points;
    glGenBuffers(1, &vbo_points);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
    glBufferData(GL_ARRAY_BUFFER, 3 * bone_count * sizeof(float), points, GL_STATIC_DRAW);

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    free(points);

    return 0;
}

int generate_particles(GLuint *vao)
{
    float vertex_velocities[PARTICLE_COUNT * 3];
    float vertex_times[PARTICLE_COUNT];

    float time_accumulator = 0.0f;

    int j = 0;

    for (int i = 0; i < PARTICLE_COUNT; i++) {
	vertex_times[i] = time_accumulator;
	time_accumulator += 0.01f;

	float x_vel = ((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f; //uhh...
	float y_vel = 1.0f;
	float z_vel = ((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f; //uhh...

	vertex_velocities[j] = x_vel;
	vertex_velocities[j + 1] = y_vel;
	vertex_velocities[j + 2] = z_vel;
	
	j += 3;
    }

    GLuint velocity_vbo;
    glGenBuffers(1, &velocity_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, velocity_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_velocities), vertex_velocities, GL_STATIC_DRAW);

    GLuint time_vbo;
    glGenBuffers(1, &time_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_times), vertex_times, GL_STATIC_DRAW);

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, velocity_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
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

    //we need a cube for this?
    //sorta but we maybe want multiple VAOs to switch for different
    //'sides' of the skybox to render
    //so actually just pass in large, world-edge quads
    //and rotate as necessary
    //maybe replace the 100 with a DEFINEd value
    //alternatively,
    //just always push each quad back via translation
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
    
    
    float skybox_vertices[] = {
	-100.0f, -100.0f, 0.0f, //bottom left
	100.0f, -100.0f, 0.0f,//bottom right
	-100.0f, 100.0f, 0.0f,//top left
	100.0f, 100.0f, 0.0f
	
    };
    
    

    uint skybox_indices[] = {
	0, 1, 2,
	2, 1, 3
    };

    uint skybox_vao;
    glGenVertexArrays(1, &skybox_vao);
    glBindVertexArray(skybox_vao);
    uint skybox_vbo;
    glGenBuffers(1, &skybox_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
    uint skybox_ebo;
    glGenBuffers(1, &skybox_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);//this 'unbinds' the vbo
    glBindVertexArray(0);//this 'unbinds' the vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//this 'unbinds' the vbo

    //




    //this should be a quad
    //add texture coords to this to render a sprite


    //I think we need some normals
    //then we translate/rotate those?

    int model_vertex_count;
    int bone_count;
    double animation_duration;
    float *model_vertices = NULL;
    float *model_normals = NULL;
    float *model_colors = NULL;
    Mat4 *bone_offset_matrices = NULL;
    //this can be static
    Mat4 bone_animation_matrices[MAX_BONES];
    GLint *bone_ids = NULL;
    GLuint model_vao;
    int model_load_failure;
    Skeleton_Node *skeleton_root_node = NULL;

    // rudy_ply.ply
    model_load_failure = load_model_file("art/rudy_rigged_better.fbx", "art/rudy_rigged_simple.ply", &model_vao, &model_vertices, &model_normals, &model_colors, &bone_offset_matrices, &bone_ids, &model_vertex_count, &bone_count, &skeleton_root_node, &animation_duration);
    if (model_load_failure) {
	printf("Hold on a model didn't load\n");
	return -1;
    }

    GLuint bone_debug_vao;
    load_points(&bone_debug_vao, bone_offset_matrices, bone_count);


    

    
    unsigned int vertex_indices_a[] = {
	0, 1, 2,
	2, 1, 3
    };

    GLuint particle_vao;
    generate_particles(&particle_vao);

    
    
    
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

    unsigned int particle_shader_program;
    shader_fault = load_shader_program(&particle_shader_program, "particle.vert", "particle.frag");
    if (shader_fault) {
	printf("your shader has a problem homie\n");
	return -1;
    }

    unsigned int point_shader_program;
    shader_fault = load_shader_program(&point_shader_program, "point.vert", "point.frag");
    if (shader_fault) {
	printf("your shader has a problem homie\n");
	return -1;
    }

    uint skybox_shader;
    shader_fault = load_shader_program(&skybox_shader, "sky.vert", "sky.frag");
    if (shader_fault) {
	printf("your shader has a problem homie\n");
	return -1;
    }

    int size_of_ground;
    int ground_width = 100;
    int ground_length = 100;
    float ground_height = 0.1f;
    float ground_scale = 100.0;


    //this looks more like a desert
    //than it does nevada, atm
    float *ground = create_ground_grid(ground_width, ground_length, ground_height, ground_scale, &size_of_ground);
    #if 0
    for (int i = 0 ; i < (ground_width * ground_length * 3); i++) {
	printf("i = %d\n", i);
	printf("ground co-ord is %f\n", ground[i]);
    }
#endif
    uint ground_element_count;
    uint size_of_elements;
    uint *ground_indices = create_ground_indices(ground_width, ground_length, &size_of_elements, &ground_element_count);
    uint ground_vao = load_ground(ground, ground_indices, size_of_ground, size_of_elements);
#if 0
    for (int i = 0; i < ground_element_count; i++) {
	printf("element is %d\n", ground_indices[i]);
    }
    #endif
    uint ground_shader;
    shader_fault= load_shader_program(&ground_shader, "ground.vert", "ground.frag");
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

    //WRAP THIS
    //load image
    int imwidth, imheight, nchannels;
    unsigned char *imdata = stbi_load("art/sand_first_pass.png", &imwidth, &imheight, &nchannels, 0);

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
    //int res_location = glGetUniformLocation(shader_program, "u_resolution");
    //int time_ms_location = glGetUniformLocation(shader_program, "time_ms");
    
    
    
    glUniform1i(tex1location, 0);
    

    //int tex2location = glGetUniformLocation(shader_program, "ourTexture2");
    //glUniform1i(tex2location, 1);

    
    
    
    stbi_image_free(imdata);

    //SDL_GL_SetSwapInterval(0);//using vsync seems to be kinda shitty?

   




    Vec3 camera = vec3_init(0.0f, 0.0f, 0.0f);
    Vec3 triangle_pos = vec3_init(0.0f, 0.0f, 0.0f);
    int transform_loc = glGetUniformLocation(shader_program, "transform");
    int perspective_loc = glGetUniformLocation(shader_program, "perspective");
    
    int basic_transform_loc = glGetUniformLocation(basic_shader_program, "transform");
    int basic_perspective_loc = glGetUniformLocation(basic_shader_program, "perspective");

    int particle_transform_loc = glGetUniformLocation(particle_shader_program, "transform");
    int particle_perspective_loc = glGetUniformLocation(particle_shader_program, "perspective");
    int particle_emitter_loc = glGetUniformLocation(particle_shader_program, "emitter_position_world");
    int particle_time_loc = glGetUniformLocation(particle_shader_program, "time_elapsed");


    int ground_transform_loc = glGetUniformLocation(ground_shader, "transform");
    int ground_perspective_loc = glGetUniformLocation(ground_shader, "perspective");

    int skybox_transform_loc = glGetUniformLocation(skybox_shader, "transform");
    int skybox_perspective_loc = glGetUniformLocation(skybox_shader, "perspective");
    int skybox_resolution_loc = glGetUniformLocation(skybox_shader, "resolution");


    

    float rot_angle = 0.0f;

    Mat4 perspective = mat4_create_perspective(60.0f, 1280.0/720.0f, 0.1f, 1000.0f);
    

    //
    float model_angle_x = 0.0f;
    float model_angle_y = 0.0f;
    float model_model_angle_z = 0.0f;

    float camera_angle_x = 0.0f;
    float camera_angle_y = 0.0f;
    float camera_angle_z = 0.0f;

    float animation_angle_x = 0.0f;
    float animation_angle_y = 0.0f;
    float animation_angle_z = 0.0f;

    float animation_timer = 0.0;

    float resolution[2] = {(float)SCREENWIDTH, (float)SCREENHEIGHT};
    //glUniform2f(res_location, resolution[0], resolution[1]);
    glUniform2fv(skybox_resolution_loc, 1, resolution);
    int bone_matrices_locations[MAX_BONES];

    Mat4 test_identity = mat4_create_identity();

    glUseProgram(basic_shader_program);

    char mat_name[64];
    for (int i = 0; i < MAX_BONES; i++) {
	sprintf(mat_name, "bone_matrices[%i]", i);//quite slick!
	bone_matrices_locations[i] = glGetUniformLocation(basic_shader_program, mat_name);
	glUniformMatrix4fv(bone_matrices_locations[i], 1, GL_FALSE, test_identity.elements);
	
	bone_animation_matrices[i] = mat4_create_identity();
    }



    Vec2 mouse_x_current;

    Vec2 mouse_y_last;


    
    //may need this 
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Vec3 camera_heading = vec3_init(0.0f, 0.0f, 0.0f);

    Vec3 ground_pos = vec3_init(0.0f, -0.5f, 0.0f);
    
    while (running) {
	current_time = SDL_GetTicks();
	float current_time_ms = current_time/1000.0f;
	time_elapsed = current_time - last_time;
	last_time = current_time;	
	float dt = time_elapsed/1000.0f;//dt is inconsistent, should be noted
	total_time_ms += dt;
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	int mouse_dx, mouse_dy;
	SDL_GetMouseState(&mouse_dx, &mouse_dy);
	SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
	float player_speed = 0.0f;
	
	//temporary semi-global animation timing
	animation_timer += (dt*0.25);
	if (animation_timer > animation_duration) {
	    //why this and not reset to zero?

	    //animation_timer = animation_duration - animation_timer;
	    animation_timer = 0.0f;
	    //printf("resetting to a value of %f\n", animation_timer);
	}

	if (keys[SDL_SCANCODE_ESCAPE]) {
	    running = false;
	    break;
	}

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

	//tgfh for MACK
	//note this is, apparently, the axis ABOUT which you rotate
	//so rotating ABOUT the x axis looks up and down.
	//unless i'm feeding these into the wrong
	//place
	float mouse_sensitivity = 5.0f;
	camera_angle_y -= (mouse_dx * mouse_sensitivity * dt);
	camera_angle_x -= (mouse_dy * mouse_sensitivity * dt);
	const float player_speed_constant = 2.5f;
	Vec2 player_strafe_vec = vec2_init(0.0f, 0.0f);
	if (keys[SDL_SCANCODE_T]) {
	    player_speed = player_speed_constant;
	    player_strafe_vec.y = 1.0f;
	    //camera_angle_x += 100*dt;
	}
	if (keys[SDL_SCANCODE_G]) {
	    player_speed = -player_speed_constant;
	    player_strafe_vec.y = -1.0f;
	    //camera_angle_x -= 100*dt;
	}
	if (keys[SDL_SCANCODE_F]) {
	    player_strafe_vec.x = 1.0f;
	    //camera_angle_y += 100*dt;
	}
	if (keys[SDL_SCANCODE_H]) {
	    player_strafe_vec.x = -1.0f;
	    //camera_angle_y -= 100*dt;
	}
	if (vec2_mag(player_strafe_vec) != 0.0f) {
	    //maybe check for float error
	    player_strafe_vec = vec2_normalize(player_strafe_vec);
	}

	camera_heading.x = sin_deg(camera_angle_y);
	//camera_heading.y = -sin_deg(camera_angle_x);
	camera_heading.z = -cos_deg(camera_angle_y);
	//camera_heading = vec3_normalize(camera_heading);
	//strafing is a bit weird
	//we have a HEADING along the x-z axis
	//and fowards/backwards *ALWAYS* simply move us foward or backwards
	//and left/right *ALWAYS* adds perpendicular motion to this point

	//try this
	//make 2 vec2s
	Vec2 mouse_heading = vec2_init(sin_deg(camera_angle_y) * player_strafe_vec.y, -cos_deg(camera_angle_y) * player_strafe_vec.y);
	Vec2 strafe_heading = vec2_init(sin_deg(camera_angle_y + 90.0f) * player_strafe_vec.x, -cos_deg(camera_angle_y + 90.0f) * player_strafe_vec.x);
	Vec2 vec2_player_heading = vec2_add(mouse_heading, strafe_heading);
	if (vec2_mag(vec2_player_heading) != 0.0f) {
	    vec2_player_heading = vec2_normalize(vec2_player_heading);
	}
	camera_heading.x = vec2_player_heading.x;
	camera_heading.z = vec2_player_heading.y;
	camera_heading.y = 0.0f;

	

	#if DEBUG_FLYTHROUGH
	camera_heading.x = sin_deg(camera_angle_y);
	camera_heading.y = -sin_deg(camera_angle_x);
	camera_heading.z = -cos_deg(camera_angle_y);
	camera = vec3_add(camera, vec3_scale(camera_heading, dt * player_speed));
	#else
	


	camera = vec3_add(camera, vec3_scale(camera_heading, dt * player_speed_constant));
	#endif

	int bone_to_animate = 3;


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
	    model_angle_x += 100*dt;
	}
	if (keys[SDL_SCANCODE_E]) {
	    model_angle_x -= 100*dt;
	}

	if (keys[SDL_SCANCODE_Z]) {
	    model_angle_y += 100*dt;
	}
	if (keys[SDL_SCANCODE_X]) {
	    model_angle_y -= 100*dt;
	}

	
	//skeleton_animate(skeleton_root_node, animation_timer, mat4_create_identity(), bone_offset_matrices, bone_animation_matrices);//TODO: create mat4 *bone_animation_matrices of size .. ? ..in main scope and init each with to mat4_identity
	
	

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










	draw_skybox(skybox_shader, skybox_vao, camera_angle_x, camera_angle_y, camera_angle_z, camera, skybox_transform_loc, skybox_perspective_loc, perspective);
	//Mat4 skybox_transform = mat4_create_translation_rotation(rotation, camera);
	//not drawing?


	

	//draw_ground(shader_program, test_vaos[0], camera_angle_x, camera_angle_y, camera_angle_z, ground_pos, camera, test_texture, tex1location, transform_loc, perspective_loc, perspective);

	draw_untextured_ground(ground_shader, ground_vao, ground_element_count, camera_angle_x, camera_angle_y, camera_angle_z, ground_pos, camera, ground_transform_loc, ground_perspective_loc, perspective);
	
	
	draw_animated_object(basic_shader_program, model_vao, model_vertex_count, triangle_pos, camera, perspective, bone_animation_matrices, basic_transform_loc, basic_perspective_loc, bone_matrices_locations, model_angle_x, model_angle_y, camera_angle_x, camera_angle_y, animation_angle_x, animation_angle_y, animation_angle_z);


	Vec3 emitter_pos = vec3_init(0.0f, 0.0f, 1.0f);
	//draw_particles(particle_shader_program, particle_vao, PARTICLE_COUNT, current_time_ms, emitter_pos, camera, perspective, particle_transform_loc, particle_perspective_loc, particle_time_loc, particle_emitter_loc);


	//used for debug purposes
	#if 0
	int point_perspective_loc = glGetUniformLocation(point_shader_program, "perspective");
	int point_transform_loc = glGetUniformLocation(point_shader_program, "transform");
	
	glUseProgram(point_shader_program);
	glUniformMatrix4fv(point_perspective_loc, 1, GL_FALSE, perspective.elements);
	//Mat4 transform = mat4_create_translation(camera);
	Mat3 rotate_x = mat3_create_rotate_x(model_angle_x + 90.0f);
	Mat3 rotate_y = mat3_create_rotate_y(model_angle_y);
	Mat3 rotation = mat3_mult(rotate_x, rotate_y);
	Mat4 transform = mat4_create_translation_rotation(rotation, camera);
	glUniformMatrix4fv(point_transform_loc, 1, GL_FALSE, transform.elements);
	
	glUseProgram(point_shader_program);
	glBindVertexArray(bone_debug_vao);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, bone_count);
	glDisable(GL_PROGRAM_POINT_SIZE);
	#endif
	SDL_GL_SwapWindow(window);
    }
    
    
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
    return 0;
}
