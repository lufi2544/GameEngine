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

internal_f vec3_t
Vec3Sub(vec3_t a, vec3_t b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

internal_f float
Vec3Dot(vec3_t a, vec3_t b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

internal_f vec3_t
Vec3Cross(vec3_t a, vec3_t b)
{
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

internal_f vec3_t
Vec3Normalize(vec3_t v)
{
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return { v.x/len, v.y/len, v.z/len };
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

internal_f void
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

internal_f void
Mat4Identity(mat4_t *m)
{
	bytes_set(m->d, 0, sizeof(float) * 16);
	
    m->d[0]  = 1.0f;
    m->d[5]  = 1.0f;
    m->d[10] = 1.0f;
    m->d[15] = 1.0f;
}


internal_f void
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


internal_f void
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


internal_f void
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

internal_f void
Mat4RotationX(mat4_t *m, float angle)
{	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m->d[5]  = c;
    m->d[6]  = s;
    m->d[9]  = -s;
    m->d[10] = c;
}

internal_f void
Mat4RotationY(mat4_t *m, float angle)
{	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m->d[0]  = c;
    m->d[2]  = -s;
    m->d[8]  = s;
    m->d[10] = c;
}

internal_f void
Mat4RotationZ(mat4_t* m, float angle)
{	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m->d[0] = c;
    m->d[1] = s;
    m->d[4] = -s;
    m->d[5] = c;
}


internal_f void
Mat4Scale(mat4_t* out, vec3_t s)
{
    Mat4Identity(out);
    out->d[0]  = s.x;
    out->d[5]  = s.y;
    out->d[10] = s.z;
}

internal_f void
Mat4Translation(mat4_t* out, vec3_t p)
{
    out->d[12] = p.x;
    out->d[13] = p.y;
    out->d[14] = p.z;
}

internal_f void
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