// Sprite.cpp: implementation of the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GUIManager.h"
#include "GUISprite.h"

const char *kpSpriteName = "Sprite";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUISprite::CGUISprite()
{
}

CGUISprite::~CGUISprite()
{
}


int	CGUISprite::ObjectCommand(const char *pCommand)
{
	cLua	*L = CGUIManager::GetInstance()->GetLuaContext();
	int retVal = 0;

	if(strcmp(pCommand, "SetRotation") == 0)
	{
		// rotate this arg rot (in radians) 
		float rot =  (float) L->GetNumberArgument(3);
		m_pTexture->SetRotation(-1, -1, rot);
	}
	if(strcmp(pCommand, "SetCenter") == 0)
	{
		// Set the rotation center args x, y (local coords) 
		float x =  (float) L->GetNumberArgument(3);
		float y =  (float) L->GetNumberArgument(4);
		m_pTexture->SetRotation(x, y, -1);
	}

	return retVal;
}

bool CGUISprite::StealsMouse(CMouse *pMouse)
{
	return false;
}

bool CGUISprite::StealsMouse(int x, int y)
{
	return false;
}


RECT CGUISprite::GetBoundingRect()
{
	return m_pTexture->GetBoundingRect();
}


