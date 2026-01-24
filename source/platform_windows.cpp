/* date = January 22nd 2026 7:53 pm */


/////////// Unique Windows platform handling

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
			default:;
		}
		
		// Calling the window proc in the window this message was pulled from.
		DispatchMessage(&msg);	
	}	
}