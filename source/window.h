/* date = January 21st 2026 3:55 pm */

#include "Windows.h"

// Create a window and display it.


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	
	switch (msg)
    {
        case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
    }
	
    return DefWindowProc(hwnd, msg, wparam, lparam);
	
}


struct window
{
	
};



window create_window(/* pass in the params **/)
{
	window result;
	
	// Window class registration.
	HINSTANCE Instance = GetModuleHandle(0);
	char CLASS_NAME[] = "engine window";
	
	
	WNDCLASS window_class = {};
	window_class.lpfnWndProc = WindowProc;
	window_class.hInstance = Instance;
	window_class.lpszClassName = CLASS_NAME;
	
	RegisterClass(&window_class);
	
	
	// Window creation
	
	const char WINDOW_NAME[] = "engine";
	
	HWND window_handle = CreateWindowEx(
										0,
										(LPCSTR)CLASS_NAME,
										(LPCSTR)WINDOW_NAME,
										WS_OVERLAPPEDWINDOW,
										200, 200, 
										800, 600,
										0,
										0, 
										Instance,
										0
										);	
	
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
	
	return result;
}