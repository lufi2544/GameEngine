/* date = January 23rd 2026 4:35 pm */


#include "scene_proxy.h"
#include "render_command.h"
#include "frame_data.h"

// TODO: Add the mesh list to the engie_shared_data.

enum enum_renderer_flags : u32
{
	RendererFlag_None,
	RendererFlag_WireFrame, 
				
	RendererFlag_Num
};


global_f bool
RendererInit();

global_f void
RendererUpdate(engine_shared_data_t *engine_data);

global_f void 
RendererComputeImportedMesh(engine_shared_data_t *engine_data, mesh_t *_mesh, const char* _texture_name);
