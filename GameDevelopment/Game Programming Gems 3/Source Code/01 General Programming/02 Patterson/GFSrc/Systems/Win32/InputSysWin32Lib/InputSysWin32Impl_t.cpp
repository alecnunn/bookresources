/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// InputSysWin32Impl_t.cpp: implementation of the InputSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/InputSysWin32Impl_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InputSysWin32Impl_t::InputSysWin32Impl_t()
{
	m_pSystems = 0;
	memset(m_bKey, 0x00, GF_KEYCODE_COUNT); 
}

InputSysWin32Impl_t::~InputSysWin32Impl_t()
{

}

bool InputSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void InputSysWin32Impl_t::Shutdown()
{
}

void InputSysWin32Impl_t::PostKeyUp( int nKeyCode )
{
	// won't accept invalid keys
	if( (nKeyCode >= GF_KEYCODE_COUNT) && (nKeyCode < 0) )
	{
		return;
	}

	// update immediate state
	m_bKey[nKeyCode] = 0;

	// update event queue
	KeyEvent_t KE;
	KE.m_nType = KeyEvent_t::EVENT_KEY_UP;
	KE.m_nKeyCode = nKeyCode;
	m_KeyEventDeque.push_front( KE );
}

void InputSysWin32Impl_t::PostKeyDown( int nKeyCode )
{
	// won't accept invalid keys
	if( (nKeyCode >= GF_KEYCODE_COUNT) && (nKeyCode < 0) )
	{
		return;
	}

	// update immediate state
	m_bKey[nKeyCode] = 1;

	// update event queue
	KeyEvent_t KE;
	KE.m_nType = KeyEvent_t::EVENT_KEY_DOWN;
	KE.m_nKeyCode = nKeyCode;
	m_KeyEventDeque.push_front( KE );
}

int InputSysWin32Impl_t::GetKeyState( int nKeyCode )
{
	// won't accept invalid keys
	if( (nKeyCode >= GF_KEYCODE_COUNT) && (nKeyCode < 0) )
	{
		return 0;
	}

	return m_bKey[nKeyCode];
}

bool InputSysWin32Impl_t::IsKeyPending()
{
	return !m_KeyEventDeque.empty();
}

bool InputSysWin32Impl_t::PeekKey(KeyEvent_t &KeyEvent)
{
	if( !m_KeyEventDeque.empty() )
	{
		KeyEvent = m_KeyEventDeque.back();
		return true;
	} else {
		return false;
	}
}

bool InputSysWin32Impl_t::ReadKey(KeyEvent_t &KeyEvent)
{
	if( !m_KeyEventDeque.empty() )
	{
		KeyEvent = m_KeyEventDeque.back();
		m_KeyEventDeque.pop_back();
		return true;
	} else {
		return false;
	}
}

void InputSysWin32Impl_t::ClearKeyQueue()
{
	m_KeyEventDeque.clear();
}

