/* date = May 14th 2026 7:02 pm */


struct actor_manager_t
{
	arena_t memory;
	
	actor_t *actors;
	u32 current_actors;
	u32 max_actors;
	
	component_memory_t* components;	
	u32 current_components;
	u32 max_components;
	
	arena_t components_memory; // memory to create the raw components
};


global_f void
ActorManagerInit(actor_manager_t *manager, arena_t *memory);


global_f component_memory_t* 
ActorManagerRequestComponentMemory(actor_manager_t *manager, u32 max);


