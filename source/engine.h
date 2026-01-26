/* date = January 22nd 2026 6:30 pm */

#include "mesh.h"
#include "engine_shared_data.h"
#include "file_reader.h"

////////// Main Engine API //////////

struct engine_t
{		
	window_t main_window;
	bool is_running;
	engine_shared_data_t shared_data;
};
///////////////////////////
