#include "t_math.h"

#include <math.h>


Vec2 vec2_init(float x, float y)
{
    Vec2 result;
    result.x = x;
    result.y = y;
    return result;
}

Vec2 vec2_add(Vec2 a, Vec2 b)
{
    Vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

Vec2 vec2_sub(Vec2 a, Vec2 b)
{
    Vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

Vec2 vec2_scale(Vec2 a, float scalar)
{
    Vec2 result;
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    return result;
}

float vec2_dot(Vec2 a, Vec2 b)
{
    float result = a.x*b.x + a.y*b.y;
    return result;
}

float vec2_mag(Vec2 a)
{
    float result = sqrt(a.x*a.x + a.y*a.y);
    return result;
}

Vec2 vec2_normalize(Vec2 a)
{
    Vec2 result;
    float mag = vec2_mag(a);
    float inv_mag = 1.0/mag;
    result = vec2_scale(a, inv_mag);
    return result;
}

Mat2 mat2_init_vec2(Vec2 a, Vec2 b)
{
    Mat2 result;
    mat2(result, 0, 0) = a.x;
    mat2(result, 0, 1) = b.x;
    mat2(result, 1, 0) = a.y;
    mat2(result, 1, 1) = b.y;
    return result;
}

Mat2 mat2_init_float(float a00, float a01,
		     float a10, float a11)
{
    Mat2 result;

    mat2(result, 0, 0) = a00;
    mat2(result, 0, 1) = a01;
    mat2(result, 1, 0) = a10;
    mat2(result, 1, 1) = a11;
    return result;
}

Mat2 mat2_scale(Mat2 a, float scalar)
{
    Mat2 result;
    for (int i = 0; i < 2; i++) {
	for (int j = 0; j < 2; j++) {
	    mat2(result,i,j) = scalar*mat2(a,i,j);
	}
    }
    return result;
}

float mat2_det(Mat2 a)
{
    float result = mat2(a,0,0)*mat2(a,1,1) - mat2(a,0,1)*mat2(a,1,0);
    return result;
}

Mat2 mat2_inverse(Mat2 a)
{
    float det = mat2_det(a);
    Mat2 result;
    mat2(result,0,0) = mat2(a,1,1);
    mat2(result,1,1) = mat2(a,0,0);
    mat2(result,0,1) = -mat2(a,0,1);
    mat2(result,1,0) = -mat2(a,1,0);
    result = mat2_scale(result, det);
    return result;
}

Vec3 vec3_init(float x, float y, float z)
{
    Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

Vec3 vec3_add(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

Vec3 vec3_scale(Vec3 a, float scalar)
{
    Vec3 result;
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    result.z = a.z * scalar;
    return result;
}

float vec3_mag(Vec3 a)
{
    float result = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return result;
}

Vec3 vec3_normalize(Vec3 a)
{
    Vec3 result;
    float mag = vec3_mag(a);
    float inv_mag = 1.0/mag;
    result = vec3_scale(a, inv_mag);
    return result;
}

float vec3_dot(Vec3 a, Vec3 b)
{
    float result = a.x*b.x + a.y*b.y + a.z*b.z;
    return result;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.y*b.z - b.y*a.z;
    result.y = a.z*b.x - b.z*a.x;
    result.z = a.x*b.y - b.x*a.y;
    return result;
}

float vec3_triple(Vec3 a, Vec3 b, Vec3 c)
{
    float result = vec3_dot(vec3_cross(a,b), c);
    return result;
}

Mat3 mat3_init_float(float a00, float a01, float a02,
		     float a10, float a11, float a12,
		     float a20, float a21, float a22)
{
    Mat3 result;
    mat3(result, 0, 0) = a00;
    mat3(result, 0, 1) = a01;
    mat3(result, 0, 2) = a02;

    mat3(result, 1, 0) = a10;
    mat3(result, 1, 1) = a11;
    mat3(result, 1, 2) = a12;

    mat3(result, 2, 0) = a20;
    mat3(result, 2, 1) = a21;
    mat3(result, 2, 2) = a22;

    return result;
}

Mat3 mat3_init_vec3(Vec3 a, Vec3 b, Vec3 c)		    
{
    Mat3 result;
    mat3(result, 0, 0) = a.x;
    mat3(result, 0, 1) = b.x;
    mat3(result, 0, 2) = c.x;

    mat3(result, 1, 0) = a.y;
    mat3(result, 1, 1) = b.y;
    mat3(result, 1, 2) = c.y;

    mat3(result, 2, 0) = a.z;
    mat3(result, 2, 1) = b.z;
    mat3(result, 2, 2) = c.z;

    return result;
}

Mat3 mat3_create_zero(void)
{
    Mat3 result;
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    mat3(result, i, j) = 0;
	}
    }
    return result;
}

Mat3 mat3_create_identity(void)
{
    Mat3 result;
    result = mat3_create_zero();
    mat3(result, 0, 0) = 1;
    mat3(result, 1, 1) = 1;
    mat3(result, 2, 2) = 1;
    return result;
}

void mat3_print_elements(Mat3 m)
{
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    printf("row %d col %d has element %f\n", i, j, mat3(m, i, j));
	}
    }
}

