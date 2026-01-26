/* date = January 25th 2026 11:53 am */

#define PI 3.14159f

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


internal_f void
Mat4LookAtLH(float* m, vec3_t eye, vec3_t at, vec3_t up)
{
    vec3_t zaxis = Vec3Normalize(Vec3Sub(at, eye));   // forward
    vec3_t xaxis = Vec3Normalize(Vec3Cross(up, zaxis));
    vec3_t yaxis = Vec3Cross(zaxis, xaxis);
	
    m[0]  = xaxis.x;
    m[1]  = xaxis.y;
    m[2]  = xaxis.z;
    m[3]  = 0;
	
    m[4]  = yaxis.x;
    m[5]  = yaxis.y;
    m[6]  = yaxis.z;
    m[7]  = 0;
	
    m[8]  = zaxis.x;
    m[9]  = zaxis.y;
    m[10] = zaxis.z;
    m[11] = 0;
	
    m[12] = -Vec3Dot(xaxis, eye);
    m[13] = -Vec3Dot(yaxis, eye);
    m[14] = -Vec3Dot(zaxis, eye);
    m[15] = 1;
}


internal_f void
Mat4Identity(f32 *m)
{
	bytes_set(m, 0, sizeof(float) * 16);
    m[0]  = 1.0f;
    m[5]  = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
}

internal_f void
Mat4Mul(float* out, const float* a, const float* b)
{
    float r[16];
	
    for(int col = 0; col < 4; ++col)
    {
        for(int row = 0; row < 4; ++row)
        {
            r[row + col*4] =
                a[row + 0*4] * b[0 + col*4] +
                a[row + 1*4] * b[1 + col*4] +
                a[row + 2*4] * b[2 + col*4] +
                a[row + 3*4] * b[3 + col*4];
        }
    }
	
    bytes_copy(out, r, sizeof(r));
}

internal_f void
Mat4PerspectiveLH(float* m, float fovY, float aspect, float zn, float zf)
{
    float yScale = 1.0f / tanf(fovY * 0.5f);
    float xScale = yScale / aspect;
	
    bytes_set(m, 0, sizeof(float) * 16);
	
    m[0]  = xScale;
    m[5]  = yScale;
    m[10] = zf / (zf - zn);
    m[11] = 1.0f;
    m[14] = -zn * zf / (zf - zn);
}


internal_f void
TransformToMatrix(float* out, transform_t t)
{
    float T[16];
    float S[16];
    float R[16]; // later
	
    Mat4Identity(T);
    Mat4Identity(S);
	
    // Translation (column-major)
    T[12] = t.position.x;
    T[13] = t.position.y;
    T[14] = t.position.z;
	
    // Scale
    S[0]  = t.scale.x;
    S[5]  = t.scale.y;
    S[10] = t.scale.z;
	
    // For now, skip rotation
    float TS[16];
    Mat4Mul(TS, T, S);
	
    bytes_copy(out, TS, sizeof(float)*16);
}

internal_f void
Mat4RotationY(float* m, float angle)
{
    Mat4Identity(m);
	
    float c = cosf(angle);
    float s = sinf(angle);
	
    m[0]  =  c;
    m[2]  =  s;
    m[8]  = -s;
    m[10] =  c;
}