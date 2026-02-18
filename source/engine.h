/* date = January 22nd 2026 6:30 pm */

#include "engine_shared_data.h"

#include "core.h"
#include "camera.h"

#include "mesh.h"
#include "asset_manager.h"

////////// Main Engine API //////////

struct engine_t
{		
	window_t main_window;
	bool is_running;
	engine_shared_data_t shared_data;
};
///////////////////////////
