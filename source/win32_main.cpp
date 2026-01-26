
/** This is the main entry point to the engine on Windows.  */

#include "includes.h"


void InitGlobals()
{
	// Init globals
	g_engine = (engine_t*)push_size(&g_memory.permanent, sizeof(engine_t));
	g_engine->is_running = true;
}

void InitFramework()
{
	mayorana_init();
	
	InitGlobals();
}

int main(int arg_num, char** args)
{
	InitFramework();
	u32 run_error = EngineRun();
	
	return run_error;
}

