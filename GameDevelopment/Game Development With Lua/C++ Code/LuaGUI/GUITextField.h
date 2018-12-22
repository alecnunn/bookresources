// TextField.h: interface for the CTextField class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTFIELD_H__51BC3A5A_8B78_4C7C_98D6_FDF290261A1A__INCLUDED_)
#define AFX_TEXTFIELD_H__51BC3A5A_8B78_4C7C_98D6_FDF290261A1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUIObject.h"

class CTextField : public CGUIObject  
{
public:
	CTextField();
	virtual ~CTextField();
	virtual	const char	*GetObjectTypeName() {return kpTextFieldName;}

	virtual int		ObjectCommand(const char *pCommand);
	virtual bool		Update(float fSecsElapsed, CUserInterface *pParent);
	virtual bool		Render(void);
	virtual	bool		KeyHit(int ascii);
	virtual bool		StealsMouse(CMouse *pMouse) {return false;}
	virtual bool		StealsMouse(int x, int y);

protected:
	std::string m_string;
	std::list<std::string>	m_lstStrings;

	D3DCOLOR	m_color;
	unsigned int		m_iStringPixels;

	bool		m_bKeyboardFocus;
	bool		m_bMouseDownLastTime;
	bool		m_bIsEditable;

	float		m_fScrollSpeed;		// in Chars per Sec
	float		m_fCurrentPosition;

	float		m_fSavedScrollSpeed;

};

#endif // !defined(AFX_TEXTFIELD_H__51BC3A5A_8B78_4C7C_98D6_FDF290261A1A__INCLUDED_)
