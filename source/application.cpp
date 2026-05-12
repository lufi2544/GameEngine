
global mesh_t* g_mesh;
global actor_t *g_actor;

global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{	
	// TODO Pipeline for actors
	g_actor = (actor_t*)push_size(&engine_data->memory->permanent, sizeof(actor_t));	
				
	// we simulate the creation of an actor.
	//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);
	
	transform_t t = TransformIdentity();
	*g_actor = ActorCreate(engine_data, &engine_data->memory->permanent, &t, "data/crab.obj", "data/crab.png");
		
	
//	g_actor = ActorCreate();
	
	//g_mesh = mesh;
}

global_f void
ApplicationUpdate(engine_shared_data_t *engine_data, f32 dt)
{
	// INPUT for the camera	
	transform_t t = GetActorTransform(g_actor);	
	//t.rotation.y += 1 *dt;
	
	//ActorSetTransform(g_actor, &t);
	
	//ActorSetTransform(g_actor, &g_actor->transform);
	
	if(IsKeyPressed(InputKey_Up))
	{
		
	}
	
}

global_f void
ApplicationShutDown(engine_shared_data_t *engine_data)
{
	
}