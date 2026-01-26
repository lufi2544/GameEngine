/* date = January 26th 2026 0:13 pm */


struct texture_uv
{
	f32 u;
	f32 v;
};

struct face
{
	s32 a;
	s32 b;
	s32 c;
	
	texture_uv a_uv;
	texture_uv b_uv;
	texture_uv c_uv;
	
};


typedef u32 color_t;

// We use the  UV coordinate method for mapping the texture to the faces.
struct texture
{
	color_t *buffer;
	s32 w;
	s32 h;	
};


struct mesh
{
	vec3 *verteces;
	face *faces;
	texture_uv *uv_coords;
	texture *texture;
	
	u32 vertex_num;
	u32 face_num;
	u32 uv_coords_num;
	u32 texture_id;
	
	vec3 rotation;
	vec3 scale;
	vec3 location;
	vec3 translation; // TODO see if this is needed...	
};
