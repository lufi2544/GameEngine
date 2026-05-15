
global actor_t *g_actor;
global actor_t *g_actor_1;

global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{
	
	// yaw - 45, p - 35
	f32 yaw   = 45.0f  * PI / 180.0f;  // giro horizontal   
	f32 pitch = 25.0f * PI / 180.0f;  // elevacion (35.26 =
	f32 roll  = 0.0f   * PI / 180.0f;  // inclinacion latera
	f32 dist  = 50.0f;        
	
	vec3_t target = Vec3Identity();
	vec3_t eye = Vec3Identity();
	
	// Posicion de la camara en coordenadas esfericas       
	vec3_t iso_dir;                                         
	iso_dir.x = cosf(pitch) * cosf(yaw);                    
	iso_dir.y = sinf(pitch);                                
	iso_dir.z = cosf(pitch) * sinf(yaw);                    
	
	Vec3MultiplyF32(&iso_dir, dist);                        
	Vec3Add(target, iso_dir, &eye);                         
	
	g_engine_camera->position = eye;
	g_engine_camera->target   = target;
	g_engine_camera->up       = {sinf(roll), cosf(roll), 0.0f};
	
	g_engine_camera->near_z = 0.1f;
	g_engine_camera->far_z = 100.0f;
	
	
	// TODO Pipeline for actors
	g_actor = (actor_t*)push_size(&engine_data->memory->permanent, sizeof(actor_t));	
	g_actor_1 = (actor_t*)push_size(&engine_data->memory->permanent, sizeof(actor_t));	
				
	// we simulate the creation of an actor.
	//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);
	
	transform_t t = TransformIdentity();
	t.position = {0, 0, 0};
	t.rotation = { 0, 0, 0};
	
	transform_t t_ = TransformIdentity();
	t_.position = {0, 0, 10};
	t_.rotation = { 0, 0, 0};
	
	g_actor = ActorManagerCreateActor(&g_engine->managers.actor_manager, &t_, 5);
	g_actor_1 = ActorManagerCreateActor(&g_engine->managers.actor_manager, &t, 5);
	
	
	SCRATCH_ARENA(&engine_data->memory->transient);
	
	mesh_component_t mesh_component;
	mesh_component.file_name = STRING_V(temp_arena, "data/drone.obj");
	mesh_component.texture_name = STRING_V(temp_arena, "data/drone.png");
		
	ATTACH_COMPONENT(g_actor, mesh_component_t, mesh_component, ComponentType_Mesh);
	
	
	mesh_component_t mesh_component_;
	mesh_component_.file_name = STRING_V(temp_arena, "data/efa.obj");
	mesh_component_.texture_name = STRING_V(temp_arena, "data/efa.png");
	
	ATTACH_COMPONENT(g_actor_1, mesh_component_t, mesh_component_, ComponentType_Mesh);		
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