
global mesh_t* g_mesh;
global actor_t *g_actor;

global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{
	
	vec3_t iso_dir = Vec3Normalize({1.0f, 1.0f, 1.0f});
	vec3_t target = {0, 0, 0};
	
	f32 distance = 50.0f;
	
	vec3_t eye;
	Vec3MultiplyF32(&iso_dir, distance),
	Vec3Add(target, iso_dir, &eye);
	
	g_engine_camera->position = eye;
	g_engine_camera->target = target;
	g_engine_camera->up = {0, 1, 0};
	g_engine_camera->fov = 60.0f * PI / 180.0f;
	
	g_engine_camera->near_z = 0.1f;
	g_engine_camera->far_z = 100.0f;
	
	
	// TODO Pipeline for actors
	g_actor = (actor_t*)push_size(&engine_data->memory->permanent, sizeof(actor_t));	
				
	// we simulate the creation of an actor.
	//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);
	
	transform_t t = TransformIdentity();
	t.rotation = { 0, 0, 0};
	*g_actor = ActorCreate(engine_data, &engine_data->memory->permanent, &t, "data/drone.obj", "data/drone.png");
		
	
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