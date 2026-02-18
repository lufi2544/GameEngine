/* date = January 21st 2026 3:55 pm */
// Create a window and display it.

struct window_t
{
    
#ifdef _WINDOWS
	HWND handle;
#endif // _WINDOWS
	
    
#ifdef _APPLE
    void* handle;
    
#endif // _APPLE
	u32 x;
	u32 y;
	u32 width;
	u32 height;	
};


global_f window_t
CreateAWindow(u32 x, u32 y, u32 h, u32 w);