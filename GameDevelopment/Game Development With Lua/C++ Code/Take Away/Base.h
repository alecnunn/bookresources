// Base.h: interface for the CBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE_H__E7FCE94C_FA81_4A6A_AA84_42DB0D54E386__INCLUDED_)
#define AFX_BASE_H__E7FCE94C_FA81_4A6A_AA84_42DB0D54E386__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MOUSE_LBUTTON_DOWN 1
#define MOUSE_RBUTTON_DOWN 2

class CWinConsole;
class CGUIManager;
class Settings;

class CBase  
{
public:
				CBase();
	virtual		~CBase();
	bool		Init(HINSTANCE hInstance, const char *szClass, const char *szCaption, WNDPROC WindowProc);
	void		MainLoop();
	void		HandleSize(WPARAM wParam);
	void		Render(float fTime);
	void		KeyHit(int k);
	Settings	*GetSettings() {return m_pSettings;}
	HWND		GetConsole() {return m_hConsole;}
	cLua		*GetLua() {return &m_lua;}

private:
	cLua		m_lua;
	DXBase		*m_lpDX;

	CGUIManager	*m_pGUIManager;
	Settings	*m_pSettings;
	HWND		m_hConsole;
};

#endif // !defined(AFX_BASE_H__E7FCE94C_FA81_4A6A_AA84_42DB0D54E386__INCLUDED_)
