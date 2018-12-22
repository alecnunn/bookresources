// GUIManager.h: interface for the CGUIManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMANAGER_H__F8164B98_0DCE_4D54_8824_C0A0A160D708__INCLUDED_)
#define AFX_GUIMANAGER_H__F8164B98_0DCE_4D54_8824_C0A0A160D708__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class cLua;

#define SCRIPTPATH "Scripts\\"
enum
{
	GUI_EVENT_BUTTON_UP = 0,
	GUI_EVENT_BUTTON_DOWN,
	GUI_EVENT_SELECTION_CHANGED,
	GUI_EVENT_TEXTFIELD_CLICKED,
	GUI_KEY_PRESS,
	GUI_REENTER_INTERFACE,
	GUI_TIMER_EXPIRED,
	GUI_ENTER_INTERFACE,
	GUI_TEXT_SCROLL_END,
	GUI_EVENT_TEXTFIELD_RETURN,
	GUI_EVENT_HOVER_TIMED_START,
	GUI_EVENT_HOVER_END,
	GUI_MOUSE_BUTTON_DOWN,
	GUI_MOUSE_BUTTON_UP,
};

class CUserInterface;

class CGUIManager  
{
private:
	static CGUIManager			*m_pInstance;
public:
	static CGUIManager			*GetInstance() {return m_pInstance;}

public:
				CGUIManager();
	virtual		~CGUIManager();
	bool		Init(cLua *pContext);

	bool		StartGUI(const char *pFilename);

	bool		Update(float fSecsElapsed);
	bool		Render();
	bool		IsMouseOverGUI();
	void		ResChangeNotify(void);

	cLua		*GetLuaContext(void) {return m_LuaState;}

	CUserInterface	*GetCurrentUI() {return m_pCurrentUI;}
	void		ClearCache(void);

	void		ScriptExecuteString(char *command);
	void		SendEvent(int iEventCode, int id, float arg1 = 0.0f, float arg2 = 0.0f, float arg3 = 0.0f, float arg4 = 0.0f);

	bool		KeyHit(int ascii);

	float		GetButtonHoverCountdownTime()			{return m_fButtonHoverCountdownTime;}
	void		SetButtonHoverCountdownTime(float f)	{m_fButtonHoverCountdownTime = f;}

	void		NotifyMouseButtonDown(int button);
	void		NotifyMouseButtonUp(int button);

private:

	cLua		*m_LuaState;

	CUserInterface	*m_pCurrentUI;

	std::map<std::string, CUserInterface *> m_GUIMap;

	float		m_fButtonHoverCountdownTime;

	bool		m_bMouseOverGUI;

};

#endif // !defined(AFX_GUIMANAGER_H__F8164B98_0DCE_4D54_8824_C0A0A160D708__INCLUDED_)
