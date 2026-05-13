/* date = January 25th 2026 11:53 am */

#define PI 3.14159f
#include "math.h"

struct  vec3_t
{	
	f32 x, y, z;
};

struct transform_t
{
	vec3_t position;
	vec3_t rotation;
	vec3_t scale;	
};

forward_f vec3_t Vec3Identity();

global_f transform_t
TransformIdentity()
{
	transform_t result;
	
	result.position = Vec3Identity();
	result.rotation = Vec3Identity();
	result.scale.x = 1.0f;
	result.scale.y = 1.0f;
	result.scale.z = 1.0f;

	
	return result;
}

global_f vec3_t Vec3Identity()
{
	vec3_t result;
		
	result.x = 0.0f;
	result.y = 0.0f;
	result.z = 0.0f;
	
	return result;
}

global_f vec3_t
Vec3Sub(vec3_t a, vec3_t b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

global_f float
Vec3Dot(vec3_t a, vec3_t b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

global_f vec3_t
Vec3Cross(vec3_t a, vec3_t b)
{
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

global_f vec3_t
Vec3Normalize(vec3_t v)
{
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return { v.x/len, v.y/len, v.z/len };
}


global_f void
Vec3Add(vec3_t a, vec3_t b, vec3_t *out)
{
	out->x = a.x + b.x;
	out->y = a.y + b.y;
	out->z = a.z + b.z;
}

global_f void
Vec3MultiplyF32(vec3_t *a, f32 b)
{
	(*a).x *= b;
	(*a).y *= b;
	(*a).z *= b;
}


////////////////////
/// Matrix 
///////////////////

/**
 * The matrices are in column major form, so when multiplied for begin applied to a vec3
 * we have to do it the opposite way.
 * 
 * Column Major: 
 * View * Proj * Translation * Rotation * Scale * Vertex -> opeartions have to be done in the opposite way
 * 
 * Row Major : 
 * Vertex * Scale * Rotation * Translation * Proj * View -> this is a more "common" equation way. 
 * 
*/

struct mat4_t;

global_f void
Mat4Identity(mat4_t *m);

struct mat4_t
{
	mat4_t()
	{
		Mat4Identity(this);
	}	
	
	f32 d[16];
};

mat4_t Mat4Identity()
{
	mat4_t m = {};
    m.d[0]  = 1.0f;
    m.d[5]  = 1.0f;
    m.d[10] = 1.0f;
    m.d[15] = 1.0f;
	
    return m;
}

global_f void
Mat4Identity(mat4_t *m)
{
	bytes_set(m->d, 0, sizeof(float) * 16);
	
    m->d[0]  = 1.0f;
    m->d[5]  = 1.0f;
    m->d[10] = 1.0f;
    m->d[15] = 1.0f;
}


global_f void
Mat4Mul(mat4_t* out, const mat4_t* a, const mat4_t* b)
{	
    for (int col = 0; col < 4; ++col)
    {
        for (int row = 0; row < 4; ++row)
        {
            out->d[row + col*4] =
                a->d[row + 0*4] * b->d[0 + col*4] +
                a->d[row + 1*4] * b->d[1 + col*4] +
                a->d[row + 2*4] * b->d[2 + col*4] +
                a->d[row + 3*4] * b->d[3 + col*4];
        }
    }	
}


global_f void
Mat4PerspectiveLH(mat4_t* m, float fovY, float aspect, float zn, float zf)
{
    float yScale = 1.0f / tanf(fovY * 0.5f);
    float xScale = yScale / aspect;		
	
    m->d[0]  = xScale;
    m->d[5]  = yScale;
    m->d[10] = zf / (zf - zn);
    m->d[11] = 1.0f;
    m->d[14] = -zn * zf / (zf - zn);
}


global_f void
Mat4LookAtLH(mat4_t* out, vec3_t eye, vec3_t at, vec3_t up)
{
	// the camera coordinate system axis.
    vec3_t zaxis = Vec3Normalize(Vec3Sub(at, eye));
    vec3_t xaxis = Vec3Normalize(Vec3Cross(up, zaxis));
    vec3_t yaxis = Vec3Cross(zaxis, xaxis);
	
	
	// setting the coordinate system in the matrix so we multiply the input point to the DotProduct of the coordinate system axis.
	// x = dot(px, xaxis.x)
    out->d[0]  = xaxis.x;
    out->d[1]  = xaxis.y;
    out->d[2]  = xaxis.z;
	
    out->d[4]  = yaxis.x;
    out->d[5]  = yaxis.y;
    out->d[6]  = yaxis.z;
	
    out->d[8]  = zaxis.x;
    out->d[9]  = zaxis.y;
    out->d[10] = zaxis.z;
	
	
	// Choose the translation so that the camera position maps to the origin in camera space:
	// For P = eye:
	// x_camera = dot(P, xaxis) + tx = 0  =>  tx = -dot(xaxis, eye)
    out->d[12] = -Vec3Dot(xaxis, eye);
    out->d[13] = -Vec3Dot(yaxis, eye);
    out->d[14] = -Vec3Dot(zaxis, eye);
}

global_f void
Mat4OrthographicLH(mat4_t* m, float width, float height, float zn, float zf)
{
    m->d[0]  = 2.0f / width;
    m->d[5]  = 2.0f / height;
    m->d[10] = 1.0f / (zf - zn);
    m->d[14] = -zn / (zf - zn);
    m->d[15] = 1.0f;
}

global_f void
Mat4RotationX(mat4_t *m, float angle)
{	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m->d[5]  = c;
    m->d[6]  = s;
    m->d[9]  = -s;
    m->d[10] = c;
}

global_f void
Mat4RotationY(mat4_t *m, float angle)
{	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m->d[0]  = c;
    m->d[2]  = -s;
    m->d[8]  = s;
    m->d[10] = c;
}

global_f void
Mat4RotationZ(mat4_t* m, float angle)
{	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m->d[0] = c;
    m->d[1] = s;
    m->d[4] = -s;
    m->d[5] = c;
}


global_f void
Mat4Scale(mat4_t* out, vec3_t s)
{
    Mat4Identity(out);
    out->d[0]  = s.x;
    out->d[5]  = s.y;
    out->d[10] = s.z;
}

global_f void
Mat4Translation(mat4_t* out, vec3_t p)
{
    out->d[12] = p.x;
    out->d[13] = p.y;
    out->d[14] = p.z;
}

global_f void
Mat4Inverse(mat4_t *out, const mat4_t *m)
{
    const float *M = m->d;
    float inv[16];

    inv[0]  =  M[5]*M[10]*M[15] - M[5]*M[11]*M[14] - M[9]*M[6]*M[15] + M[9]*M[7]*M[14] + M[13]*M[6]*M[11] - M[13]*M[7]*M[10];
    inv[1]  = -M[1]*M[10]*M[15] + M[1]*M[11]*M[14] + M[9]*M[2]*M[15] - M[9]*M[3]*M[14] - M[13]*M[2]*M[11] + M[13]*M[3]*M[10];
    inv[2]  =  M[1]*M[6]*M[15]  - M[1]*M[7]*M[14]  - M[5]*M[2]*M[15] + M[5]*M[3]*M[14] + M[13]*M[2]*M[7]  - M[13]*M[3]*M[6];
    inv[3]  = -M[1]*M[6]*M[11]  + M[1]*M[7]*M[10]  + M[5]*M[2]*M[11] - M[5]*M[3]*M[10] - M[9]*M[2]*M[7]   + M[9]*M[3]*M[6];
    inv[4]  = -M[4]*M[10]*M[15] + M[4]*M[11]*M[14] + M[8]*M[6]*M[15] - M[8]*M[7]*M[14] - M[12]*M[6]*M[11] + M[12]*M[7]*M[10];
    inv[5]  =  M[0]*M[10]*M[15] - M[0]*M[11]*M[14] - M[8]*M[2]*M[15] + M[8]*M[3]*M[14] + M[12]*M[2]*M[11] - M[12]*M[3]*M[10];
    inv[6]  = -M[0]*M[6]*M[15]  + M[0]*M[7]*M[14]  + M[4]*M[2]*M[15] - M[4]*M[3]*M[14] - M[12]*M[2]*M[7]  + M[12]*M[3]*M[6];
    inv[7]  =  M[0]*M[6]*M[11]  - M[0]*M[7]*M[10]  - M[4]*M[2]*M[11] + M[4]*M[3]*M[10] + M[8]*M[2]*M[7]   - M[8]*M[3]*M[6];
    inv[8]  =  M[4]*M[9]*M[15]  - M[4]*M[11]*M[13] - M[8]*M[5]*M[15] + M[8]*M[7]*M[13] + M[12]*M[5]*M[11] - M[12]*M[7]*M[9];
    inv[9]  = -M[0]*M[9]*M[15]  + M[0]*M[11]*M[13] + M[8]*M[1]*M[15] - M[8]*M[3]*M[13] - M[12]*M[1]*M[11] + M[12]*M[3]*M[9];
    inv[10] =  M[0]*M[5]*M[15]  - M[0]*M[7]*M[13]  - M[4]*M[1]*M[15] + M[4]*M[3]*M[13] + M[12]*M[1]*M[7]  - M[12]*M[3]*M[5];
    inv[11] = -M[0]*M[5]*M[11]  + M[0]*M[7]*M[9]   + M[4]*M[1]*M[11] - M[4]*M[3]*M[9]  - M[8]*M[1]*M[7]   + M[8]*M[3]*M[5];
    inv[12] = -M[4]*M[9]*M[14]  + M[4]*M[10]*M[13] + M[8]*M[5]*M[14] - M[8]*M[6]*M[13] - M[12]*M[5]*M[10] + M[12]*M[6]*M[9];
    inv[13] =  M[0]*M[9]*M[14]  - M[0]*M[10]*M[13] - M[8]*M[1]*M[14] + M[8]*M[2]*M[13] + M[12]*M[1]*M[10] - M[12]*M[2]*M[9];
    inv[14] = -M[0]*M[5]*M[14]  + M[0]*M[6]*M[13]  + M[4]*M[1]*M[14] - M[4]*M[2]*M[13] - M[12]*M[1]*M[6]  + M[12]*M[2]*M[5];
    inv[15] =  M[0]*M[5]*M[10]  - M[0]*M[6]*M[9]   - M[4]*M[1]*M[10] + M[4]*M[2]*M[9]  + M[8]*M[1]*M[6]   - M[8]*M[2]*M[5];

    float det = M[0]*inv[0] + M[1]*inv[4] + M[2]*inv[8] + M[3]*inv[12];
    if (fabsf(det) < 1e-6f) { Mat4Identity(out); return; }

    det = 1.0f / det;
    for (int i = 0; i < 16; i++) out->d[i] = inv[i] * det;
}

global_f void
TransformToMatrix(mat4_t* out, transform_t t)
{
    mat4_t T, S, Rx, Ry, Rz;
    mat4_t Ryx, Rzyx;
    mat4_t RS;
	
    Mat4Translation(&T, t.position);
    Mat4Scale(&S, t.scale);
	
    Mat4RotationX(&Rx, t.rotation.x);
    Mat4RotationY(&Ry, t.rotation.y);
    Mat4RotationZ(&Rz, t.rotation.z);
	
    // R = Rz * Ry * Rx
    Mat4Mul(&Ryx, &Ry, &Rx);
    Mat4Mul(&Rzyx, &Rz, &Ryx);
	
    // RS = R * S
    Mat4Mul(&RS, &Rzyx, &S);
	
    // TRS = T * R * S
    Mat4Mul(out, &T, &RS);
}