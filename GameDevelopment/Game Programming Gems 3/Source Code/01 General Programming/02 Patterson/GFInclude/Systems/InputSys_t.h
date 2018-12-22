/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// InputSys_t.h: interface for the InputSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTSYS_T_H__41747CBB_A850_4183_AB89_A0647A291B12__INCLUDED_)
#define AFX_INPUTSYS_T_H__41747CBB_A850_4183_AB89_A0647A291B12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum {
	GF_KEYCODE_NONE,
	GF_KEYCODE_ENTER,
	GF_KEYCODE_ESCAPE,
	GF_KEYCODE_UP,
	GF_KEYCODE_DOWN,
	GF_KEYCODE_LEFT,
	GF_KEYCODE_RIGHT,
	GF_KEYCODE_COUNT
};

class KeyEvent_t {
public:
	typedef enum {
		EVENT_NONE,
		EVENT_KEY_DOWN,
		EVENT_KEY_UP,
	}KeyEventType_t;

	KeyEventType_t m_nType;	// EVENT_XXX, type of event
	long m_nKeyCode;		// key code
};

class Systems_t;

class InputSys_t  
{
public:
	InputSys_t() {};
	virtual ~InputSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void PostKeyUp( int nKeyCode ) = 0;
	virtual void PostKeyDown( int nKeyCode ) = 0;

	virtual int GetKeyState( int nKeyCode ) = 0;
	virtual bool IsKeyPending() = 0;
	virtual bool PeekKey( KeyEvent_t &KeyEvent ) = 0;
	virtual bool ReadKey( KeyEvent_t &KeyEvent ) = 0;
	virtual void ClearKeyQueue() = 0;
};

#endif // !defined(AFX_INPUTSYS_T_H__41747CBB_A850_4183_AB89_A0647A291B12__INCLUDED_)
