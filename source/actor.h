/* date = February 22nd 2026 8:15 pm */



struct scene_proxy_t;

struct actor_t
{	
	mesh_t* mesh;
	
	transform_t transform;	
};



transform_t GetActorTransform(actor_t *actor)
{
	return actor->transform;
}


// TODO: Think about decoupling the texture maybe?
global_f actor_t
ActorCreate(engine_shared_data_t *shared_data, transform_t *transform, const char* file_name, const char* texture_name);

global_f void
ActorSetTransform(actor_t *actor, transform_t *transform);

callback_f void
ActorOnProxySet(void* actor);