Mat3 mat3_mult(Mat3 a, Mat3 b)
{
    //this is easy to code but probably slow atm
    //so do an optimization pass later
    Mat3 result;
    //result = mat3_create_zero();
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    mat3(result, i, j) = 0;
	    for (int k = 0; k < 3; k++) {
		mat3(result, i, j) += mat3(a,i,k)*mat3(b,k,j);
	    }
	}
    }

    return result;
}

Mat3 mat3_transpose(Mat3 a)
{
    Mat3 result;
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    mat3(result, i, j) = mat3(a, j, i);
	}
    }
    return result;
}

float mat3_det(Mat3 a)
{
    //the determinant of a 3x3 matrix
    //is equal to the scalar triple product
    //of the column vectors of the matrix

    //insane how much more
    //compact this is
    Vec3 va = vec3_from_mat3(a,0);
    Vec3 vb = vec3_from_mat3(a,1);
    Vec3 vc = vec3_from_mat3(a,2);
    float result = vec3_triple(va,vb,vc);
    return result;
}

#if 0
float mat3_det(Mat3 a)
{
    //use the explicit formula
    float t1 = mat3(a,0,0)*mat3(a,1,1)*mat3(a,2,2);
    float t2 = mat3(a,0,1)*mat3(a,1,2)*mat3(a,2,0);
    float t3 = mat3(a,0,2)*mat3(a,1,0)*mat3(a,2,1);
    
    float t4 = mat3(a,0,0)*mat3(a,1,2)*mat3(a,2,1);
    float t5 = mat3(a,0,1)*mat3(a,1,0)*mat3(a,2,2);
    float t6 = mat3(a,0,2)*mat3(a,1,1)*mat3(a,2,0);

    float result = t1 + t2 + t3 - t4 - t5 - t6;
    return result;
}
#endif

Mat3 mat3_adjugate(Mat3 a)
//I *think* this is correct but a typo is quite possible,
//better to make a function which
//generates arbitrary adjugate matrices
{
    float a00 = mat3(a,1,1)*mat3(a,2,2) - mat3(a,1,2)*mat3(a,2,1);
    float a01 = mat3(a,0,2)*mat3(a,2,1) - mat3(a,0,1)*mat3(a,2,2);
    float a02 = mat3(a,0,1)*mat3(a,1,2) - mat3(a,0,2)*mat3(a,1,1);

    float a10 = mat3(a,1,2)*mat3(a,2,0) - mat3(a,1,0)*mat3(a,2,2);
    float a11 = mat3(a,0,0)*mat3(a,2,2) - mat3(a,0,2)*mat3(a,2,0);
    float a12 = mat3(a,0,2)*mat3(a,1,0) - mat3(a,0,0)*mat3(a,1,2);

    float a20 = mat3(a,1,0)*mat3(a,2,1) - mat3(a,1,1)*mat3(a,2,0);
    float a21 = mat3(a,0,1)*mat3(a,2,0) - mat3(a,0,0)*mat3(a,2,1);
    float a22 = mat3(a,0,0)*mat3(a,1,1) - mat3(a,0,1)*mat3(a,1,0);

    Mat3 result = mat3_init_float(a00, a01, a02,
				  a10, a11, a12,
				  a20, a21, a22);
    return result;
}

Mat3 mat3_scale(Mat3 a, float scalar)
{
    Mat3 result;
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    mat3(result,i,j) = mat3(a,i,j)*scalar;
	}
    }
    return result;
}
    

Mat3 mat3_inverse(Mat3 a)
{
    //oh yeah, check if det is zero...
    float det = mat3_det(a);
    if (det == 0.0f) {
	//handle degenerate case
    }
    float inv_det = 1.0f/det;
    Mat3 adj = mat3_adjugate(a);
    Mat3 result = mat3_scale(adj, inv_det);
    return result;
}

