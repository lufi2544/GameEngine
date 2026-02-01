#ifdef _WINDOWS
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{	
	switch (msg)
    {		
		case WM_CLOSE:
		{		
			DestroyWindow(hwnd); // triggers WM_DESTROY
			return 0;
		};
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			
		}break;
		
		case WM_PAINT:
		{
			
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			
			// Fils the update region with the COLOR_WINDOW 
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));
			EndPaint(hwnd, &ps);
			
			
			/*
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
    return 0;
*/
			
		}break;
	}
	
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


window_t CreateAWindow(u32 x, u32 y, u32 h, u32 w)
{
	window_t result;
	
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
	result.height = h;
	result.width = w;
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

#endif //_WINDOW


#ifdef _APPLE
#include "window_macos.mm"

#endif // _APPLE
