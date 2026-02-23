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
	s32 reserved;
};

global_f void*
RenderMailBoxRequestArgsMemory(reserver_t *reserver, u32 size)
{
	if(reserver->reserved == -1)
	{
		printf("RenderMailBoxRequestArgsMemory Reserver does not own a buffer. \n");
		return 0;
	}
	
	arena_t *buffer_arena = &g_render_mailbox->buffer_memory[reserver->reserved];
	return push_size(buffer_arena, size);	
}

global_f void
RenderMailBoxCommitCommand(reserver_t *reserver, render_command_t *command)
{
	u32 free_idx = reserver->reserved;
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
	
	bool b_any_ready =  b_has_data && b_is_free;
	
	return !b_any_ready;
}

global_f bool 
RenderMailBoxAsignReserver(reserver_t *reserver)
{
	s32 free_idx = -1;
	
	for(u8 idx = 0; idx < 2; ++idx)
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
	
	reserver->reserved = free_idx;
	return true;
}



global_f void
RenderMailBoxReturnReserver(reserver_t *reserver)
{
	if(reserver->reserved == -1)
	{
		return;
	}
	
	render_mailbox_t *mailbox =  g_render_mailbox;
	u32 idx_to_return = reserver->reserved;	
	mailbox->free_buffer_flags[idx_to_return] = 1;
	
	PlatformMemoryBarrier();
	reserver->reserved = -1;
}

global_f bool
RenderMailBoxReadyToBeProcessed(reserver_t *reserver)
{
	
	s32 ready_idx = -1;	
	bool b_any_ready = false;
	for(u8 idx = 0; idx < 2; ++idx)
	{
		render_mailbox_t* mailbox = g_render_mailbox;
		bool b_has_data = mailbox->command_current[idx] > 0;
		bool b_is_free = mailbox->free_buffer_flags[idx] == true;
		
		b_any_ready =  b_has_data && b_is_free;
	}
	
	render_mailbox_t *mailbox =  g_render_mailbox;
	
	
}

global_f void
RenderMailBoxFreeReserver(reserver_t *reserver)
{
	if(reserver->reserved == -1)
	{
		return;
	}
	
	render_mailbox_t *mailbox =  g_render_mailbox;
	u32 idx_to_free = reserver->reserved;
	
	mailbox->command_current[idx_to_free] = 0;	
	mailbox->free_buffer_flags[idx_to_free] = 1;
	reset_arena(&mailbox->buffer_memory[idx_to_free]);
	
	PlatformMemoryBarrier();
	reserver->reserved = -1;
}


