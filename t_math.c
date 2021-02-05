#include "t_math.h"

//column major or row major?

Vec2 vec2_init(float x, float y)
{
    Vec2 result;
    result.x = x;
    result.y = y;
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

Vec4 vec4_init(float x, float y, float z, float w)
{
    Vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

Vec2 vec2_scale(float s, Vec2 v)
{
    Vec2 result = v;
    result.x *= s;
    result.y *= s;
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

float vec2_mag(Vec2 v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

Vec2 vec2_normalize(Vec2 v)
{
    float mag = vec2_mag(v);
    Vec2 result = v;
    if (mag != 0.0f) {
	result.x /= mag;
	result.y /= mag;
    }
    return result;
}

float vec2_dot(Vec2 a, Vec2 b)
{
    return a.x*b.x + a.y*b.y;
}

float vec2_angle_between(Vec2 a, Vec2 b)
{
    // NOTE this is given in radians
    // dot(a,b) = mag(a)*mag(b)cos(theta)
    float dot = vec2_dot(a, b);
    float mag_a = vec2_mag(a);
    float mag_b = vec2_mag(b);
    return acos(dot/(mag_a*mag_b));
}

float vec2_angle(Vec2 a)
{
    //NOTE this is given in radians
    //tan = opp/adj
    float result;
    a = vec2_normalize(a);
    if (a.x == 0.0f) {
	if (a.y == 1.0f) {
	    result = M_PI_2;
	} else {
	    result = 3*M_PI_2;
	}
    } else if (a.y == 0.0f) {
	if (a.x == 1.0f) {
	    result = 0.0f;
	} else {
	    result = M_PI;
	}
    } else {
	result = atan(a.y/a.x);
    }
    return result;
}



Vec3 vec3_scale(float s, Vec3 v)
{
    Vec3 result = v;
    result.x *= s;
    result.y *= s;
    result.z *= s;
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

float vec3_mag(Vec3 v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vec3 vec3_normalize(Vec3 v)
{
    float mag = vec3_mag(v);
    Vec3 result = v;
    if (mag != 0.0f) {
	result.x /= mag;
	result.y /= mag;
	result.z /= mag;
    }
    return result;
}


float vec3_dot(Vec3 a, Vec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    //note the cross product of parallel vectors is zero
    Vec3 result;

    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    
    return result;
}

float vec3_scalar_triple(Vec3 a, Vec3 b, Vec3 c)
{
    //this looks useful for getting angles
    float result;    
    result = vec3_dot(vec3_cross(a,b), c);
    return result;
}

#if 0
float mat3_det(Mat3 m)
{
    return ();
}
#endif

void mat4_create_translation(Mat4 a, Vec3 t)
{
    a[0][3] = t.x;
    a[1][3] = t.y;
    a[2][3] = t.z;
}

void mat4_create_identity(Mat4 a)
{
    a[0][0] = 1.0f;
    a[0][1] = 0.0f;
    a[0][2] = 0.0f;
    a[0][3] = 0.0f;

    a[1][0] = 0.0f;
    a[1][1] = 1.0f;
    a[1][2] = 0.0f;
    a[1][3] = 0.0f;

    a[2][0] = 0.0f;
    a[2][1] = 0.0f;
    a[2][2] = 1.0f;
    a[2][3] = 0.0f;

    a[3][0] = 0.0f;
    a[3][1] = 0.0f;
    a[3][2] = 0.0f;
    a[3][3] = 1.0f;
}

void mat4_create_scale(Mat4 a, float scalar)
{
    a[0][0] = scalar;
    a[0][1] = 0.0f;
    a[0][2] = 0.0f;
    a[0][3] = 0.0f;

    a[1][0] = 0.0f;
    a[1][1] = scalar;
    a[1][2] = 0.0f;
    a[1][3] = 0.0f;

    a[2][0] = 0.0f;
    a[2][1] = 0.0f;
    a[2][2] = scalar;
    a[2][3] = 0.0f;

    a[3][0] = 0.0f;
    a[3][1] = 0.0f;
    a[3][2] = 0.0f;
    a[3][3] = 1.0f;
}


//check these for column major
void mat4_create_x_rotation(Mat4 a, float angle)
{
    mat4_create_identity(a);
    a[1][1] = cos(angle);
    a[1][2] = -sin(angle);
    a[2][1] = sin(angle);
    a[2][2] = cos(angle);    
}

void mat4_create_y_rotation(Mat4 a, float angle)
{
    mat4_create_identity(a);
    a[0][0] = cos(angle);
    a[0][2] = sin(angle);
    a[1][1] = 1;
    a[2][0] = -sin(angle);
    a[2][2] = cos(angle);    
}

void mat4_create_z_rotation(Mat4 a, float angle)
{
    mat4_create_identity(a);
    a[0][0] = cos(angle);
    a[1][1] = cos(angle);
    a[1][0] = sin(angle);
    a[0][1] = -sin(angle);    
}


//check for column major
void mat4_create_perspective(Mat4 a, float fovy, float aspect_ratio, float near, float far)
{

    float fovy_degrees = (fovy * M_PI) / 180.0;
    mat4_create_identity(a);
    float g = 1.0f / tan(fovy_degrees * 0.5f);
    float k = far / (far - near);
    a[0][0] = g / aspect_ratio;
    a[1][1] = g;
    a[2][2] = k;
    a[2][3] = -near * k;
    a[3][2] = 1.0f;
    a[3][3] = 0.0f;
}

void mat4_create_infinite_perspective();


//could create a temporary matrix in here to do this rather than
//the in-out stuff
void mat4_transpose(Mat4 in, Mat4 out)
{
    
    out[0][0] = in[0][0];
    out[0][1] = in[1][0];
    out[0][2] = in[2][0];
    out[0][3] = in[3][0];

    out[1][0] = in[0][1];
    out[1][1] = in[1][1];
    out[1][2] = in[2][1];
    out[1][3] = in[3][1];

    out[2][0] = in[0][2];
    out[2][1] = in[1][2];
    out[2][2] = in[2][2];
    out[2][3] = in[3][2];

    out[3][0] = in[0][3];
    out[3][1] = in[1][3];
    out[3][2] = in[2][3];
    out[3][3] = in[3][3];
}

void mat4_transpose_in_place(Mat4 in)
{
    //Mat4 out;

    //this won't actually work!
    for (int i = 0; i < 3; i++) {
	for (int j = i+1; j < 4; j++) {

	    //why you no work?
	    float tmp1 = in[i][j];
	    in[i][j] = in[j][i];
	    in[j][i] = tmp1;


	}
    }
    
}

void mat4_compare(Mat4 a, Mat4 b)
{
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 4; j++) {
	    float a1 = a[i][j];
	    float b1 = b[i][j];
	    int same = a1==b1;
	}
    }
}


//check for column major
void mat4_mult(Mat4 a, Mat4 b, Mat4 c)
{
    c[0][0] = a[0][0]*b[0][0] +
	      a[0][1]*b[1][0] +
	      a[0][2]*b[2][0] +
	      a[0][3]*b[3][0];
    c[0][1] = a[0][0]*b[0][1] +
	      a[0][1]*b[1][1] +
	      a[0][2]*b[2][1] +
	      a[0][3]*b[3][1];
    c[0][2] = a[0][0]*b[0][2] +
	      a[0][1]*b[1][2] +
	      a[0][2]*b[2][2] +
	      a[0][3]*b[3][2];
    c[0][3] = a[0][0]*b[0][3] +
	      a[0][1]*b[1][3] +
	      a[0][2]*b[2][3] +
	      a[0][3]*b[3][3];

    c[1][0] = a[1][0]*b[0][0] +
	      a[1][1]*b[1][0] +
	      a[1][2]*b[2][0] +
	      a[1][3]*b[3][0];
    c[1][1] = a[1][0]*b[0][1] +
	      a[1][1]*b[1][1] +
	      a[1][2]*b[2][1] +
	      a[1][3]*b[3][1];
    c[1][2] = a[1][0]*b[0][2] +
	      a[1][1]*b[1][2] +
	      a[1][2]*b[2][2] +
	      a[1][3]*b[3][2];
    c[1][3] = a[1][0]*b[0][3] +
	      a[1][1]*b[1][3] +
	      a[1][2]*b[2][3] +
	      a[1][3]*b[3][3];

    c[2][0] = a[2][0]*b[0][0] +
	      a[2][1]*b[1][0] +
	      a[2][2]*b[2][0] +
	      a[2][3]*b[3][0];
    c[2][1] = a[2][0]*b[0][1] +
	      a[2][1]*b[1][1] +
	      a[2][2]*b[2][1] +
	      a[2][3]*b[3][1];
    c[2][2] = a[2][0]*b[0][2] +
	      a[2][1]*b[1][2] +
	      a[2][2]*b[2][2] +
	      a[2][3]*b[3][2];
    c[2][3] = a[2][0]*b[0][3] +
	      a[2][1]*b[1][3] +
	      a[2][2]*b[2][3] +
	      a[2][3]*b[3][3];

    c[3][0] = a[3][0]*b[0][0] +
	      a[3][1]*b[1][0] +
	      a[3][2]*b[2][0] +
	      a[3][3]*b[3][0];
    c[3][1] = a[3][0]*b[0][1] +
	      a[3][1]*b[1][1] +
	      a[3][2]*b[2][1] +
	      a[3][3]*b[3][1];
    c[3][2] = a[3][0]*b[0][2] +
	      a[3][1]*b[1][2] +
	      a[3][2]*b[2][2] +
	      a[3][3]*b[3][2];
    c[3][3] = a[3][0]*b[0][3] +
	      a[3][1]*b[1][3] +
	      a[3][2]*b[2][3] +
	      a[3][3]*b[3][3];


}


void mat4_copy(Mat4 a, Mat4 b)
{
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 4; j++) {
	    b[i][j] = a[i][j];
	}
    }
}

