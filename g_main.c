#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



//todo:mouselook
//(eventually via quaternions??)

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
//todo: check the animation system using an actual
//confirmed good rigged test model, rather than the one i made






//NEXT: return to work on the animation system.
//with the new bone animation mats


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

#define PARTICLE_COUNT 300

#define MAX_BONES 32 //enough?


Mat4 g_local_animations[MAX_BONES];

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
    //technically these are versors
    Quaternion *rotation_keys;
    Vec3 *scale_keys;
    
    double *position_key_times;
    double *rotation_key_times;
    double *scale_key_times;

    int num_position_keys;
    int num_rotation_keys;
    int num_scale_keys;
    
    //existing data
    char name[64];
    int num_children;
    int bone_index;
} Skeleton_Node;

boolean import_skeleton_node(struct aiNode *assimp_node, Skeleton_Node **skeleton_node, int bone_count, char bone_names[][64])//this might be cpp nonsense
{
    //function will work like this:
    //we give it the assimp root node (scene)
    //a null-pointer to skeleton root node (allocate internally)
    //and total bone count

    Skeleton_Node *temp = (Skeleton_Node*)malloc(sizeof(Skeleton_Node));

    strcpy(temp->name, assimp_node->mName.data);//this may fuck us up

    printf("-node name = %s\n", temp->name);
    temp->num_children = 0;
    printf("node has %d children\n", (int)assimp_node->mNumChildren);
    temp->bone_index = -1;
    for (int i = 0; i < MAX_BONES; i++) {
	temp->children[i] = NULL;//init to zero
    }
    boolean has_bone = false;
    for (int i = 0; i < bone_count; i++) {
	if (!strcmp(bone_names[i], temp->name)) {
	    printf("node uses bone %i\n", i);
	    temp->bone_index = i;
	    has_bone = true;
	    break;
	}
    }

    if (!has_bone) {
	printf("No bone found\n");
    } else {
	printf("found a BONE, ey?\n");
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
	    printf("useless child culled\n");
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

void skeleton_animate(Skeleton_Node *node, Mat4 parent_matrix, Mat4 *bone_offset_matrices,
    Mat4 *bone_animation_matrices)
{
    //assert node here?
    if (node == NULL) {
	//corrupting the stack which aint great
	return;
    }

    Mat4 our_matrix = parent_matrix;

    Mat4 local_animation = mat4_create_identity();

    int bone_index = node->bone_index;

    //where do we change bone animation matrices??
    
    if (bone_index > -1) {
	Mat4 bone_offset = bone_offset_matrices[bone_index];
	Mat4 inverse_bone_offset = mat4_inverse(bone_offset);
	local_animation = g_local_animations[bone_index];
	our_matrix = mat4_mult(parent_matrix, mat4_mult(inverse_bone_offset, mat4_mult(local_animation, bone_offset)));
	bone_animation_matrices[bone_index] = our_matrix;
    } else {
	printf("animation did nothing to thsi node because bone index was %d\n", bone_index);
	//our_matrix = parent_matrix;
	//bone_animation_matrices[bone_index] = our_matrix;
    }

    for (int i = 0; i < node->num_children; i++) {
	skeleton_animate(node->children[i], our_matrix, bone_offset_matrices, bone_animation_matrices);
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

void draw_object(int model_shader, int model_vao, int vertex_count, Vec3 pos, Vec3 camera, Mat4 perspective, Mat4 *bone_animation_matrices, int transform_loc, int perspective_loc, int *bone_matrices_locations, float model_angle_x, float model_angle_y, float camera_angle_x, float camera_angle_y, float animation_angle_x, float animation_angle_y, float animation_angle_z)
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

    for (int i = 0; i < MAX_BONES; i++) {
	
	glUniformMatrix4fv(bone_matrices_locations[i], 1, GL_FALSE, bone_animation_matrices[i].elements);
    }

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


    if (animations > 0) {
	struct aiAnimation *animation = scene->mAnimations[0];
	*animation_duration = animation->mDuration;
	/* we will get keys here */
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

	    (*bone_offset_matrices)[i] = mat4_from_assimp_mat4(bone->mOffsetMatrix);

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
    model_load_failure = load_model_file("art/rudy_rigged_animation.fbx", "art/rudy_rigged.ply", &model_vao, &model_vertices, &model_normals, &model_colors, &bone_offset_matrices, &bone_ids, &model_vertex_count, &bone_count, &skeleton_root_node, &animation_duration);
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

    int particle_transform_loc = glGetUniformLocation(particle_shader_program, "transform");
    int particle_perspective_loc = glGetUniformLocation(particle_shader_program, "perspective");
    int particle_emitter_loc = glGetUniformLocation(particle_shader_program, "emitter_position_world");
    int particle_time_loc = glGetUniformLocation(particle_shader_program, "time_elapsed");
    

    float rot_angle = 0.0f;

    Mat4 perspective = mat4_create_perspective(60.0f, 1280.0/720.0f, 0.1f, 100.0f);
    

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

    double animation_timer = 0.0;

    float resolution[2] = {(float)SCREENWIDTH, (float)SCREENHEIGHT};
    glUniform2f(res_location, resolution[0], resolution[1]);

    int bone_matrices_locations[MAX_BONES];

    Mat4 test_identity = mat4_create_identity();

    glUseProgram(basic_shader_program);

    char mat_name[64];
    for (int i = 0; i < MAX_BONES; i++) {
	sprintf(mat_name, "bone_matrices[%i]", i);//quite slick!
	bone_matrices_locations[i] = glGetUniformLocation(basic_shader_program, mat_name);
	glUniformMatrix4fv(bone_matrices_locations[i], 1, GL_FALSE, test_identity.elements);
	g_local_animations[i] = mat4_create_identity();
	bone_animation_matrices[i] = mat4_create_identity();
    }



    
    
    
    while (running) {
	current_time = SDL_GetTicks();
	float current_time_ms = current_time/1000.0f;
	time_elapsed = current_time - last_time;
	last_time = current_time;	
	float dt = time_elapsed/1000.0f;//dt is inconsistent, should be noted
	total_time_ms += dt;
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	boolean motion_occured = false;

	//temporary semi-global animation timing
	animation_timer += dt;
	if (animation_timer > animation_duration) {
	    animation_timer -= animatation_duration;
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
	if (keys[SDL_SCANCODE_T]) {
	    camera_angle_x += 100*dt;
	}
	if (keys[SDL_SCANCODE_G]) {
	    camera_angle_x -= 100*dt;
	}
	if (keys[SDL_SCANCODE_F]) {
	    camera_angle_y += 100*dt;
	}
	if (keys[SDL_SCANCODE_H]) {
	    camera_angle_y -= 100*dt;
	}

	int bone_to_animate = 3;
	if (keys[SDL_SCANCODE_I]) {
	    animation_angle_x += 100*dt;
	    g_local_animations[bone_to_animate] = mat4_from_mat3(mat3_create_rotate_x(animation_angle_x));
	    motion_occured = true;
	}
	if (keys[SDL_SCANCODE_K]) {
	    animation_angle_x -= 100*dt;
	    g_local_animations[bone_to_animate] = mat4_from_mat3(mat3_create_rotate_x(animation_angle_x));
	    motion_occured = true;
	}
	if (keys[SDL_SCANCODE_J]) {
	    animation_angle_y += 100*dt;
	    g_local_animations[bone_to_animate] = mat4_from_mat3(mat3_create_rotate_y(animation_angle_y));
	    motion_occured = true;
	}
	if (keys[SDL_SCANCODE_L]) {
	    animation_angle_y -= 100*dt;
	    g_local_animations[bone_to_animate] = mat4_from_mat3(mat3_create_rotate_y(animation_angle_y));
	    motion_occured = true;
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

	if (motion_occured) {
	    skeleton_animate(skeleton_root_node, mat4_create_identity(), bone_offset_matrices, bone_animation_matrices);//TODO: create mat4 *bone_animation_matrices of size .. ? ..in main scope and init each with to mat4_identity
	}
	

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
	
	
	draw_object(basic_shader_program, model_vao, model_vertex_count, triangle_pos, camera, perspective, bone_animation_matrices, basic_transform_loc, basic_perspective_loc, bone_matrices_locations, model_angle_x, model_angle_y, camera_angle_x, camera_angle_y, animation_angle_x, animation_angle_y, animation_angle_z);


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
