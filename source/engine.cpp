
global_f void 
EngineInit()
{	
	g_engine->main_window = CreateAWindow(200, 200, 1080, 1920);
	g_engine->shared_data.memory = &g_memory;
    g_engine_camera  = (camera_t*)push_size(&g_memory.permanent, sizeof(camera_t));
	
	if(RendererInit())
	{
		MAYORANA_LOG("Renderer Init successfully");
	}
	else
	{
		MAYORANA_LOG("Windows: Renderer NOT INITALIZED! ");		
	}
	
	
	// TODO Maybe application layer and move this to the engine shared data layer
	g_engine_camera->position = {0, 0, -1};
	g_engine_camera->target = {0, 0, 0};
	g_engine_camera->up = {0, 1, 0};
	g_engine_camera->fov = 60.0f * PI / 180.0f;
	
	g_engine_camera->near_z = 0.1f;
	g_engine_camera->far_z = 100.0f;		
	
	// Mesh array init
	g_engine->shared_data.meshes = (mesh_t*)push_size(&g_memory.permanent, MAX_MESH_COUNT * sizeof(mesh_t));
	ApplicationInit(&g_engine->shared_data);
}



global_f void
EngineUpdate(f32 dt)
{
	ApplicationUpdate(&g_engine->shared_data, (f32)(dt / 1000.0f) );
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

global_f void
EngineInput()
{
	HandleInput();
}


global_f u32 
EngineRun()
{
	EngineInit();
	
	
	f32 dt = 33.33f;
	f32 target_fps = MAX_FPS;
	f32 target_ms_frame = (f32)(1/target_fps) * 1000.0f;
	while(g_engine->is_running)
	{		
		f64 frame_begin = PlatformNow();
		
		PlatformUpdate();
		
		EngineInput();
		EngineUpdate(dt);
		EngineRender();
		f64 frame_end = PlatformNow();
		
		f64 frame_time = frame_end - frame_begin;
		
		if(frame_time < target_ms_frame)
		{
			PlatformSleep(target_ms_frame - frame_time);
			frame_end = PlatformNow();
		}
		
		dt = frame_end - frame_begin;
	}
	
	EngineShutDown();
	
	return 0;
}