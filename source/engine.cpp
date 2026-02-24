

global_f void 
EngineInit(engine_t *engine)
{	
	// TODO WRAP THIS
	// RENDER THREAD STUFF
	engine->frame_pipeline = (frame_pipeline_t*)push_size(&g_memory.permanent, sizeof(frame_pipeline_t));
	engine->frame_pipeline->game_index = 0;
	engine->frame_pipeline->render_index = 0;	
	engine->frame_pipeline->engine_event = PlatformEventCreate(&g_memory.permanent);
	engine->frame_pipeline->render_event = PlatformEventCreate(&g_memory.permanent);
	
	g_render_mailbox = (render_mailbox_t*)push_size(&g_memory.permanent, sizeof(render_mailbox_t));
	g_render_mailbox->command_max = MAX_RENDER_COMMANDS_PER_FRAME;
	g_render_mailbox->command_current[0] = 0;	
	g_render_mailbox->command_current[1] = 0;	
	g_render_mailbox->free_buffer_flags[0] = 1;
	g_render_mailbox->free_buffer_flags[1] = 1;
	g_render_mailbox->command_buffer[0] = (render_command_t*)push_size(&g_memory.permanent, sizeof(render_command_t) * MAX_RENDER_COMMANDS_PER_FRAME);
	g_render_mailbox->command_buffer[1] = (render_command_t*)push_size(&g_memory.permanent, sizeof(render_command_t) * MAX_RENDER_COMMANDS_PER_FRAME);
	
	g_render_mailbox->buffer_memory[0] = push_arena(&g_memory.permanent, MAX_RENDER_COMMANDS_PER_FRAME * RENDER_COMMAND_ARGS_MAX_SIZE);
	g_render_mailbox->buffer_memory[1] = push_arena(&g_memory.permanent, MAX_RENDER_COMMANDS_PER_FRAME * RENDER_COMMAND_ARGS_MAX_SIZE);
	
	
	g_engine_reserver = (reserver_t*)push_size(&g_memory.permanent, sizeof(reserver_t));

	
	engine->main_window = CreateAWindow(200, 200, 1080, 1920);
	engine->shared_data.memory = &g_memory;
    g_engine_camera  = (camera_t*)push_size(&g_memory.permanent, sizeof(camera_t));
	
	
	//// RENDERER
	////////
	
	arena_t render_arena = push_arena(&g_memory.permanent, Megabyte(100));
	if(RendererInit(render_arena))
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
	engine->shared_data.meshes = (mesh_t*)push_size(&g_memory.permanent, MAX_MESH_COUNT * sizeof(mesh_t));
	ApplicationInit(&engine->shared_data);
}

global_f void
EngineUpdate(engine_t *engine, f32 dt)
{
	ApplicationUpdate(&engine->shared_data, (f32)(dt / 1000.0f) );
}

global_f void 
EngineRender(engine_t *engine)
{
	RendererUpdate(&engine->shared_data);
}


global_f void 
EngineShutDown(engine_t *engine)
{
	
}

global_f void
EngineInput()
{
	HandleInput();
}

volatile u32 engine_frame = 0;
volatile u32 render_frame = 0;

global_f void 
RenderThreadLoop(void* data)
{
	
	arena_t* renderer_memory = RendererGetMemory();
	g_scene = (scene_t*)push_size(renderer_memory, sizeof(frame_pipeline_t));
	g_scene->scene_proxies = (scene_proxy_t*)push_size(renderer_memory, sizeof(scene_proxy_t) * MAX_MESH_COUNT);
	g_scene->current_scene_proxies = 0;
	g_scene->max_scene_proxies = MAX_MESH_COUNT;
	
	
	// The renderer will read from the render_mailbox 	
	while(g_engine->is_running)
	{				
		RenderMailBoxProcessCommands();
		
		//PlatformEventWait(g_engine->frame_pipeline->render_event);		
		MemoryBarrier();				
		
		frame_data_t *read = &g_engine->frame_pipeline->buffers[g_engine->frame_pipeline->render_index];
		printf("RenderFrame %i \n", read->frame_number);
		
		EngineRender(g_engine);
		
		++render_frame;
		
		g_engine->frame_pipeline->render_index ^= 1;
		
		
		PlatformEventSignal(g_engine->frame_pipeline->engine_event);		
	}
}

global_f void
SyncWithRenderThread(frame_pipeline_t *pipeline)
{	
	MemoryBarrier();	
	pipeline->game_index ^= 1;
	
	PlatformEventSignal(pipeline->render_event);
}

global_f void
CreateRenderThread(mythread_t *render_thread, engine_t *engine)
{
	SCRATCH();
	
	string_t render_thread_name = STRING_V(temp_arena, "render_thread");	
	start_thread(render_thread, &engine->shared_data.memory->threads, render_thread_name, RenderThreadLoop, 0);
}

global_f u32
EngineRunLoop(engine_t *engine, f32 dt, f32 target_fps, f32 target_ms_frame)
{					
	RenderMailBoxAsignReserver(g_engine_reserver);
	
	frame_data_t* write = &engine->frame_pipeline->buffers[engine->frame_pipeline->game_index];
	write->frame_number = ++engine_frame;
	
	printf("Engine frame %i \n", write->frame_number);
	
	f64 frame_begin = PlatformNow();		
	PlatformUpdate();
	
	EngineInput();
	EngineUpdate(engine, dt);
	
	f64 frame_end = PlatformNow();
	
	f64 frame_time = frame_end - frame_begin;
	if(frame_time < target_ms_frame)
	{
		PlatformSleep(target_ms_frame - frame_time);
		frame_end = PlatformNow();
	}
	
	dt = frame_end - frame_begin;
	
	printf("Engine Thread: Free %i \n", g_engine_reserver->id);
	
	RenderMailBoxReturnReserver(g_engine_reserver);
	SyncWithRenderThread(engine->frame_pipeline);		
	
	return 0;
}


global_f u32 
EngineRun(engine_t *engine)
{
	SCRATCH();
	
	EngineInit(engine);		
		
	mythread_t render_thread;
	CreateRenderThread(&render_thread, engine);
	
	
	f32 dt = 33.33f;
	f32 target_fps = MAX_FPS;
	f32 target_ms_frame = (f32)(1 / target_fps) * 1000.0f;
	
	// first time loop run to activate the engine thread without the need to wait for the engine platform event.
	EngineRunLoop(engine, dt, target_fps, target_ms_frame);
	
	while(engine->is_running)
	{
		EngineRunLoop(engine, dt, target_fps, target_ms_frame);
	}
				
	EngineShutDown(engine);	
	
	// TODO scope the render thread somewhere else.
	end_thread(&render_thread, true);
	
	return 0;
}