


callback_f void
mesh_component_t_OnAttached(actor_t *actor, void* component)
{
	printf("Actor attached!! \n");
	
	mesh_component_t *mesh_component = (mesh_component_t*)component;
	arena_t *geometry_memory = EngineRequestMemory(enum_memory_sandbox_geometry);
	
	transform_t transform = actor->transform;
	mesh_importer_data_t mesh = CreateMeshFromFile(&g_engine->shared_data, geometry_memory, transform.position, transform.rotation, *mesh_component->file_name, *mesh_component->texture_name,  &ActorOnProxySet);
	mesh_component->vertex_num = mesh.vertex_num;
	mesh_component->face_num = mesh.face_num;
	mesh_component->uv_coords_num = mesh.uv_coords_num;
			
	RendererComputeImportedMesh(&mesh, mesh_component, &transform, *mesh_component->texture_name, &ActorOnProxySet);
	
}