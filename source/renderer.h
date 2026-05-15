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


struct mesh_component_t;
struct mesh_importer_data_t;

/** Callback signature so we know when the scene proxy on the Renderer memory has been created and the mesh is ready to be rendered. */
typedef void (*scene_proxy_set_t)(void*);

struct engine_shared_data_t;
struct mesh_t;

global_f bool
RendererInit(arena_t *render_memory);

global_f void
RendererUpdate(engine_shared_data_t *engine_data);

global_f void
RendererComputeImportedMesh(mesh_importer_data_t *importer_data, mesh_component_t *_mesh_component, transform_t *transform, const char* _texture_name, scene_proxy_set_t set_callback);

global_f scene_proxy_t*
RendererCreateSceneProxy();

global_f arena_t* 
RendererGetMemory();
