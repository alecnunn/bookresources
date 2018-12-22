// Sprite.h: interface for the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__615C9020_A178_47F8_B9A7_78FB6AAAAF98__INCLUDED_)
#define AFX_SPRITE_H__615C9020_A178_47F8_B9A7_78FB6AAAAF98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GUIObject.h"

class CGUISprite : public CGUIObject
{
public:
						CGUISprite();
	virtual				~CGUISprite();
	virtual	const char	*GetObjectTypeName() {return kpSpriteName;}
	virtual int			ObjectCommand(const char *pCommand);
	virtual bool		StealsMouse(CMouse *pMouse);
	virtual bool		StealsMouse(int x, int y);
	
	RECT				GetBoundingRect();
};

#endif // !defined(AFX_SPRITE_H__615C9020_A178_47F8_B9A7_78FB6AAAAF98__INCLUDED_)
