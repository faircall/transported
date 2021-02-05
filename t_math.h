#include <math.h>

#define M_PI 3.1415926535
#define M_PI_2 1.5707963267

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


typedef float Mat2[2][2];
typedef float Mat3[3][3];
typedef float Mat4[4][4];

Vec2 vec2_init(float x, float y);
Vec3 vec3_init(float x, float y, float z);
Vec4 vec4_init(float x, float y, float z, float w);

Vec2 vec2_scale(float s, Vec2 v);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
float vec2_mag(Vec2 v);
Vec2 vec2_normalize(Vec2 v);
float vec2_dot(Vec2 a, Vec2 b);
float vec2_angle_between(Vec2 a, Vec2 b);
float vec2_angle(Vec2 a);

Vec3 vec3_scale(float s, Vec3 v);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
float vec3_mag(Vec3 v);
Vec3 vec3_normalize(Vec3 v);
float vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);
float vec3_scalar_triple(Vec3 a, Vec3 b, Vec3 c);

float mat2_det(Mat2 m);
float mat3_det(Mat3 m);
float mat4_det(Mat4 m);
void mat4_create_identity(Mat4 a);
void mat4_create_scale(Mat4 a, float scalar);
void mat4_translate(Mat4 a, Vec3 t);
void mat4_create_x_rotation(Mat4 a, float angle);
void mat4_create_y_rotation(Mat4 a, float angle);
void mat4_create_z_rotation(Mat4 a, float angle);
void mat4_create_perspective(Mat4 a, float fovy, float aspect_ratio, float near, float far);

void mat4_copy(Mat4 a, Mat4 b);

void mat4_transpose_in_place(Mat4 in);
void mat4_transpose(Mat4 in, Mat4 out);
void mat4_compare(Mat4 a, Mat4 b);




