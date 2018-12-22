/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Keyboard.h
 *    Desc: Wrapper of a DirectInput keyboard object
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <memory.h>
#include <dinput.h>

class cInputLayer;

/**
 * Any object that implements this interface can receive input
 * from the keyboard.
 */
struct iKeyboardReceiver
{
	virtual void KeyUp( int key ){};
	virtual void KeyDown( int key ){};
};


class cKeyboard  
{

	LPDIRECTINPUTDEVICE8	m_pDevice;	// The DInput device used to encapsulate the keyboard

	char					m_keyState[256];

	iKeyboardReceiver*		m_pTarget;

public:

	void ClearTable()
	{
		memset( m_keyState, 0, sizeof(char)*256 );
	}

	cKeyboard( HWND hWnd );
	~cKeyboard();

	// Poll to see if a certain key is down
	bool Poll( int key );

	// Use this to establish a KeyboardReceiver as the current input focus
	void SetReceiver( iKeyboardReceiver* pTarget );

	eResult Update();
};

#endif //_KEYBOARD_H
