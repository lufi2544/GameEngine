/* date = February 22nd 2026 8:15 pm */



struct scene_proxy_t;
struct actor_manager_t;

// TODO review alingment
struct actor_t
{	
	
	mesh_t* mesh;	// TODO REMOVE
	u32 id;
	transform_t transform;	
	
	component_memory_t *components;
	u32 components_num;
	u32 components_max;
	arena_t components_memory; // used to allocate any components related memory has 		
	
	// TODO As we remove components from the actor, we will need to add logic for making space for components, maybe deactivating/activating them? or just a set of components and then we just activate/deactivate as needed. 
	
};

transform_t GetActorTransform(actor_t *actor)
{
	return actor->transform;
}


// TODO: Think about decoupling the texture maybe?
global_f actor_t
ActorCreate(actor_manager_t *manager, engine_shared_data_t *shared_data, transform_t *transform, const char* file_name, const char* texture_name, u32 components_max);

global_f void
ActorSetTransform(actor_t *actor, transform_t *transform);

callback_f void
ActorOnProxySet(void* actor);

internal_f void
ActorAttachComponent(actor_t *actor, void* to_add_component, u32 size, u8 type, component_attached_callback_t callback);


