/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Master.h"
#include "GameApp.h"


CWinApp *CWinApp::m_pMainApp;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *pszCmdLine, int nShowCmd)
{
	CGameApp app(hInstance, hPrevInstance, pszCmdLine, nShowCmd);
	if(app.InitInstance())
		app.Run();
	return app.ExitInstance();
}

void CDisplayMode::LoadDisplaySettings()
{
	m_bFullScreen = (GetApp()->GetProfileInt(DISPLAY_SECTION, "FullScreen", 0) != 0);
	m_nWidth = GetApp()->GetProfileInt(DISPLAY_SECTION, "Width", 640);
	m_nHeight = GetApp()->GetProfileInt(DISPLAY_SECTION, "Height", 480);
	m_nBitsPerPixel = m_bFullScreen ? GetApp()->GetProfileInt(DISPLAY_SECTION, "BitsPerPixel", 32) : 0;
	m_nFrequency = m_bFullScreen ? GetApp()->GetProfileInt(DISPLAY_SECTION, "Frequency", 0) : 0;
}

void CDisplayMode::SaveDisplaySettings()
{
	GetApp()->WriteProfileInt(DISPLAY_SECTION, "FullScreen", m_bFullScreen);
	GetApp()->WriteProfileInt(DISPLAY_SECTION, "Width", m_nWidth);
	GetApp()->WriteProfileInt(DISPLAY_SECTION, "Height", m_nHeight);
	GetApp()->WriteProfileInt(DISPLAY_SECTION, "BitsPerPixel", m_nBitsPerPixel);
	GetApp()->WriteProfileInt(DISPLAY_SECTION, "Frequency", m_nFrequency);
}

bool CGameApp::InitInstance()
{
	// Register the window class and create the window
	WNDCLASS wc = {CS_OWNDC | CS_VREDRAW | CS_HREDRAW, (WNDPROC)WindowProc, 0, 0, m_hInstance, LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_APP)), LoadCursor((HINSTANCE)NULL, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), MAKEINTRESOURCE(IDM_APP), m_szAppName};
	if(!RegisterClass(&wc))
	{
		MessageBox("Unable to register window class, aborting.");
		return false;
	}

	// Load the last saved display mode and use it to initialize the window
	LoadDisplaySettings();
	return InitMode(IsFullScreen(), GetWidth(), GetHeight(), GetBitsPerPixel(), GetFrequency());
}

bool CGameApp::InitMode(bool bFullScreen, int nWidth, int nHeight, int nBitsPerPixel, int nFrequency)
{
	if(m_hWnd)
	{
		ResetDisplayMode();
		DestroyWindow();
		m_hWnd = NULL;
	}
	SetFullScreen(bFullScreen);
	SetWidth(nWidth);
	SetHeight(nHeight);
	SetBitsPerPixel(nBitsPerPixel);
	SetFrequency(nFrequency);

	DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | (IsFullScreen() ? WS_POPUP : WS_OVERLAPPEDWINDOW);
#ifdef _DEBUG
	DWORD dwExStyle = 0;
#else
	DWORD dwExStyle = (IsFullScreen() ? WS_EX_TOPMOST : 0);
#endif
	CRect rect(0, 0, GetWidth(), GetHeight());
	if(!CreateEx(m_hInstance, m_szAppName, m_szAppName, dwExStyle, dwStyle, &rect))
	{
		MessageBox("Unable to create application window, aborting.");
		return false;
	}
	if(!IsFullScreen())
	{
		CalcWindowRect(&rect);
		MoveWindow(0, 0, rect.Width(), rect.Height(), false);
	}
	ShowWindow(m_nShowCmd);
	UpdateWindow();
	return true;
}

int CGameApp::ExitInstance()
{
	UnregisterClass(m_szAppName, m_hInstance);
	SaveDisplaySettings();
	ResetDisplayMode();
	return 0;
}

bool CGameApp::OnIdle()
{
	if(!m_bActive)
		return false;
	int nTimer = timeGetTime();
	m_pGameEngine->RenderFrame(nTimer-m_nTimer);
	SwapBuffers(m_hDC);
	m_nTimer = nTimer;
	return true;
}

void CGameApp::Pause()
{
	if(m_bActive)
	{
#ifndef _DEBUG
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
#endif
		m_pGameEngine->Pause();
		m_bActive = false;
		if(IsFullScreen())
		{
			ShowWindow(SW_MINIMIZE);
			ResetDisplayMode();
		}
	}
}

void CGameApp::Restore()
{
	if(!m_bActive)
	{
#ifndef _DEBUG
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#endif
		m_bActive = true;
		if(IsFullScreen())
		{
			ShowWindow(SW_SHOWNORMAL);
			SetDisplayMode();
		}
		m_nTimer = timeGetTime();
		m_pGameEngine->Restore();
	}
}

