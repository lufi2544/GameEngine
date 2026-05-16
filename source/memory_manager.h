/* date = May 14th 2026 7:20 pm */


/* In charge of dividing the engine memory through the different sandboxes for the
 * for the different engine modules.
 */

enum enum_memory_sandbox
{
	enum_memory_sandbox_renderer,
	enum_memory_sandbox_string_table,	
	enum_memory_sandbox_geometry,
	enum_memory_sandbox_actors,
	enum_memory_sandbox_ai,
	
	enum_memory_sandbox_num
};

struct memory_sandbox_t
{
	enum_memory_sandbox type;
	arena_t memory;
};


global_f memory_sandbox_t g_sandboxes[enum_memory_sandbox_num] = 
{
	{ enum_memory_sandbox_renderer, {} },
	{ enum_memory_sandbox_geometry, {}},
	{ enum_memory_sandbox_actors, {}},
	{ enum_memory_sandbox_ai, {}},
};


global_f void
EngineMemoryInit(arena_t *memory)
{
	u8 a = (enum_memory_sandbox)enum_memory_sandbox_num;
	for(u8 idx = 0; idx < enum_memory_sandbox_num; ++idx)
	{
		enum_memory_sandbox type = (enum_memory_sandbox)idx;
		
		switch(type)
		{
			case enum_memory_sandbox_renderer:
			{
				g_sandboxes[idx].memory = push_arena(memory, Megabyte(100));
				
			}break;
			
			case enum_memory_sandbox_geometry:
			{				
				g_sandboxes[idx].memory = push_arena(memory, Megabyte(100));
				
			}break;
			case enum_memory_sandbox_actors:
			{
				g_sandboxes[idx].memory = push_arena(memory, Megabyte(100));
			}break;
			case enum_memory_sandbox_ai:
			{
				g_sandboxes[idx].memory = push_arena(memory, Megabyte(100));
			}break;
			
			case enum_memory_sandbox_string_table:
			{
				g_sandboxes[idx].memory = push_arena(memory, Megabyte(100));
			}break;			
			
			default:break;
		};
	}
};


global_f arena_t* 
EngineRequestMemory(enum_memory_sandbox type)
{		
	if(type == enum_memory_sandbox_num)
	{
		return 0;
	}
	
	return &g_sandboxes[type].memory;
}

