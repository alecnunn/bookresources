// Button.h: interface for the CButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUTTON_H__A2CC7298_99CF_427E_A5EA_8079D1CD5551__INCLUDED_)
#define AFX_BUTTON_H__A2CC7298_99CF_427E_A5EA_8079D1CD5551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUIObject.h"

class Sprite;

enum eButtonState
{
	BUTTON_NORMAL = 0,
	BUTTON_HOVER,
	BUTTON_SELECTED,
	BUTTON_DISABLED
};

class CTexture;

class CButton : public CGUIObject
{
public:
							CButton();
	virtual					~CButton();
	virtual	const char		*GetObjectTypeName() {return kpButtonName;}
	virtual int				ObjectCommand(const char *pCommand);

	bool					SetButtonTextures(char *normal, char *hover=NULL, char *selected=NULL, char *disabled=NULL);
	virtual bool			Update(float fSecsElapsed, CUserInterface *pParent);
//	virtual void			ResChangeNotify(void);

	virtual void			SetPosition(float fx, float fy, float fw, float fh, float z);
	virtual bool			StealsMouse(int x, int y);

	void					SetButtonState(eButtonState state) {m_buttonState = state;}

protected:
	Sprite					*m_pButtonTextures[4];
	eButtonState			m_buttonState;
	RECT					m_rHotSpot;
	float					m_fHotx, m_fHoty, m_fHotw, m_fHoth;
	float					m_fButtonHoverCountdown;
};

#endif // !defined(AFX_BUTTON_H__A2CC7298_99CF_427E_A5EA_8079D1CD5551__INCLUDED_)
