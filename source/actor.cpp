

// TODO pass the actor arena and just add a new actor there.
global_f actor_t
ActorCreate(engine_shared_data_t *shared_data, transform_t *transform, const char  *file_name, const char *texture_name)
{	
	
	actor_t result;
	mesh_t* mesh = CreateMeshFromFile(shared_data, transform->position, transform->rotation, file_name, texture_name,  &ActorOnProxySet);
	result.mesh = mesh;
	result.transform = *transform;
		
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