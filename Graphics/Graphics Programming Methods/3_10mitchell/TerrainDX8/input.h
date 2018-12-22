// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef INPUT_H
#define INPUT_H

#ifndef DEBUG_H
#include "debug.h"
#endif

#define SAMPLE_BUFFER_SIZE 16  // arbitrary number of buffer elements

class Input_Class
{
public:
	Input_Class();
	~Input_Class();

	void Update();

	BYTE Get_Key(BYTE key) { return Keys[key]; }

	int Get_Mouse_Delta_X() { return Mouse_State.lX; }
	int Get_Mouse_Delta_Y() { return -Mouse_State.lY; }

	BYTE Get_Mouse_LButton() { return Mouse_State.rgbButtons[0]&0x80; }
	BYTE Get_Mouse_RButton() { return Mouse_State.rgbButtons[1]&0x80; }

	void Process_Msg(UINT msg, WPARAM wParam);

private:

	void Key_Up(WPARAM key) { Keys[key]=0; }
	void Key_Down(WPARAM key) { Keys[key]=1; }

	void Mouse_Unacquire() { if (Mouse) Mouse->Unacquire(); }
	void Mouse_Acquire() { if (Mouse) Mouse->Acquire(); }

	void Read_Immediate_Data();
	void Read_Buffered_Data();

	LPDIRECTINPUT8       DI;         
	LPDIRECTINPUTDEVICE8 Mouse;

	DIMOUSESTATE2			Mouse_State;      // DirectInput mouse state structure
	BYTE						Keys[256];
};

#endif
