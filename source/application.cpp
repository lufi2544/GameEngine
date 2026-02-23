
global mesh_t* g_mesh;
global actor_t *g_actor;

global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{	
	g_actor = (actor_t*)push_size(&engine_data->memory->permanent, sizeof(actor_t));
				
	// we simulate the creation of an actor.
	mesh_t* mesh = CreateMeshFromFile(&g_engine->shared_data, {0, 0, 5}, {0, 0, 0}, "data/crab.obj", "data/crab.png");
	//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);
	
	scene_proxy_t *scene_proxy = CreateSceneProxy();
	
	g_actor->scene_proxy = scene_proxy;
	g_actor->mesh = mesh;
	
	g_mesh = mesh;
}

global_f void
ApplicationUpdate(engine_shared_data_t *engine_data, f32 dt)
{
	// INPUT for the camera
	
	g_mesh->transform.rotation.y += 1 * dt;
	
	if(IsKeyPressed(InputKey_Up))
	{
		
	}
	
}

global_f void
ApplicationShutDown(engine_shared_data_t *engine_data)
{
	
}