Mat3 mat3_create_rotate_z(float angle)
{
    Mat3 result = mat3_create_identity();
    mat3(result, 0, 0) = cos_deg(angle);
    mat3(result, 1, 1) = cos_deg(angle);
    mat3(result, 0, 1) = -sin_deg(angle);
    mat3(result, 1, 0) = sin_deg(angle);
    return result;
}

Mat3 mat3_create_rotate_x(float angle)
{
    Mat3 result = mat3_create_identity();
    mat3(result, 1, 1) = cos_deg(angle);
    mat3(result, 2, 2) = cos_deg(angle);
    mat3(result, 1, 2) = -sin_deg(angle);
    mat3(result, 2, 1) = sin_deg(angle);
    return result;
}

Mat3 mat3_create_rotate_y(float angle)
{
    Mat3 result = mat3_create_identity();
    mat3(result, 0, 0) = cos_deg(angle);
    mat3(result, 2, 2) = cos_deg(angle);
    mat3(result, 0, 2) = sin_deg(angle);
    mat3(result, 2, 0) = -sin_deg(angle);
    return result;
}



Mat4 mat4_create_zero(void)
{
    Mat4 result;
        for (int i = 0; i < 4; i++) {
	    for (int j = 0; j < 4; j++) {
		mat4(result, i, j) = 0;
	    }
    }
    return result;    
}

Mat4 mat4_create_identity(void)
{
    Mat4 result;
    result = mat4_create_zero();
    mat4(result, 0, 0) = 1.0f;
    mat4(result, 1, 1) = 1.0f;
    mat4(result, 2, 2) = 1.0f;
    mat4(result, 3, 3) = 1.0f;
    return result;
}

Mat4 mat4_create_translation(Vec3 t)
{
    Mat4 result = mat4_create_identity();
    mat4(result, 0, 3) = t.x;
    mat4(result, 1, 3) = t.y;
    mat4(result, 2, 3) = t.z;
    return result;
}

Mat4 mat4_create_translation_rotation(Mat3 rotation, Vec3 t)
{
    Mat4 result = mat4_create_identity();
    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    mat4(result, i, j) = mat3(rotation, i, j);
	}
    }
    mat4(result, 0, 3) = t.x;
    mat4(result, 1, 3) = t.y;
    mat4(result, 2, 3) = t.z;

    return result;
}

Mat4 mat4_create_perspective(float fovy, float aspect, float near, float far)
{
    //s is aspect_ratio
    //n is near
    //f is far

    //I feel like the aspect ratio is not quite right
    float g = 1.0f / tan_deg(fovy * 0.5f);
    float k = far / (far - near);

    Mat4 result = mat4_create_identity();
    #if 1
    mat4(result, 0, 0) = g/aspect;
    mat4(result, 1, 1) = g;
    mat4(result, 2, 2) = k;
    mat4(result, 2, 3) = -near*k;
    mat4(result, 3, 2) = 1.0f;
    mat4(result, 3, 3) = 0.0f;
    #endif
    return result;
}

Mat4 mat4_mult(Mat4 a, Mat4 b)
{
    Mat4 result;
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 4; j++) {
	    mat4(result, i, j) = 0;
	    for (int k = 0; k < 4; k++) {
		mat4(result, i, j) += mat4(a,i,k)*mat4(b,k,j);
	    }
	}
    }

    return result;
}

Vec3 vec3_from_mat3(Mat3 a, int col)
{
    //extracts a vec3 from a column of A
    Vec3 result;
    result.x = mat3(a, 0, col);
    result.y = mat3(a, 1, col);
    result.z = mat3(a, 2, col);
    return result;
}

Vec3 vec3_from_mat4(Mat4 a, int col)
{
    //extracts a vec3 from a column of A
    Vec3 result;
    result.x = mat4(a, 0, col);
    result.y = mat4(a, 1, col);
    result.z = mat4(a, 2, col);
    return result;
}

