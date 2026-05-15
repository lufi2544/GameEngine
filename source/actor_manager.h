/* date = May 14th 2026 7:02 pm */


struct actor_manager_t
{
	arena_t memory;
	
	actor_t *actors;
	u32 current_actors;
	u32 max_actors;
	
	u32* free_actors;
	u32 current_free_actors;
	
	
	component_memory_t* components;	
	u32 current_components;
	u32 max_components;
	
	arena_t components_memory; // main memory to pass to the actors when created so they can allocate components memory as needed.
};


global_f void
ActorManagerInit(actor_manager_t *manager, arena_t *memory);


global_f component_memory_t* 
ActorManagerRequestComponentMemory(actor_manager_t *manager, u32 max);


global_f actor_t*
ActorManagerCreateActor(actor_manager_t *manager, transform_t *transform, u32 components_max);