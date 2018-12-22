// input
// (c) Kenny Mitchell - Westwood Studios EA 2002

#ifndef _INPUT_H
#define _INPUT_H


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Input
{
public:
	Input();
	~Input();

	void Update();

	BYTE GetKey(BYTE key) { return m_Keys[key]; }

	int GetMouseDeltaX() { return m_MouseState.lX; }
	int GetMouseDeltaY() { return -m_MouseState.lY; }

	BYTE GetMouseLButton() { return m_MouseState.rgbButtons[0]&0x80; }
	BYTE GetMouseRButton() { return m_MouseState.rgbButtons[1]&0x80; }

	void ProcessMsg(UINT msg, WPARAM wParam);

private:

	void KeyUp(WPARAM key) { m_Keys[key]=0; }
	void KeyDown(WPARAM key) { m_Keys[key]=1; }

	void MouseUnacquire() { if (m_pMouse) m_pMouse->Unacquire(); }
	void MouseAcquire() { if (m_pMouse) m_pMouse->Acquire(); }

	void ReadImmediateData();
	void ReadBufferedData();

	LPDIRECTINPUT8       m_pDI;         
	LPDIRECTINPUTDEVICE8 m_pMouse;

	DIMOUSESTATE2			m_MouseState;      // DirectInput mouse state structure
	BYTE						m_Keys[256];
};

#endif
