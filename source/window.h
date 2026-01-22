/* date = January 21st 2026 3:55 pm */

#include "Windows.h"

// Create a window and display it.


struct window
{
	HWND handle;
	
	u32 x;
	u32 y;
	u32 width;
	u32 height;	
};


window CreateAWindow(u32 x, u32 y, u32 h, u32 w);