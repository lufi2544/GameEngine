
global mesh_t* g_mesh;
global actor_t *g_actor;

global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{	
	// TODO Pipeline for actors
	g_actor = (actor_t*)push_size(&engine_data->memory->permanent, sizeof(actor_t));
	g_actor->transform.position = {0, 0, 5};
	g_actor->transform.rotation = {0, 0, 0};
	g_actor->transform.scale = {1, 1, 1};
	
				
	// we simulate the creation of an actor.
	scene_proxy_t *scene_proxy = 0;
	mesh_t* mesh = CreateMeshFromFile(&g_engine->shared_data, {0, 0, 5}, {0, 0, 0}, "data/crab.obj", "data/crab.png", &scene_proxy);
	//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);
	
	
	g_actor->mesh = mesh;
	
	g_mesh = mesh;
}

global_f void
ApplicationUpdate(engine_shared_data_t *engine_data, f32 dt)
{
	// INPUT for the camera
	
	g_actor->transform.rotation.y += 1 * dt;
	
	transform_t t = GetActorTransform(g_actor);	
	t.rotation.y += 1 *dt;
	
	ActorSetTransform(g_actor, &t);
	
	if(IsKeyPressed(InputKey_Up))
	{
		
	}
	
}

global_f void
ApplicationShutDown(engine_shared_data_t *engine_data)
{
	
}