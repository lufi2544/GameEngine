
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{	
	switch (msg)
    {
        case WM_DESTROY:		
		case WM_QUIT:
		{
			PostQuitMessage(0);
			g_engine->is_running = false;
			return 0;
		}
		
		case WM_CLOSE:
		{		
			DestroyWindow(g_engine->main_window.handle); // triggers WM_DESTROY
			return 0;
		};
	}
	
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


window CreateAWindow(u32 x, u32 y, u32 h, u32 w)
{
	window result;
	
	// Window class registration.
	HINSTANCE Instance = GetModuleHandle(0);
	char CLASS_NAME[] = "engine window";
	
	
	WNDCLASS WindowClass = {};
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = CLASS_NAME;
	
	RegisterClass(&WindowClass);
	
	
	// Window creation
	
	const char WINDOW_NAME[] = "engine";	
	HWND window_handle = CreateWindowEx(
										0,
										(LPCSTR)CLASS_NAME,
										(LPCSTR)WINDOW_NAME,
										WS_OVERLAPPEDWINDOW,
										x, y, 
										w, h,
										0,
										0, 
										Instance,
										0
										);	
	
	
	result.handle = window_handle;	
	if(window_handle == 0)
	{
		MAYORANA_LOG("The window handle is null");
		return result;
	}
	
	
	STARTUPINFOW startup_info = {};
	startup_info.cb = sizeof(startup_info);
	
	GetStartupInfoW(&startup_info);
	
	int nCmdShow = (startup_info.dwFlags & STARTF_USESHOWWINDOW)
		? startup_info.wShowWindow
		: SW_SHOWDEFAULT;
	
	ShowWindow(window_handle, nCmdShow);
	UpdateWindow(window_handle);
	
	return result;
}