/* date = January 25th 2026 11:53 am */

struct vec3
{	
	f32 x, y, z;
};

internal_f vec3
Vec3Sub(vec3 a, vec3 b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

internal_f float
Vec3Dot(vec3 a, vec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

internal_f vec3
Vec3Cross(vec3 a, vec3 b)
{
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

internal_f vec3
Vec3Normalize(vec3 v)
{
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    return { v.x/len, v.y/len, v.z/len };
}


internal_f void
Mat4LookAtLH(float* m, vec3 eye, vec3 at, vec3 up)
{
    vec3 zaxis = Vec3Normalize(Vec3Sub(at, eye));   // forward
    vec3 xaxis = Vec3Normalize(Vec3Cross(up, zaxis));
    vec3 yaxis = Vec3Cross(zaxis, xaxis);
	
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