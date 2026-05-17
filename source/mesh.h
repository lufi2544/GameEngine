/* date = January 26th 2026 0:13 pm */


#include "mesh_importer.h"

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


struct mesh_component_t
{
	scene_proxy_t *scene_proxy;
	
	u32 vertex_num;
	u32 face_num;
	u32 uv_coords_num;
	u32 flags;
	
	string_t *file_name;
	string_t *texture_name;
};


global_f mesh_component_t
MeshComponent()
{
	mesh_component_t result;
		
	result. scene_proxy = 0;
	
	result.vertex_num = 0;
	result.face_num = 0;
	result.uv_coords_num = 0;
	result.flags = 0;
	
	result.file_name = 0;
	result.texture_name = 0;
	
	return result;
}

callback_f void
mesh_component_t_OnAttached(actor_t *actor, void* data);