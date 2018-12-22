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

#ifndef __GameApp_h__
#define __GameApp_h__

#include "GameEngine.h"

/*******************************************************************************
* Class: CDisplayMode
********************************************************************************
* This class just makes it easier for the main part of the program to handle
* display modes. Windowed mode is also treated as a display mode.

// Build list of valid display modes
DEVMODE dm;
CDisplayModeList llModes;
for(int i=0; EnumDisplaySettings(NULL, i, &dm); i++)
{
	if(dm.dmBitsPerPel >= 16 && dm.dmPelsHeight >= 480)
	{
		CDisplayMode *pMode = new CDisplayMode(dm);
		TRACE("%s Mode: %dx%d, %d BPP, %d Hz", pMode->IsFullScreen() ? "Fullscreen" : "Windowed", pMode->GetWidth(), pMode->GetHeight(), pMode->GetBitsPerPixel(), pMode->GetFrequency());
		llModes.AddTail(pMode);
	}
}
*******************************************************************************/
#define CDisplayModeList CLinkedList<CDisplayMode>
class CDisplayMode : public CListNode<CDisplayMode>
{
protected:
	bool m_bFullScreen;
	int m_nWidth;
	int m_nHeight;
	int m_nBitsPerPixel;
	int m_nFrequency;

public:
	CDisplayMode()	{}
	CDisplayMode(bool bFullScreen, int nWidth, int nHeight, int nBitsPerPixel=0, int nFrequency=0)
	{
		m_bFullScreen = bFullScreen;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_nBitsPerPixel = nBitsPerPixel;
		m_nFrequency = nFrequency;
	}
	CDisplayMode(CDisplayMode &dm)		{ *this = dm; }
	CDisplayMode(DEVMODE &dm)			{ *this = dm; }
	bool operator==(CDisplayMode &dm)	{ return (m_bFullScreen == dm.m_bFullScreen && m_nWidth == dm.m_nWidth && m_nHeight == dm.m_nHeight && m_nBitsPerPixel == dm.m_nBitsPerPixel && m_nFrequency == dm.m_nFrequency); }
	bool operator==(DEVMODE &dm)		{ return (m_bFullScreen == true && m_nWidth == (int)dm.dmPelsWidth && m_nHeight == (int)dm.dmPelsHeight && m_nBitsPerPixel == (int)dm.dmBitsPerPel && m_nFrequency == (int)dm.dmDisplayFrequency); }
	void operator=(CDisplayMode &dm)
	{
		m_bFullScreen = dm.m_bFullScreen;
		m_nWidth = dm.m_nWidth;
		m_nHeight = dm.m_nHeight;
		m_nBitsPerPixel = dm.m_nBitsPerPixel;
		m_nFrequency = dm.m_nFrequency;
	}
	void operator=(DEVMODE &dm)
	{
		m_bFullScreen = true;
		m_nWidth = dm.dmPelsWidth;
		m_nHeight = dm.dmPelsHeight;
		m_nBitsPerPixel = dm.dmBitsPerPel;
		m_nFrequency = dm.dmDisplayFrequency;
	}
	bool operator<(CDisplayMode &dm)
	{
		return	// Reorder the statements below to change how display modes are sorted
				(m_nWidth < dm.m_nWidth ||					// Then sort by width
				(m_nWidth == dm.m_nWidth &&
				(m_nHeight < dm.m_nHeight ||				// Then sort by height
				(m_nHeight == dm.m_nHeight &&
				(m_nBitsPerPixel < dm.m_nBitsPerPixel ||	// Then sort by bits per pixel
				(m_nBitsPerPixel == dm.m_nBitsPerPixel && 
				(m_nFrequency < dm.m_nFrequency //||			// Then sort by frequency
				//(m_nFrequency == dm.m_nFrequency &&
				)))))));
	}
	void LoadDisplaySettings();
	void SaveDisplaySettings();

	bool IsFullScreen()		{ return m_bFullScreen; }
	int GetWidth()			{ return m_nWidth; }
	int GetHeight()			{ return m_nHeight; }
	int GetBitsPerPixel()	{ return m_nBitsPerPixel; }
	int GetFrequency()		{ return m_nFrequency; }

	void SetFullScreen(bool b)
	{
		m_bFullScreen = b;
		if(!m_bFullScreen)
			m_nBitsPerPixel = m_nFrequency = 0;
	}
	void SetWidth(int n)		{ m_nWidth = n; }
	void SetHeight(int n)		{ m_nHeight = n; }
	void SetBitsPerPixel(int n)	{ if(m_bFullScreen) m_nBitsPerPixel = n; }
	void SetFrequency(int n)	{ if(m_bFullScreen) m_nFrequency = n; }

	LONG SetDisplayMode()
	{
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		dm.dmPelsWidth = m_nWidth;
		dm.dmPelsHeight = m_nHeight;
		dm.dmBitsPerPel = m_nBitsPerPixel;
		if(m_nFrequency)
		{
			dm.dmFields |= DM_DISPLAYFREQUENCY;
			dm.dmDisplayFrequency = m_nFrequency;
		}
		//return ChangeDisplaySettingsEx(NULL, &dm, NULL, CDS_FULLSCREEN, NULL);
		return ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}
	static LONG ResetDisplayMode()
	{
		//return ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);
		return ChangeDisplaySettings(NULL, CDS_FULLSCREEN);
	}
};

/*******************************************************************************
* Class: CGameApp
********************************************************************************
* This class is similar to MFC's CWinApp, providing a lot of the same functions
* without the extra baggage of MFC. This instance handles all the Windows-specific
* OpenGL initialization/cleanup code. It also creates an instance of CGameEngine
* and calls its RenderFrame() function in OnIdle().
*******************************************************************************/
class CGameApp : public CWinApp, public CDisplayMode
{
// Attributes
protected:
	HDC m_hDC;
	HGLRC m_hGLRC;
	bool m_bActive;
	int m_nTimer;

	CGameEngine *m_pGameEngine;

// Operations
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	int OnCreate(HWND hWnd);
	void OnDestroy();
	void OnSize(int nType, int nWidth, int nHeight);
	bool OnCommand(WPARAM wParam, LPARAM lParam);

public:
	CGameApp(HINSTANCE hInstance, HINSTANCE hPrevInstance=NULL, char *pszCmdLine="", int nShowCmd=SW_SHOWNORMAL)
		: CWinApp(hInstance, hPrevInstance, pszCmdLine, nShowCmd)
	{
		m_hDC = NULL;
		m_hGLRC = NULL;
		m_bActive = false;
		m_pGameEngine = NULL;
	}

	virtual bool InitInstance();
	virtual int ExitInstance();
	virtual bool OnIdle();
	virtual void Pause();
	virtual void Restore();
	virtual bool InitMode(bool bFullScreen, int nWidth, int nHeight, int nBitsPerPixel=0, int nFrequency=0);
	bool IsActive()								{ return m_bActive; }
	void MakeCurrent()							{ wglMakeCurrent(m_hDC, m_hGLRC); }
	HDC GetHDC()								{ return m_hDC; }
};

inline CGameApp *GameApp()	{ return (CGameApp *)GetApp(); }

#endif // __GameApp_h__
