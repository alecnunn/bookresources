// UserInterface.h: interface for the CUserInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERINTERFACE_H__24C5747D_FEDD_4906_B84B_B35295719258__INCLUDED_)
#define AFX_USERINTERFACE_H__24C5747D_FEDD_4906_B84B_B35295719258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Sprite;

class CGUIObject;

class CUserInterface  
{
public:
				CUserInterface();
	virtual		~CUserInterface();
	bool		Init(const char *fname);

	bool		Update(float fSecsElapsed);
	bool		Render(void);
	bool		IsMouseOverGUI();
	void		ResChangeNotify(void);

	void		AddGUIObject(unsigned int id, CGUIObject *pObject) {m_mapObjects[id] = pObject;}
	void		DeleteGUIObject(unsigned int id);
	void		SetGUIObjectPosition(unsigned int id, float fx, float fy, float fw, float fh);
	void		SetGUIObjectFont(unsigned int id, char *fontName, int fontSize);
	void		SetCoordTrans(float w, float h) {m_fTransW = w; m_fTransH = h;}
	float		GetCoordTransWidth() {return m_fTransW;}
	float		GetCoordTransHeight() {return m_fTransH;}

	int			ObjectCommand(unsigned int id, const char *pCommand);
	void		EnableObject(unsigned int id, bool bDisable, bool bKeepDrawing);

	CGUIObject	*FindObject(unsigned int id);

	void		SetEventHandler(const char *pHandlerName);
	const char	*GetEventHandler(void) {return m_pEventHandlerName;}

	bool		KeyHit(int ascii);
	std::list<unsigned int> HitTest(float x, float y);
	void		StartTimer(float fTime) {m_bTimerActive = true; m_fTimer = fTime;}

	std::list<unsigned int> SpriteCollision(unsigned int id);

protected:
	std::map<unsigned int, CGUIObject *>				m_mapObjects;

	char		*m_pEventHandlerName;
	float		m_fTransW;
	float		m_fTransH;
	float		m_fTimer;
	bool		m_bTimerActive;
};

#endif // !defined(AFX_USERINTERFACE_H__24C5747D_FEDD_4906_B84B_B35295719258__INCLUDED_)