int CGameApp::OnCreate(HWND hWnd) 
{
	PIXELFORMATDESCRIPTOR pfdDesc;
	memset((char *)&pfdDesc, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfdDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfdDesc.nVersion = 1;
	pfdDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfdDesc.iPixelType = PFD_TYPE_RGBA;
	pfdDesc.iLayerType = PFD_MAIN_PLANE;
	pfdDesc.cColorBits = 32;
	pfdDesc.cAlphaBits = 8;
	pfdDesc.cDepthBits = 32;
	pfdDesc.cStencilBits = 32;

	m_hWnd = hWnd;
	m_hDC = ::GetDC(m_hWnd);
	int nPixelIndex = ChoosePixelFormat(m_hDC, &pfdDesc);
	if(!SetPixelFormat(m_hDC, nPixelIndex, &pfdDesc))
	{
		MessageBox("Error finding a suitable pixel format.");
		return -1;
	}
	DescribePixelFormat(m_hDC, GetPixelFormat(m_hDC), sizeof(PIXELFORMATDESCRIPTOR), &pfdDesc);

	m_hGLRC = wglCreateContext(m_hDC);
	if(!m_hGLRC || !wglMakeCurrent(m_hDC, m_hGLRC))
	{
		MessageBox("Error creating OpenGL rendering context.");
		return -1;
	}

	m_pGameEngine = new CGameEngine;
	return 0;
}

void CGameApp::OnDestroy()
{
	if(m_pGameEngine)
	{
		delete m_pGameEngine;
		m_pGameEngine = NULL;
	}

	if(wglGetCurrentContext())
		wglMakeCurrent(NULL, NULL);
	if(m_hGLRC)
	{
		wglDeleteContext(m_hGLRC);
		m_hGLRC = NULL;
	}
	if(m_hDC)
	{
		::ReleaseDC(m_hWnd, m_hDC);
		m_hDC = NULL;
	}
}

void CGameApp::OnSize(int nType, int nWidth, int nHeight)
{
	if(!nHeight || !nWidth)
		return;
	glViewport(0, 0, nWidth, nHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)nWidth / (double)nHeight, MIN_DISTANCE, MAX_DISTANCE*1.05);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(!IsFullScreen())
	{
		CRect rect;
		GetClientRect(&rect);
		SetWidth(rect.Width());
		SetHeight(rect.Height());
	}
}

bool CGameApp::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
		case ID_FILE_EXIT:
			PostMessage(WM_CLOSE);
			return true;
		case ID_OPTIONS_PLANET:
			MessageBox("This feature has not been implemented yet.");
			return true;
		case ID_HELP_KEYCOMMANDS:
		{
			CDialog dlg(IDD_KEY_COMMANDS, m_hInstance, m_hWnd);
			dlg.DoModal();
			return true;
		}
	}
	return false;
}

LRESULT CALLBACK CGameApp::WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CRect rect;
	switch(nMsg)
	{
		case WM_CREATE:
			return GameApp()->OnCreate(hWnd);
		case WM_DESTROY:
			GameApp()->OnDestroy();
			break;
		case WM_CLOSE:
			GameApp()->DestroyWindow();
			PostQuitMessage(0);
			return 0;
		case WM_COMMAND:
			if(GameApp()->OnCommand(wParam, lParam))
				return 0;
			break;
		case WM_SIZE:
			GameApp()->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_ACTIVATE:
			if(wParam && !GameApp()->IsActive())
				GameApp()->Restore();
			else if(!wParam && GameApp()->IsActive())
				GameApp()->Pause();
			break;
		case WM_GETMINMAXINFO:
			rect = CRect(0, 0, 320, 240);
			CWnd(hWnd).CalcWindowRect(&rect);
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = rect.Width();
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = rect.Height();
			return 0;
		case WM_SYSCOMMAND:
			// Prevent system commands (like closing, moving, sizing, screensaver, power management, etc) when active
			//if(GameApp()->m_bActive)
			//	return 0;
			break;
		case WM_POWERBROADCAST:
			// Prevent power suspend when active
			if(GameApp()->m_bActive && wParam == PBT_APMQUERYSUSPEND)
				return BROADCAST_QUERY_DENY;
			break;
		case WM_SYSKEYDOWN:
			if(wParam == VK_MENU)
				return 0;
			break;
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE)
			{
				if(GameApp()->IsActive())
					GameApp()->Pause();
				else
					GameApp()->Restore();
			}
			break;
		/* Disable full-screen for now
		case WM_CHAR:
			switch(wParam)
			{
				case 'f':
					GameApp()->InitMode(!GameApp()->IsFullScreen(), 640, 480, 32);
					break;
			}
			break;
		*/
	}
	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}
