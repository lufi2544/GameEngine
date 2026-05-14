/* date = May 14th 2026 6:28 pm */



///// For now the approach is to follow a simple actor component based where the actors have a reference 
// to the memory of the components.


struct component_memory_t
{
	void* component;
	u32 size;	
	u8 type;
};


global_f component_memory_t 
CreateComponent(arena_t *_arena, u8 _type, u32 _size)
{
	component_memory_t result;
	result.type = _type;
	result.size = _size;
	
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


#define GET_COMPONENT(component_mem, enum_type, type) \
(type*)GetComponent(component_mem, enum_type);

#define CREATE_COMPONENT(arena, type) \
(type*)push_size(arena, sizeof(type))
