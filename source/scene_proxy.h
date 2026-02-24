/* date = February 22nd 2026 8:06 pm */


/// Data structure that is handled by the render thread so we can manage the scene data and render it.
struct scene_proxy_t
{			
	transform_t transform;	
	u32 render_flags;
};


global_f void 
UpdateProxyTransform(scene_proxy_t *proxy, transform_t *transform)
{	
	if(!proxy)
	{
		return;
	}
	
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
		render_command.user_data = args;
		
				
		RenderMailBoxCommitCommand(g_engine_reserver, &render_command);
	}	
	// TODO LOG
}



global_f void
CreateSceneProxy_Enqueue(struct actor_t *actor)
{
	struct create_scene_proxy_t
	{
		actor_t* actor;		
	};
	
	
	command_t command = [](void* data)
	{
		create_scene_proxy_t* args = (create_scene_proxy_t*)data;
	//	args->actor->scene_proxy = RendererCreateSceneProxy();
	};
	
	if(create_scene_proxy_t* args = 
	   (create_scene_proxy_t*)RenderMailBoxRequestArgsMemory(g_engine_reserver, sizeof(create_scene_proxy_t)))
	{		
		args->actor = actor;
		
		render_command_t render_command;
		render_command.command = command;
		render_command.user_data = args;
		
		RenderMailBoxCommitCommand(g_engine_reserver, &render_command);
	}
	
}
