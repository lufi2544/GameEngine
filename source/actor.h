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

global_f void
ActorSetTransform(actor_t *actor, transform_t *transform)
{
	actor->transform = *transform;	
	UpdateProxyTransform(actor->mesh->scene_proxy, transform); 
}