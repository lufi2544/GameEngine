/* date = January 22nd 2026 7:53 pm */


/////////// Unique Windows platform handling

/** For windows this is the messate handling */
global_f bool 
PlatformUpdate()
{
	MSG msg;
	BOOL message_result = GetMessage(&msg, NULL, 0, 0);	
	if(message_result == -1)
	{
		
	}
	else
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return message_result;
}