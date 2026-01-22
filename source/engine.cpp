
void 
EngineInit()
{
	g_engine->main_window = CreateAWindow(200, 200, 800, 600);
}

u32 
EngineRun()
{
	EngineInit();
	while(PlatformUpdate() && g_engine->is_running)
	{		
		EngineRender();
		EngineUpdate();
	}
	
	EngineShutDown();
	
	return 0;
}

void
EngineUpdate()
{
	
}

void 
EngineRender()
{
	
}


void 
EngineShutDown()
{
	
}
