



global_f void
ActorManagerInit(actor_manager_t *manager, arena_t *memory)
{
	actor_manager_t result;
	
	manager->current_actors = 0;
	manager->max_actors = MAX_ACTORS;
	manager->actors = (actor_t*)push_size(memory, sizeof(actor_t) * MAX_ACTORS);
	
	manager->current_components = 0;
	manager->max_components = MAX_COMPONENTS_PER_ACTOR * MAX_ACTORS;
	manager->components = (component_memory_t*)push_size(memory, sizeof(component_memory_t) * MAX_COMPONENTS_PER_ACTOR * MAX_ACTORS);	
	manager->components_memory = push_arena(memory, MAX_COMPONENT_SIZE* MAX_COMPONENTS_PER_ACTOR * MAX_ACTORS);	
}


global_f component_memory_t*
ActorManagerRequestComponentMemory(actor_manager_t *manager, u32 max)
{
	if(manager->current_components + max >= manager->max_components)
	{
		printf("Not enough space for more components\n");
		assert(false);
	}
	
	return manager->components + manager->current_components++;
}