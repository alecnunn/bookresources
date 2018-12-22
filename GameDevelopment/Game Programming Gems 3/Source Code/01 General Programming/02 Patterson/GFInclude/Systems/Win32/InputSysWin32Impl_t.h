/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// InputSysWin32Impl_t.h: interface for the InputSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTSYSWIN32IMPL_T_H__5BB2598A_F415_445C_BCD9_5B4E5B2B16DE__INCLUDED_)
#define AFX_INPUTSYSWIN32IMPL_T_H__5BB2598A_F415_445C_BCD9_5B4E5B2B16DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/InputSys_t.h"

typedef std::deque<KeyEvent_t> KeyEventDeque_t;

class KeyQueue_t {
public:
private:
};

class InputSysWin32Impl_t : public InputSys_t  
{
public:
	InputSysWin32Impl_t();
	virtual ~InputSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void PostKeyUp( int nKeyCode );
	virtual void PostKeyDown( int nKeyCode );

	virtual int GetKeyState( int nKeyCode );
	virtual bool IsKeyPending();
	virtual bool PeekKey( KeyEvent_t &KeyEvent );
	virtual bool ReadKey( KeyEvent_t &KeyEvent );
	virtual void ClearKeyQueue();
protected:
	Systems_t *m_pSystems;

	KeyEventDeque_t m_KeyEventDeque;
	unsigned char m_bKey[GF_KEYCODE_COUNT];

};

#endif // !defined(AFX_INPUTSYSWIN32IMPL_T_H__5BB2598A_F415_445C_BCD9_5B4E5B2B16DE__INCLUDED_)
