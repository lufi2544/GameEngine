/* date = February 22nd 2026 8:29 pm */


/////////// Command used to process a render scene proxy.
struct render_command_t
{
	
};


struct render_mailbox_t
{
	// Double buffer
	render_command_t *command_buffers[2];
};



