/* date = February 22nd 2026 8:06 pm */


/// Data structure that is handled by the render thread so we can manage the scene data and render it.
struct scene_proxy_t
{			
	transform_t transform;	
	u32 render_flags;
};

struct scene_t
{
	// TODO: We will track the free scene proxies in the scene somehow.
	scene_proxy_t *scene_proxies;
	u32 scene_proxy_current;
	u32 scene_proxy_max_num;	
};


global_f scene_proxy_t*
CreateSceneProxy()
{
	if(g_scene->scene_proxy_current + 1 >= g_scene->scene_proxy_max_num)
	{
		// TODO LOG 
		return nullptr;
	}
	
	scene_proxy_t *result;		
	result = g_scene->scene_proxies + g_scene->scene_proxy_current++;	
	return result;
}

global_f void 
UpateProxyTransform(scene_proxy_t *proxy, transform_t *transform)
{
	
	struct update_proxy_transform_t
	{
		scene_proxy_t *proxy;
		transform_t transform;
	};
	
	command_t command = [](void* data)
	{
		update_proxy_transform_t* args = (update_proxy_transform_t*)(data);
		args->proxy->transform = args->transform;
	};
	
	
	if(update_proxy_transform_t* args = 
	   (update_proxy_transform_t*)RenderMailBoxRequestArgsMemory(g_engine_reserver, sizeof(update_proxy_transform_t)))
	{
		args->proxy = proxy;
		args->transform = *transform;
		
		render_command_t render_command;
		render_command.command = command;
		render_command.user_data = &args;
		
				
		RenderMailBoxCommitCommand(g_engine_reserver, &render_command);
	}	
	// TODO LOG
}