Mat4 mat4_inverse(Mat4 a)
{

    //seeeeems to work except
    //just note we'll have negative zero floats due
    //to the structure
    Vec3 va = vec3_from_mat4(a, 0);
    Vec3 vb = vec3_from_mat4(a, 1);
    Vec3 vc = vec3_from_mat4(a, 2);
    Vec3 vd = vec3_from_mat4(a, 3);

    float x = mat4(a,3,0);
    float y = mat4(a,3,1);
    float z = mat4(a,3,2);
    float w = mat4(a,3,3);

    Vec3 s = vec3_cross(va, vb);
    Vec3 t = vec3_cross(vc, vd);
    Vec3 u = vec3_sub(vec3_scale(va, y), vec3_scale(vb, x));
    Vec3 v = vec3_sub(vec3_scale(vc, w), vec3_scale(vd, z));

    float det = vec3_dot(s,v) + vec3_dot(t,u);
    if (det == 0.0f) {
	//handle degenerate case
    }
    float inv_det = 1.0f/det;

    //check equation 1.99 from Lengyel

    Vec3 r0_v3 = vec3_add(vec3_cross(vb,v), vec3_scale(t, y));
    Vec4 r0 = vec4_init(r0_v3.x, r0_v3.y, r0_v3.z, -vec3_dot(vb,t));

    Vec3 r1_v3 = vec3_sub(vec3_cross(v, va), vec3_scale(t, x));
    Vec4 r1 = vec4_init(r1_v3.x, r1_v3.y, r1_v3.z, vec3_dot(va, t));

    Vec3 r2_v3 = vec3_add(vec3_cross(vd, u), vec3_scale(s, w));
    Vec4 r2 = vec4_init(r2_v3.x, r2_v3.y, r2_v3.z, -vec3_dot(vd, s));

    Vec3 r3_v3 = vec3_sub(vec3_cross(u, vc), vec3_scale(s, z));
    Vec4 r3 = vec4_init(r3_v3.x, r3_v3.y, r3_v3.z, vec3_dot(vc, s));

    Mat4 row_matrix = mat4_from_vec4_row(r0, r1, r2, r3);

    Mat4 result = mat4_scale(row_matrix, inv_det);
    return result;
			
}

Vec4 vec4_init(float x, float y, float z, float w)
{
    Vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

Vec4 vec4_add(Vec4 a, Vec4 b)
{
    Vec4 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

Vec4 vec4_sub(Vec4 a, Vec4 b)
{
    Vec4 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    return result;
}

Vec4 vec4_scale(Vec4 a, float scalar)
{
    Vec4 result;
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    result.z = a.z * scalar;
    result.w = a.w * scalar;
    return result;
}


float vec4_mag(Vec4 a)
{
    float result = sqrt(a.x*a.x + a.y*a.y + a.z*a.z +a.w*a.w);
    return result;
}

Vec4 vec4_normalize(Vec4 a)
{
    Vec4 result;
    float mag = vec4_mag(a);
    float inv_mag = 1.0/mag;
    result = vec4_scale(a, inv_mag);
    return result;
}

float vec4_dot(Vec4 a, Vec4 b)
{
    float result = a.x*b.x + a.y*b.y + a.z*b.z + a.w*a.w;
    return result;
}

Mat4 mat4_scale(Mat4 a, float scalar)
{
    Mat4 result;
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 4; j++) {
	    mat4(result, i, j) = mat4(a, i, j) * scalar;
	}
    }

    return result;
}

Mat4 mat4_from_vec4_row(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3)
{
    Mat4 result;

    mat4(result, 0, 0) = r0.x;
    mat4(result, 0, 1) = r0.y;
    mat4(result, 0, 2) = r0.z;
    mat4(result, 0, 3) = r0.w;

    mat4(result, 1, 0) = r1.x;
    mat4(result, 1, 1) = r1.y;
    mat4(result, 1, 2) = r1.z;
    mat4(result, 1, 3) = r1.w;

    mat4(result, 2, 0) = r2.x;
    mat4(result, 2, 1) = r2.y;
    mat4(result, 2, 2) = r2.z;
    mat4(result, 2, 3) = r2.w;

    mat4(result, 3, 0) = r3.x;
    mat4(result, 3, 1) = r3.y;
    mat4(result, 3, 2) = r3.z;
    mat4(result, 3, 3) = r3.w;
    
    
    return result;
}

Mat4 mat4_from_vec4_col(Vec4 c0, Vec4 c1, Vec4 c2, Vec4 c3)
{
    Mat4 result;

    mat4(result, 0, 0) = c0.x;
    mat4(result, 1, 0) = c0.y;
    mat4(result, 2, 0) = c0.z;
    mat4(result, 3, 0) = c0.w;

    mat4(result, 0, 1) = c1.x;
    mat4(result, 1, 1) = c1.y;
    mat4(result, 2, 1) = c1.z;
    mat4(result, 3, 1) = c1.w;
    
    mat4(result, 0, 2) = c2.x;
    mat4(result, 1, 2) = c2.y;
    mat4(result, 2, 2) = c2.z;
    mat4(result, 3, 2) = c2.w;

    mat4(result, 0, 3) = c3.x;
    mat4(result, 1, 3) = c3.y;
    mat4(result, 2, 3) = c3.z;
    mat4(result, 3, 3) = c3.w;

    
    return result;
}




