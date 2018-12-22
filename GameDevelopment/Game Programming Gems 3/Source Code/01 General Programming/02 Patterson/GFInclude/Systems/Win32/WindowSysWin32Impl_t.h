/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// WindowSysWin32Impl_t.h: interface for the WindowSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWSYSWIN32IMPL_T_H__1C5F282E_336C_4A3C_A201_7424D160CB76__INCLUDED_)
#define AFX_WINDOWSYSWIN32IMPL_T_H__1C5F282E_336C_4A3C_A201_7424D160CB76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/WindowSys_t.h"

class ProcessInfoWin32Impl_t;

class WindowSysWin32Impl_t : public WindowSys_t  
{
public:
	WindowSysWin32Impl_t();
	virtual ~WindowSysWin32Impl_t();

	virtual void SetProcessInfo( ProcessInfo_t *pProcessInfo );
	virtual void SetWindowInfo( int nWidth, int nHeight );
	virtual void GetWindowInfo( int &nWidth, int &nHeight );
	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void Process();

	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_hWnd;
protected:
	bool GetGFKeyCode( WPARAM wParam, int &nKeyCode );

	ProcessInfoWin32Impl_t *m_pProcessInfo;
	Systems_t *m_pSystems;

	int m_nWidth;
	int m_nHeight;
	String_t m_TitleString;
	String_t m_WindowClassString;

	bool m_bActive;
};

#endif // !defined(AFX_WINDOWSYSWIN32IMPL_T_H__1C5F282E_336C_4A3C_A201_7424D160CB76__INCLUDED_)
