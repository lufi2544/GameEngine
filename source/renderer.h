/* date = January 23rd 2026 4:35 pm */


#include "render_command.h"

#include "scene_proxy.h"
#include "scene.h"
#include "frame_data.h"

// TODO: Add the mesh list to the engie_shared_data.

enum enum_renderer_flags : u32
{
	RendererFlag_None,
	RendererFlag_WireFrame, 
				
	RendererFlag_Num
};


struct engine_shared_data_t;
struct mesh_t;

global_f bool
RendererInit(arena_t render_memory);

global_f void
RendererUpdate(engine_shared_data_t *engine_data);


// Must implement the 
global_f void
RendererComputeImportedMesh(mesh_t *_mesh, const char* _texture_name);


// gpu_mesh implemented per platform.
global_f void
RendererEnqueueCreateSceneProxyFromMesh(mesh_t *mesh, void* gpu_mesh);

global_f scene_proxy_t*
RendererCreateSceneProxy();


global_f void
RendererRegisterSceneProxyInScene(scene_proxy_t *proxy);


global_f arena_t* 
RendererGetMemory();
