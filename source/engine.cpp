

global_f void 
EngineInit()
{	
	g_engine->main_window = CreateAWindow(200, 200, 800, 600);	
	g_engine->shared_data.memory = &g_memory;
	
	if(RendererInit())
	{
		MAYORANA_LOG("Renderer Init successfully");
	}
	else
	{
		MAYORANA_LOG("Windows: Renderer NOT INITALIZED! ");		
	}
	
	
	// TODO Maybe application layer and move this to the engine shared data layer
	g_engine_camera.position = {0, 0, -5};
	g_engine_camera.target = {0, 0, 0};
	g_engine_camera.up = {0, 1, 0};
	g_engine_camera.fov = 60.0f * PI / 180.0f;
	
	g_engine_camera.near_z = 0.1f;
	g_engine_camera.far_z = 100.0f;		
	
	// Mesh array init
	g_engine->shared_data.meshes = (mesh_t*)push_size(&g_memory.permanent, MAX_MESH_COUNT * sizeof(mesh_t));
	ApplicationInit(&g_engine->shared_data);
}



global_f void
EngineUpdate()
{
	
}

global_f void 
EngineRender()
{
	RendererUpdate(&g_engine->shared_data);
}


global_f void 
EngineShutDown()
{
	
}


global_f u32 
EngineRun()
{
	EngineInit();
	while(g_engine->is_running)
	{	
		PlatformUpdate();
		
		EngineUpdate();
		EngineRender();
	}
	
	EngineShutDown();
	
	return 0;
}