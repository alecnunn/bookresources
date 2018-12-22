/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// WindowSysWin32Impl_t.cpp: implementation of the WindowSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "resource.h"
#include "GFInclude/Systems/Win32/WindowSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/ProcessInfoWin32Impl_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Systems/VisualSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"

// this callback calls our associated class
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowSysWin32Impl_t *pWindowSys = (WindowSysWin32Impl_t*)GetWindowLong(hWnd, GWL_USERDATA);
	return pWindowSys->WndProc(hWnd, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WindowSysWin32Impl_t::WindowSysWin32Impl_t()
{
	m_pSystems = 0;
	m_nWidth = 800;
	m_nHeight = 600;
	m_bActive = false;
}

WindowSysWin32Impl_t::~WindowSysWin32Impl_t()
{

}

void WindowSysWin32Impl_t::SetProcessInfo( ProcessInfo_t *pProcessInfo )
{
	m_pProcessInfo = (ProcessInfoWin32Impl_t *)pProcessInfo;
}

void WindowSysWin32Impl_t::SetWindowInfo( int nWidth, int nHeight )
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void WindowSysWin32Impl_t::GetWindowInfo( int &nWidth, int &nHeight )
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}

bool WindowSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;

	// Initialize global strings
	m_TitleString = "GameFramework";
	m_WindowClassString = "GAMEFRAMEWORK_020215";

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_pProcessInfo->m_hInst;
	wcex.hIcon			= LoadIcon(wcex.hInstance, (LPCTSTR)101/*IDI_GFDEMO*/); // Icon 101
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= m_WindowClassString.c_str();
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)102/*IDI_SMALL*/); // Icon 102

	RegisterClassEx(&wcex);

	m_hWnd = CreateWindow(m_WindowClassString.c_str(),
		m_TitleString.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_nWidth + (GetSystemMetrics(SM_CXSIZEFRAME)*2),
		m_nHeight + (GetSystemMetrics(SM_CYSIZEFRAME)*2)+GetSystemMetrics(SM_CYCAPTION),
		NULL, NULL, m_pProcessInfo->m_hInst, NULL);

	if(!m_hWnd)
	{
		return false;
	}

	// associate this class with the window
	SetWindowLong(m_hWnd, GWL_USERDATA, (long)this);

	ShowWindow(m_hWnd, m_pProcessInfo->m_nCmdShow);
	UpdateWindow(m_hWnd);
	m_bActive = true;
	
	return true;
}

void WindowSysWin32Impl_t::Shutdown()
{
}

void WindowSysWin32Impl_t::Process()
{
	MSG msg;

	BOOL bGotMsg = TRUE;
	while( bGotMsg )
	{
        if( m_bActive )
		{
            bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		} else {
            bGotMsg = GetMessage( &msg, NULL, 0U, 0U );
		}

        if( bGotMsg )
        {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool WindowSysWin32Impl_t::GetGFKeyCode( WPARAM wParam, int &nKeyCode )
{
	switch( wParam )
	{
	case VK_RETURN:
		nKeyCode = GF_KEYCODE_ENTER;
		break;
	case VK_ESCAPE:
		nKeyCode = GF_KEYCODE_ESCAPE;
		break;
	case VK_LEFT:
		nKeyCode = GF_KEYCODE_LEFT;
		break;
	case VK_UP:
		nKeyCode = GF_KEYCODE_UP;
		break;
	case VK_RIGHT:
		nKeyCode = GF_KEYCODE_RIGHT;
		break;
	case VK_DOWN:
		nKeyCode = GF_KEYCODE_DOWN;
		break;
	default:
		return false;
	}
	return true;
}

LRESULT WindowSysWin32Impl_t::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	//TCHAR szHello[] = "Hello!";
	InputSys_t *pInputSys;
	int nKeyCode;

	switch (message) 
	{
        case WM_KEYUP:
			// only post keys we care about
			if( GetGFKeyCode( wParam, nKeyCode ) )
			{
				pInputSys = m_pSystems->GetInputSys();
				if( pInputSys )
				{
					pInputSys->PostKeyUp( nKeyCode );
				}
			}
            break;
		case WM_KEYDOWN:
			// only post keys we care about
			if( GetGFKeyCode( wParam, nKeyCode ) )
			{
				pInputSys = m_pSystems->GetInputSys();
				if( pInputSys )
				{
					pInputSys->PostKeyDown( nKeyCode );
				}
			}
            break;
		case WM_MOVE:
            // If in windowed mode, move the Framework's window
            if( m_bActive )
			{
				VisualSys_t *pVS = m_pSystems->GetVisualSys();
				if( pVS )
				{
					pVS->WindowMove( (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) );
				}
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			//RECT rt;
			//GetClientRect(hWnd, &rt);
			//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE )
			{
				m_bActive = false;
			} else {
				m_bActive = true;
			}
			break;
		case WM_DESTROY:
			m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_REMOVE_ALL, 0 );
			m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::ASYNC_REMOVE_ALL, 0 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

