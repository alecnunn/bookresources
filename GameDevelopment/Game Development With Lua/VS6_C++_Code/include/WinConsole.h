// WinConsole.h: interface for the CWinConsole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINCONSOLE_H__B7711619_397D_4663_A391_9E68B4D82FD7__INCLUDED_)
#define AFX_WINCONSOLE_H__B7711619_397D_4663_A391_9E68B4D82FD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cLua;

class CWinConsole  
{
public:
	static HWND StartConsole(HINSTANCE hInstance, cLua *pScriptContext);
	static void StopConsole();
	static void Write(const char *pString);

	static LRESULT WINAPI MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool	ConsoleReady(void)	{return m_hWnd!= NULL;}

private:
	CWinConsole();
	virtual ~CWinConsole();

	void	Init(HINSTANCE hInstance);
	void	ResizeControls(void);
	void	AdjustScrollBar(void);
	void	Paint(HDC hDC);


private:
	static volatile bool	m_bWinIsActive;
	static volatile HWND m_hWnd;
	static volatile HWND m_hEditControl;
	static char m_CommandBuffer[4096];
	
	std::list<std::string> m_stringList;
	HINSTANCE m_hInstance;

	cLua *m_pScriptContext;

	int m_ScrollyPos;
	int m_textAreaHeight;
};


extern CWinConsole *g_Console;

#endif // !defined(AFX_WINCONSOLE_H__B7711619_397D_4663_A391_9E68B4D82FD7__INCLUDED_)
