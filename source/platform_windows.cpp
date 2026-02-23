/* date = January 22nd 2026 7:53 pm */

/////////////////////
//// WINDOWS PLATFORM UPDATE

/** For windows this is the messate handling */
global_f void
PlatformUpdate()
{
	MSG msg;
	/* In here we have PeekMessage, since GetMessage blocks the thread until a message is received, while PeekMessage will return non-zero
until we have no more messages in the message queue.
 */
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{						
		// related to keyboard intput.
		TranslateMessage(&msg);		
				
		switch(msg.message)
		{				
			case WM_QUIT:
			{
				g_engine->is_running = false;
				return;
			};
			
			case WM_KEYDOWN:
			{
				WPARAM wParam = msg.wParam;
				ProcessWindowsInputKey(wParam, true);
				break;
			}
			
			case WM_KEYUP:
			{
				WPARAM wParam = msg.wParam;
				ProcessWindowsInputKey(wParam, false);
				break;
			}
			default:;
		}
		
		// Calling the window proc in the window this message was pulled from.
		DispatchMessage(&msg);	
	}	
}


global_f f64
PlatformNow()
{
	f32 result;
	DWORD time_since_start = GetTickCount();
	result = time_since_start;
	return result;
}


global_f void
PlatformSleep(f64 sleep_for_ms)
{
	Sleep(sleep_for_ms);
}



struct platform_event_t
{
	HANDLE handle;
};

global_f platform_event_t*
PlatformEventCreate(arena_t *arena)
{
	platform_event_t *result = (platform_event_t*)push_size(arena, sizeof(platform_event_t));
	result->handle = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	return result;
}

global_f void 
PlatformEventWait(platform_event_t *event)
{
	WaitForSingleObject(event->handle, INFINITE);
}


global_f void
PlatformEventSignal(platform_event_t *event)
{
	SetEvent(event->handle);
}