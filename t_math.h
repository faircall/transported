#ifndef T_MATH_H
#define T_MATH_H

#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535
#define M_PI_2 1.5707963267


#define MYPI 3.1415

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

//they are the same struct but we want to be
//specific 
typedef Vec4 Quaternion;

//I'm unsure if this is actually
//getting around the issue of a bunch of
//heap allocs
//but it *will* make the code much easier to write
//and I suppose I could go back and do a perf pass
//to make these stack alloc'd if that's the better
//option, or add that option with some extra code
typedef struct {
    float elements[2][2];
} Mat2;


//here's where we make it column major
#define mat2(mat,a,b) mat.elements[b][a]

typedef struct {
    float elements[3][3];
} Mat3;


//here's where we make it column major
#define mat3(mat,a,b) mat.elements[b][a]


typedef struct {
    float elements[4][4];
} Mat4;

#define mat4(mat,a,b) mat.elements[b][a]


Vec2 vec2_init(float x, float y);

Vec2 vec2_add(Vec2 a, Vec2 b);

Vec2 vec2_sub(Vec2 a, Vec2 b);

Vec2 vec2_scale(Vec2 a, float scalar);

float vec2_dot(Vec2 a, Vec2 b);

float vec2_mag(Vec2 a);

Vec2 vec2_normalize(Vec2 a);

Mat2 mat2_init_vec2(Vec2 a, Vec2 b);
Mat2 mat2_init_float(float a00, float a01,
		     float a10, float a11);

Mat2 mat2_scale(Mat2 a, float scalar);

float mat2_det(Mat2 a);

Mat2 mat2_inverse(Mat2 a);

Vec3 vec3_init(float x, float y, float z);


Vec3 vec3_add(Vec3 a, Vec3 b);

Vec3 vec3_sub(Vec3 a, Vec3 b);

Vec3 vec3_scale(Vec3 a, float scalar);


float vec3_mag(Vec3 a);

Vec3 vec3_normalize(Vec3 a);


Vec3 vec3_cross(Vec3 a, Vec3 b);

float vec3_triple(Vec3 a, Vec3 b, Vec3 c);

Mat3 mat3_create_zero(void);

Mat3 mat3_create_identity(void);


void mat3_print_elements(Mat3 m);



Mat3 mat3_init_vec3(Vec3 a, Vec3 b, Vec3 c);
Mat3 mat3_init_float(float a00, float a01, float a02,
		     float a10, float a11, float a12,
		     float a20, float a21, float a22);

Mat3 mat3_mult(Mat3 a, Mat3 b);


Mat3 mat3_transpose(Mat3 a);

float mat3_det(Mat3 a);




Mat3 mat3_adjugate(Mat3 a);
Mat3 mat3_scale(Mat3 a, float scalar);
Mat3 mat3_inverse(Mat3 a);

Mat3 mat3_create_rotate_z(float angle);
Mat3 mat3_create_rotate_x(float angle);
Mat3 mat3_create_rotate_y(float angle);

Vec3 vec3_from_mat3(Mat3 a, int col);
Vec3 vec3_from_mat4(Mat4 a, int col);

Mat4 mat4_create_zero(void);
Mat4 mat4_create_identity(void);
Mat4 mat4_mult(Mat4 a, Mat4 b);

Vec4 vec4_init(float x, float y, float z, float w);
Mat4 mat4_scale(Mat4 a, float scalar);



Mat4 mat4_from_vec4_row(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3);

Mat4 mat4_from_mat3(Mat3 a);

void mat4_print_elements(Mat4 m);

Mat4 mat4_init(float a00, float a01, float a02, float a03,
	       float a10, float a11, float a12, float a13,
	       float a20, float a21, float a22, float a23,
	       float a30, float a31, float a32, float a33);

Mat4 mat4_from_vec4_col(Vec4 c0, Vec4 c1, Vec4 c2, Vec4 c3);

Mat4 mat4_inverse(Mat4 a);

Mat4 mat4_create_translation(Vec3 t);

Mat4 mat4_create_translation_rotation(Mat3 rotation, Vec3 t);

Mat4 mat4_create_rotation(Mat3 rotation);

Mat4 mat4_create_perspective(float fovy, float s, float n, float f);

float cos_deg(float angle);
float sin_deg(float angle);
float tan_deg(float angle);

float deg_to_rad(float angle);


Quaternion quaternion_init(float x, float y, float z, float w);
Quaternion quaternion_from_vector_scalar(Vec3 v, float s);
Quaternion quaternion_add(Quaternion a, Quaternion b);
Quaternion quaternion_scale(Quaternion q, float s);
Vec3 vec3_from_quaternion(Quaternion q);
float scalar_from_quaternion(Quaternion q);
Quaternion quaternion_mult(Quaternion a, Quaternion b);
Quaternion quaternion_conjugate(Quaternion q);
Quaternion quaternion_inverse(Quaternion q);
Quaternion quaternion_pure(Vec3 v);
Quaternion quaternion_slerp(Quaternion q1, Quaternion q2, float t);
Quaternion quaternion_lerp(Quaternion q1, Quaternion q2, float t);
Mat3 mat3_from_quaternion(Quaternion q);
Mat4 mat4_from_quaternion(Quaternion q);
Mat4 mat4_transpose(Mat4 a);
Mat4 mat4_create_scale_from_float(float s);
Mat4 mat4_create_scale_from_vec(Vec3 v);

float quaternion_mag(Quaternion q);

#endif




