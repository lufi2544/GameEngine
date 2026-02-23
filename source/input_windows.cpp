

/////////////////////
//// WINDOWS INPUT HANDLING
internal_f u8 
TranslateWindowsInputKey(WPARAM windows_key)
{
	// 1. Handle contiguous Ranges (A-Z and 0-9)
    // This works perfectly as long as your InputKey enum is sequential
    if (windows_key >= 'A' && windows_key <= 'Z')
    {
        return (u8)(InputKey_A + (windows_key - 'A'));
    }
    
    if (windows_key >= '0' && windows_key <= '9')
    {
        return (u8)(InputKey_Num0 + (windows_key - '0'));
    }
	
    // 2. Handle non-contiguous Special Keys
    switch (windows_key)
    {
        case VK_UP:      return InputKey_Up;
        case VK_DOWN:    return InputKey_Down;
        case VK_LEFT:    return InputKey_Left;
        case VK_RIGHT:   return InputKey_Right;
        
        case VK_ESCAPE:  return InputKey_Escape;
        case VK_RETURN:  return InputKey_Enter;
        case VK_SPACE:   return InputKey_Space;
        case VK_TAB:     return InputKey_Tab;
        case VK_BACK:    return InputKey_Backspace;
        
        case VK_SHIFT:   return InputKey_Shift;
        case VK_CONTROL: return InputKey_Ctrl;
        case VK_MENU:    return InputKey_Alt; // 'Alt' key is VK_MENU in Win32
		
        default:         return InputKey_None;
    }
}

internal_f void
ProcessWindowsInputKey(WPARAM windows_key, bool b_is_pressed)
{
	// This is our engine key
	u8 input_key = TranslateWindowsInputKey(windows_key);	
	bool b_current_state = IsKeyPressed(input_key);
	
	if(b_current_state != b_is_pressed)
	{
		SetKeyState(input_key, b_is_pressed);						
	}
	
	
}