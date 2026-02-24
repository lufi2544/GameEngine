/* date = February 22nd 2026 8:29 pm */




// Engine Thread is the one that writes and the Render Thread is the one that read the buffers from the game thread.

/////////// Command used to process a render scene proxy.
typedef void (*command_t)(void*/*user data*/);

#define RENDER_COMMAND_ARGS_MAX_SIZE 128
struct render_command_t
{
	command_t command;
	void* user_data;
};

#define MAX_RENDER_COMMANDS_PER_FRAME 10
#define RENDER_COMMAND_BUFFER_NUM 2

// Game thread will write to this and the render thread is the consumer.
struct render_mailbox_t
{	
	render_command_t *command_buffer[RENDER_COMMAND_BUFFER_NUM];
	u32 command_current[RENDER_COMMAND_BUFFER_NUM];
	u32 command_max;
	bool free_buffer_flags[RENDER_COMMAND_BUFFER_NUM];
	arena_t buffer_memory[RENDER_COMMAND_BUFFER_NUM];
};

struct reserver_t
{
	s32 id;
};

global_f arena_t*
RenderMailBoxRequestMemoryArena(reserver_t *reserver)
{
	arena_t *buffer_arena = &g_render_mailbox->buffer_memory[reserver->id];	
	return buffer_arena;
}

global_f void*
RenderMailBoxRequestArgsMemory(reserver_t *reserver, u32 size)
{
	if(reserver->id == -1)
	{
		printf("RenderMailBoxRequestArgsMemory Reserver does not own a buffer. \n");
		return 0;
	}
	
	arena_t *buffer_arena = &g_render_mailbox->buffer_memory[reserver->id];
	return push_size(buffer_arena, size);	
}

global_f void
RenderMailBoxCommitCommand(reserver_t *reserver, render_command_t *command)
{
	u32 free_idx = reserver->id;
	if(free_idx < 0)
	{
		printf("Reserver not assigned... \n");
		return;
	}
	
	render_command_t *command_buffer = g_render_mailbox->command_buffer[free_idx];
	u32* command_current = &g_render_mailbox->command_current[free_idx];
	
	if(*command_current + 1 >= g_render_mailbox->command_max)
	{
		return;
	}
	
	command_buffer[(*command_current)++] = *command;	
}

internal_f bool 
RenderMailBoxIsBufferFree(u32 idx)
{
	render_mailbox_t* mailbox = g_render_mailbox;
	bool b_has_data = mailbox->command_current[idx] == 0;
	bool b_is_free = mailbox->free_buffer_flags[idx] == true;
	
	bool b_ready =  b_has_data && b_is_free;
	
	return b_ready;
}

global_f bool 
RenderMailBoxAsignReserver(reserver_t *reserver)
{
	s32 free_idx = -1;
	
	for(u8 idx = 0; idx < RENDER_COMMAND_BUFFER_NUM; ++idx)
	{
		
		bool b_free = RenderMailBoxIsBufferFree(idx);
		if(b_free)
		{
			free_idx = idx;
			break;
		}
	}
	
	if(free_idx < 0)
	{
		//TODO decide what should we do with full buffers
		printf("Render Mail Box full.\n");
		return false;
	}
	
	reserver->id = free_idx;
	return true;
}



global_f void
RenderMailBoxReturnReserver(reserver_t *reserver)
{
	if(reserver->id == -1)
	{
		return;
	}
	
	render_mailbox_t *mailbox =  g_render_mailbox;
	u32 idx_to_return = reserver->id;	
	mailbox->free_buffer_flags[idx_to_return] = 1;
	
	PlatformMemoryBarrier();
	reserver->id = -1;
}

global_f bool
RenderMailBoxGetReadyToProcessReserver(reserver_t *out_reserver)
{	
	s32 ready_idx = -1;	
	bool b_any_ready = false;
	render_mailbox_t* mailbox = g_render_mailbox;
	
	for(u8 idx = 0; idx < RENDER_COMMAND_BUFFER_NUM; ++idx)
	{
		bool b_has_data = mailbox->command_current[idx] > 0;
		bool b_is_free = mailbox->free_buffer_flags[idx] == true;
		
		b_any_ready =  b_has_data && b_is_free;
		if(b_any_ready)
		{
			ready_idx = idx;
			break;
		}
	}
	
	
	out_reserver->id = ready_idx;
	
	return ready_idx != -1;		
}

/* 
 * Called by the engine thread to check if it can continue the next frame, since the Render Thread can be still 
 * processing the last render frame.
 */
global_f bool
RenderMailBoxCheckIfBuffersFull(reserver_t *current)
{
	bool b_full = true;
	render_mailbox_t* mailbox = g_render_mailbox;
	
	for(u8 idx = 0; idx < RENDER_COMMAND_BUFFER_NUM; ++idx)
	{
		if(idx == current->id)
		{
			continue;
		}
		
		bool b_free = RenderMailBoxIsBufferFree(idx);
		if(b_free)
		{
			return false;	
		}
	}
	
	
	return true;
}

global_f void
RenderMailBoxFreeReserver(reserver_t *reserver)
{
	if(reserver->id == -1)
	{
		return;
	}
	
	render_mailbox_t *mailbox =  g_render_mailbox;
	u32 idx_to_free = reserver->id;
	
	mailbox->command_current[idx_to_free] = 0;
	mailbox->free_buffer_flags[idx_to_free] = 1;
	reset_arena(&mailbox->buffer_memory[idx_to_free]);
	
	PlatformMemoryBarrier();
	reserver->id = -1;
}

global_f void
RenderMailBoxProcessCommands()
{
	reserver_t reserver;
	if(!RenderMailBoxGetReadyToProcessReserver(&reserver))
	{
		return;
	}
	
	printf("Render Thread: Processing %i \n", reserver.id);
		
	u32 idx = reserver.id;
	render_mailbox_t *mailbox = g_render_mailbox;		
	render_command_t *command_buffer = mailbox->command_buffer[idx];
	u32 *commands_num = &mailbox->command_current[idx];
	
	for(u32 command_idx = 0; command_idx < *commands_num; ++command_idx)
	{
		render_command_t command = command_buffer[command_idx];
		command.command(command.user_data);
	}
	

	RenderMailBoxFreeReserver(&reserver);
}
