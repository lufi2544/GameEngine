/* date = January 22nd 2026 6:30 pm */



////////// Main Engine API //////////

struct engine
{		
	window main_window;
	bool is_running;
};


void 
EngineInit();

u32 
EngineRun();

void
EngineUpdate();

void 
EngineRender();

void 
EngineShutDown();


///////////////////////////

