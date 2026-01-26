/* date = January 22nd 2026 6:30 pm */

#include "engine_shared_data.h"
#include "file_reader.h"

////////// Main Engine API //////////

struct engine
{		
	window main_window;
	bool is_running;
	engine_shared_data shared_data;
};
///////////////////////////
