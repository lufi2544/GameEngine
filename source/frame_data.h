/* date = February 22nd 2026 9:10 pm */

struct frame_data_t
{
	u32 frame_number;
	
};


struct platform_event_t;

struct frame_pipeline_t
{
	frame_data_t buffers[2];	
	
	u32 game_index;
	u32 render_index;
	
	platform_event_t *platform_event;
};

