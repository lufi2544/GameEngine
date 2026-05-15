/* date = January 26th 2026 0:08 pm */

struct mesh_t;


// TODO Find a better place for this.
struct mesh_import_data_t
{
	u32 pending_importing_meshes;
	u32 last_frame_importing_meshes;
};

struct engine_shared_data_t
{
	memory_t *memory;	
	mesh_import_data_t mesh_import_data;
};