Mat4 mat4_init(float a00, float a01, float a02, float a03,
	       float a10, float a11, float a12, float a13,
	       float a20, float a21, float a22, float a23,
	       float a30, float a31, float a32, float a33)
{
    Mat4 result;

    mat4(result, 0, 0) = a00;
    mat4(result, 0, 1) = a01;
    mat4(result, 0, 2) = a02;
    mat4(result, 0, 3) = a03;

    mat4(result, 1, 0) = a10;
    mat4(result, 1, 1) = a11;
    mat4(result, 1, 2) = a12;
    mat4(result, 1, 3) = a13;

    mat4(result, 2, 0) = a20;
    mat4(result, 2, 1) = a21;
    mat4(result, 2, 2) = a22;
    mat4(result, 2, 3) = a23;

    mat4(result, 3, 0) = a30;
    mat4(result, 3, 1) = a31;
    mat4(result, 3, 2) = a32;
    mat4(result, 3, 3) = a33;

    return result;
}

Mat4 mat4_from_mat3(Mat3 a)
{
    Mat4 result = mat4_create_identity();

    for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
	    mat4(result, i, j) = mat3(a, i, j);
	}
    }
    
    return result;
}

void mat4_print_elements(Mat4 m)
{
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 4; j++) {
	    printf("element %d, %d is %f\n", i, j, mat4(m, i, j));
	}
    }
}


float deg_to_rad(float angle)
{
    return (angle * MYPI) / 180.0f;
}

float cos_deg(float angle)
{
    return cos(deg_to_rad(angle));
}
float sin_deg(float angle)
{
    return sin(deg_to_rad(angle));
}
float tan_deg(float angle)
{
    return tan(deg_to_rad(angle));
}

Quaternion quaternion_init(float x, float y, float z, float w)
{
    Quaternion result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

Quaternion quaternion_from_vector_scalar(Vec3 v, float s)
{
    Quaternion result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    result.w = s;
    return result;
}

Quaternion quaternion_add(Quaternion a, Quaternion b)
{
    Quaternion result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

Quaternion quaternion_scale(Quaternion q, float s)
{
    Quaternion result;
    result.x = q.x*s;
    result.y = q.y*s;
    result.z = q.z*s;
    result.w = q.w*s;
    return result;
}

Vec3 vec3_from_quaternion(Quaternion q)
{
    Vec3 result;
    result.x = q.x;
    result.y = q.y;
    result.z = q.z;
    return result;
}

float scalar_from_quaternion(Quaternion q)
{
    return q.w;
}

Quaternion quaternion_mult(Quaternion a, Quaternion b)
{
    //use a nice formula here
    //though it's a little slower probably
    Quaternion result;
    
    Vec3 va = vec3_from_quaternion(a);
    Vec3 vb = vec3_from_quaternion(b);
    
    Vec3 cross = vec3_cross(va,vb);
    float dot = vec3_dot(va,vb);

    Vec3 sv1 = vec3_scale(va,b.w);
    Vec3 sv2 = vec3_scale(vb,a.w);

    Vec3 q_vector = vec3_add(cross, vec3_add(sv1, sv2));
    float q_scalar = a.w*b.w - dot;

    result = quaternion_from_vector_scalar(q_vector, q_scalar);
    
    return result;
}

Quaternion quaternion_conjugate(Quaternion q)
{
    Vec3 q_vector = vec3_from_quaternion(q);
    q_vector = vec3_scale(q_vector, -1.0f);
    Quaternion result = quaternion_from_vector_scalar(q_vector, q.w);
    return result;
}

Quaternion quaternion_inverse(Quaternion q)
{
    Quaternion q_conjugate = quaternion_conjugate(q);
    float mag = quaternion_mag(q);
    float inv_mag_squared = 1.0f/(mag*mag);
    Quaternion result = quaternion_scale(q_conjugate, inv_mag_squared);
    return result;
    
}

Quaternion quaternion_pure(Vec3 v)
{
    Quaternion result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    result.w = 0.0f;
    return result;
}

float quaternion_mag(Quaternion q)
{
    Quaternion q_conjugate = quaternion_conjugate(q);
    Quaternion product = quaternion_mult(q, q_conjugate);
    float result = sqrt(product.w);
    return result;
}
