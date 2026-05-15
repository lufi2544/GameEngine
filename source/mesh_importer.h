/* date = May 15th 2026 4:53 pm */


struct face_t;
struct texture_uv_t;
struct texture_t;


struct mesh_importer_data_t
{
	vec3_t *verteces;
	face_t *faces;
	texture_uv_t *uv_coords;
	texture_t *texture;
	
	u32 vertex_num;
	u32 face_num;
	u32 uv_coords_num;
	u32 texture_id;
	
	string_t path;
	
	u32 flags;	
};

