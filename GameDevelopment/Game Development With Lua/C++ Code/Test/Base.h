// Base.h: interface for the CBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE_H__E7FCE94C_FA81_4A6A_AA84_42DB0D54E386__INCLUDED_)
#define AFX_BASE_H__E7FCE94C_FA81_4A6A_AA84_42DB0D54E386__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWinConsole;
class CGUIManager;

class CBase  
{
public:
				CBase();
	virtual		~CBase();
	bool		Init(HINSTANCE hInstance, const char *szClass, const char *szCaption, WNDPROC WindowProc);
	void		MainLoop();
	void		HandleSize(WPARAM wParam);
	void		Render(float fTime);
	

private:
	cLua		m_lua;
	DXContext	*m_lpDX;

	CGUIManager	*m_pGUIManager;

};

#endif // !defined(AFX_BASE_H__E7FCE94C_FA81_4A6A_AA84_42DB0D54E386__INCLUDED_)
