
global mesh_t* g_mesh;

global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{
	mesh_t* plane = CreateMeshFromFile(&g_engine->shared_data, {0, 0, 5}, {0, 0, 0}, "data/crab.obj", "data/crab.png");		
//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);

	
	g_mesh = plane;
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