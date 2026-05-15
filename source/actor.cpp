
/*
// TODO pass the actor arena and just add a new actor there.
global_f actor_t
ActorCreate(actor_manager_t *manager, engine_shared_data_t *shared_data, transform_t *transform, const char  *file_name, const char *texture_name, u32 components_max)
{	
	arena_t *geometry_memory = EngineRequestMemory(enum_memory_sandbox_geometry);
	
	actor_t result;
	mesh_t mesh = CreateMeshFromFile(shared_data, geometry_memory, transform->position, transform->rotation, file_name, texture_name,  &ActorOnProxySet);
		
	component_memory_t* first_component = ActorManagerRequestComponentMemory(manager, components_max);
	result.components = first_component;
	
	mesh_component_t component;
	component.mesh = mesh;	 

	
	
//	RendererComputeImportedMesh(&component->mesh, transform, texture_name, &ActorOnProxySet);
	
	result.transform = *transform;
	result.components_max = components_max;
	result.components_num = 0;
	
	return result;
}
*/
global_f void
ActorSetTransform(actor_t *actor, transform_t *transform)
{
	actor->transform = *transform;
	
	// GET COMPONENT SET SCENE PROXY
	//UpdateProxyTransform(actor->mesh->scene_proxy, transform);
}

callback_f void
ActorOnProxySet(void* mesh)
{
	
}

internal_f void
ActorAttachComponent(actor_t *actor, void* to_add_component, u32 size, u8 type, component_attached_callback_t callback)
{
	if(actor->components_num + 1 >= actor->components_max)
	{
		printf("Actor with id %i trying to attach component %i --> not enough space for more components.\n", actor->id, type);
		assert(false);
	}
	
	
	component_memory_t *component_memory = actor->components + actor->components_num++;
	
	void* component = push_size(&actor->components_memory, size);
	bytes_copy(component, to_add_component, size);
	
	component_memory->component = component;
	component_memory->type = type;	
	
	callback(actor, component_memory->component);
}

