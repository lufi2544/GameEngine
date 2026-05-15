/* date = May 14th 2026 6:28 pm */



///// For now the approach is to follow a simple actor component based where the actors have a reference 
// to the memory of the components.


struct component_memory_t
{
	void* component;
	u8 type;
};

typedef void (*component_attached_callback_t)(actor_t*, void*);


global_f component_memory_t 
CreateComponent(arena_t *_arena, u8 _type, u32 _size)
{
	component_memory_t result;
	result.type = _type;
	
	result.component = push_size(_arena, _size);
	
	return result;
}

global_f void* 
GetComponent(component_memory_t *_component, u8 _type)
{
	if(_type != _component->type)
	{
		return 0;
	}
	
	return _component->component;
}



global_f void 
AttachComponent(actor_t *actor, component_memory_t to_attach, component_attached_callback_t callback);

internal_f void
CreateComponent(arena_t *arena, u32 size);


#define GET_COMPONENT(component_mem, enum_type, type) \
(type*)GetComponent(component_mem, enum_type);

#define CREATE_COMPONENT(actor, type) \
(type*)push_size(arena, sizeof(type)) \
  

#define ATTACH_COMPONENT(actor, compo_type, compo, compo_enum_type)\
ActorAttachComponent(actor, &compo, sizeof(compo_type), compo_enum_type, &GLUE(compo_type, _OnAttached))
