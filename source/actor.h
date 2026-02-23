/* date = February 22nd 2026 8:15 pm */



struct scene_proxy_t;

struct actor_t
{	
	mesh_t* mesh;
	
	// Only written by the render thread
	scene_proxy_t *scene_proxy;
};
