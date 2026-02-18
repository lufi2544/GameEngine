
global_f void
ApplicationInit(engine_shared_data_t *engine_data)
{
	mesh_t* plane = CreateMeshFromFile(&g_engine->shared_data, {0, 0, 5}, {0, 0, 0}, "data/crab.obj", "data/crab.png");		
//	SetFlag(&plane->flags, RendererFlag_WireFrame, true);

	
	
}



global_f void
ApplicationUdpate(engine_shared_data_t *engine_data)
{
	
}



global_f void
ApplicationShutDown(engine_shared_data_t *engine_data)
{
	
}