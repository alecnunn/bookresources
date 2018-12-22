/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Mouse.h
 *    Desc: Wrapper of a DirectInput mouse object
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MOUSE_H
#define _MOUSE_H

#include <dinput.h>

/**
 * Any object that implements this interface can receive input
 * from the mouse.
 */
struct iMouseReceiver
{
	virtual void MouseMoved( int dx, int dy ){};
	virtual void MouseButtonUp( int button ){};
	virtual void MouseButtonDown( int button ){};
};


class cMouse  
{
	LPDIRECTINPUTDEVICE8	m_pDevice; 

	DIMOUSESTATE			m_lastState;

	iMouseReceiver*			m_pTarget;

public:

	cMouse( HWND hWnd, bool bExclusive );
	~cMouse();

	/**
	 * Use this to establish a MouseReceiver as the current 
	 * input focus
	 */
	void SetReceiver( iMouseReceiver* pTarget );

	eResult Update();

	eResult Acquire();
	void UnAcquire();
};

#endif // _MOUSE_H
