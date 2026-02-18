/* date = January 26th 2026 0:13 pm */


struct texture_uv_t
{
	f32 u;
	f32 v;
};

struct face_t
{
	s32 a;
	s32 b;
	s32 c;
	
	texture_uv_t a_uv;
	texture_uv_t b_uv;
	texture_uv_t c_uv;
	
};


typedef u32 color_t;

// We use the  UV coordinate method for mapping the texture to the faces.
struct texture_t
{
	color_t *buffer;
	s32 w;
	s32 h;	
};


struct mesh_t
{
	vec3_t *verteces;
	face_t *faces;
	texture_uv_t *uv_coords;
	texture_t *texture;
	
	u32 vertex_num;
	u32 face_num;
	u32 uv_coords_num;
	u32 texture_id;
	
	
	transform_t transform;
	string_t path;
	
	u32 flags;	
};
