/** This is the main entry point to the engine on macOS. */

#import <Cocoa/Cocoa.h>

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
	// Ensure Objective-C runtime & autorelease pool exist
	@autoreleasepool
	{
		// Nothing else needed here
		// NSApplication is initialized lazily in platform_macos.mm
	}
    
	InitFramework();
	u32 run_error = EngineRun();
    
	return run_error;
}