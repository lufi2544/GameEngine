
global_f void
ActorManagerInit(actor_manager_t *manager)
{
	actor_manager_t result;
	
	arena_t* actors_memory = EngineRequestMemory(enum_memory_sandbox_actors);	
	manager->current_actors = 0;
	manager->max_actors = MAX_ACTORS;
	manager->actors = (actor_t*)push_size(actors_memory, sizeof(actor_t) * MAX_ACTORS);
	
	manager->free_actors = (u32*)push_size(actors_memory, sizeof(u32) * MAX_ACTORS);
	manager->current_free_actors = manager->max_actors;
	
	for(u32 idx = 0; 
		idx < manager->max_actors;
		++idx)
	{
		manager->free_actors[idx] = idx;
	}
	
	manager->current_components = 0;
	manager->max_components = MAX_COMPONENTS_PER_ACTOR * MAX_ACTORS;
	manager->components = (component_memory_t*)push_size(actors_memory, sizeof(component_memory_t) * MAX_COMPONENTS_PER_ACTOR * MAX_ACTORS);		
	manager->components_memory = push_arena(actors_memory, MAX_ACTORS * MAX_COMPONENTS_PER_ACTOR * MAX_COMPONENT_SIZE);
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

global_f actor_t*
ActorManagerCreateActor(actor_manager_t *manager, transform_t *transform, u32 components_max)
{
	if(manager->current_free_actors == 0)
	{
		return 0;
	}
	
	if(components_max > MAX_COMPONENTS_PER_ACTOR)
	{
		printf(" Requesting actor with more components than MAX_COMPONENTS_PER_ACTOR \n");
		assert(false);
	}
	
	u32 actor_id = manager->current_free_actors--;
	actor_t *free_actor =  &manager->actors[actor_id];
	free_actor->id = actor_id;
	free_actor->components_num = 0;
	free_actor->components_max = components_max;
	free_actor->transform = *transform;
	
	free_actor->components = ActorManagerRequestComponentMemory(manager, components_max);
	
	arena_t *actors_memory = EngineRequestMemory(enum_memory_sandbox_actors);		
	free_actor->components_memory = push_arena(&manager->components_memory, MAX_COMPONENT_SIZE * components_max);
	
	return free_actor;
}