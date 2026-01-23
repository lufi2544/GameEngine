
global_f void 
EngineInit()
{	
	g_engine->main_window = CreateAWindow(200, 200, 800, 600);
	
	if(RendererInit())
	{
		MAYORANA_LOG("Renderer Init successfully");
	}
	else
	{
		MAYORANA_LOG("Windows: Renderer NOT INITALIZED! ");		
	}
	
}



global_f void
EngineUpdate()
{
	
}

global_f void 
EngineRender()
{
	RendererUpdate();
}


global_f void 
EngineShutDown()
{
	
}


global_f u32 
EngineRun()
{
	EngineInit();
	while(g_engine->is_running)
	{	
		PlatformUpdate();
		
		EngineUpdate();
		EngineRender();
	}
	
	EngineShutDown();
	
	return 0;
}