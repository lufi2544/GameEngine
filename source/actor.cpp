

// TODO pass the actor arena and just add a new actor there.
global_f actor_t
ActorCreate(actor_manager_t *manager, engine_shared_data_t *shared_data, transform_t *transform, const char  *file_name, const char *texture_name, u32 components_max)
{	
	arena_t *geometry_memory = EngineRequestMemory(enum_memory_sandbox_geometry);
	
	actor_t result;
	mesh_t mesh = CreateMeshFromFile(manager, shared_data, geometry_memory, transform->position, transform->rotation, file_name, texture_name,  &ActorOnProxySet);
	
	
	
	component_memory_t* first_component = ActorManagerRequestComponentMemory(manager, components_max);
	result.components = first_component;
	
	mesh_component_t* component = CREATE_COMPONENT(&manager->components_memory, mesh_component_t);
	component->mesh = mesh;
	
	component_memory_t to_attach;
	to_attach.component = component;
	to_attach.size = sizeof(mesh_component_t);
	
	ActorAttachComponent(&result, to_attach);
	
	RendererComputeImportedMesh(&component->mesh, transform, texture_name, &ActorOnProxySet);	
	
	result.transform = *transform;
	result.components_max = components_max;
	result.components_num = 0;
		
	return result;
}

global_f void
ActorSetTransform(actor_t *actor, transform_t *transform)
{
	actor->transform = *transform;
	
	UpdateProxyTransform(actor->mesh->scene_proxy, transform);
}


callback_f void
ActorOnProxySet(void* mesh)
{
	
}


global_f void
ActorAttachComponent(actor_t *actor, component_memory_t component_memory)
{
	if(actor->components_num == actor->components_max)
	{
		// TODO Actor ID
		printf("Actor exceeeded the max actor components \n");
		return;
	}
	
	actor->components[actor->components_num++] = component_memory;
}