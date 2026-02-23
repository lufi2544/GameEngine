

////////////////////////////////////
/// Main Input file for the engine 
///////////////////////////////////

#define INPUT_DEBUG 1

MY_ENUM()
enum InputKey : u8
{
    InputKey_None = 0,
	
    // Letters
    InputKey_A,
    InputKey_B,
    InputKey_C,
    InputKey_D,
    InputKey_E,
    InputKey_F,
    InputKey_G,
    InputKey_H,
    InputKey_I,
    InputKey_J,
    InputKey_K,
    InputKey_L,
    InputKey_M,
    InputKey_N,
    InputKey_O,
    InputKey_P,
    InputKey_Q,
    InputKey_R,
    InputKey_S,
    InputKey_T,
    InputKey_U,
    InputKey_V,
    InputKey_W,
    InputKey_X,
    InputKey_Y,
    InputKey_Z,
	
    // Numbers
    InputKey_Num0,
    InputKey_Num1,
    InputKey_Num2,
    InputKey_Num3,
    InputKey_Num4,
    InputKey_Num5,
    InputKey_Num6,
    InputKey_Num7,
    InputKey_Num8,
    InputKey_Num9,
	
    // Arrow Keys
    InputKey_Up,
    InputKey_Down,
    InputKey_Left,
    InputKey_Right,
	
    // Special Keys
    InputKey_Escape,
    InputKey_Enter,
    InputKey_Space,
    InputKey_Tab,
    InputKey_Backspace,
    InputKey_Shift,
    InputKey_Ctrl,
    InputKey_Alt,
		
		
	InputKey_Num
};


global bool g_input[InputKey_Num - 1];

global_f bool
IsKeyPressed(u8 key)
{
	assert(key <= ArrayCount(g_input));
	
	return g_input[key];
}

global_f void
SetKeyState(u8 key, bool b_is_pressed)
{
	assert(key <= ArrayCount(g_input));		
		
	g_input[key] = b_is_pressed;
	
#if INPUT_DEBUG		
	if(b_is_pressed)
	{
		MAYORANA_LOG("Input %u presses", key);
	}
	else
	{
		MAYORANA_LOG("Input %u release", key);
	}
#endif // INPUT_DEBUG
	
}


internal_f void
HandleInput()
{
	if(IsKeyPressed(InputKey_Escape))
	{
		g_engine->is_running = false;
	}
}