// UserInterface.cpp: implementation of the CUserInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <cLua.h>
#include <DXBase.h>

#include "GUIObject.h"
#include "GUISprite.h"
#include "GUIManager.h"
#include "UserInterface.h"

extern bool		g_bDebugMode;
extern Font		*g_pDebugFont;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUserInterface::CUserInterface()
{
	m_pEventHandlerName = NULL;
	m_fTransW = m_fTransH = 1.0f;
	m_bTimerActive = false;
}

CUserInterface::~CUserInterface()
{
	{
		std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
		while(it != m_mapObjects.end())
		{
			SAFE_DELETE((*it).second);

			it++;
		}
	}


	SAFE_DELETE_ARRAY(m_pEventHandlerName);
}

bool CUserInterface::Init(const char *fname)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	lua->RunScript(fname);
	return true;
}

void CUserInterface::SetGUIObjectPosition(unsigned int id, float fx, float fy, float fw, float fh)
{
#if 0
	// must account for w/h errors in other parts of Mark's awful code
	// by reducing w/h by 1.
	float tw = (fw-1) / m_fTransW;
	float th = (fh-1) / m_fTransH;
#else
	float tw = fw / m_fTransW;
	float th = fh / m_fTransH;
#endif

	// if passed neg value, pass neg translation values for
	// the GUIObject to deal with on a texture based pixel ratio
	if(fw < 0.0f)
		tw = -m_fTransW;

	if(fh < 0.0f)
		th = -m_fTransH;

	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.find(id);
	if(it != m_mapObjects.end())
	{
		(*it).second->SetPosition(fx/m_fTransW, fy/m_fTransH, tw, th, 0.99f);
	}
}

void CUserInterface::SetEventHandler(const char *pHandlerName)
{
	SAFE_DELETE_ARRAY(m_pEventHandlerName);
	m_pEventHandlerName = new char[strlen(pHandlerName)+1];
	strcpy(m_pEventHandlerName, pHandlerName);
}


CGUIObject *CUserInterface::FindObject(unsigned int id)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.find(id);
	if(it != m_mapObjects.end())
	{
		return (*it).second;
	}
	return NULL;
}


bool CUserInterface::KeyHit(int ascii)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
	while(it != m_mapObjects.end())
	{
		if(((*it).second)->KeyHit(ascii))
			return true;

		it++;
	}

	// if we got here, no objects wanted the keypress, so send it to the script
	CGUIManager::GetInstance()->SendEvent(GUI_KEY_PRESS, ascii);
	return true;
}

bool CUserInterface::Update(float fSecsElapsed)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
	while(it != m_mapObjects.end())
	{
		if(!((*it).second)->isDisabled())
		{
			if(!((*it).second)->Update(fSecsElapsed, this))
				it = m_mapObjects.end();
		}

		if(it != m_mapObjects.end())
			++it;
	}

	if(m_bTimerActive)
	{
		m_fTimer -= fSecsElapsed;
		if(m_fTimer <= 0.0f)
		{
			m_bTimerActive = false;
			CGUIManager::GetInstance()->SendEvent(GUI_TIMER_EXPIRED, 0);
			
		}
	}

	return true;
}

bool CUserInterface::Render(void)
{
	if(m_mapObjects.size())
	{
		std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
		while(it != m_mapObjects.end())
		{
			if(((*it).second)->isDrawing())
				((*it).second)->Render();

			it++;
		}
#if 0
		if(g_bDebugMode && g_pDebugFont)
		{
			char buf[40];
			it = m_mapObjects.begin();
			while(it != m_mapObjects.end())
			{
				if(((*it).second)->isDrawing())
				{
					int id = ((*it).second)->GetID();
					RECT r = ((*it).second)->GetScreenRect();
					sprintf(buf, "%d", id);
					g_pDebugFont->DrawText(r.left, r.top, 0.0f, RGB(255,0,255), buf);
				}

				it++;
			}

		}
#endif

	}
	return true;
}


bool CUserInterface::IsMouseOverGUI()
{
#if 0
	int x, y, buttons;
	g_pGUIBase->MouseStatus(x, y, buttons);

	std::map<unsigned int,CGUIObject *>::iterator it;
	for (it = m_mapObjects.begin(); it != m_mapObjects.end(); ++it)
	{
		if (it->second->StealsMouse(x, y))
			return true;
	}
#endif
	return false;
}


void CUserInterface::SetGUIObjectFont(unsigned int id, char *fontName, int fontSize)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.find(id);
	if(it != m_mapObjects.end())
	{
		(*it).second->SetFont(fontName, fontSize / m_fTransH);
	}
}

int CUserInterface::ObjectCommand(unsigned int id, const char *pCommand)
{
	int retVal = 0;
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.find(id);
	if(it != m_mapObjects.end())
	{
		retVal = (*it).second->ObjectCommand(pCommand);
	}

	return retVal;
}
	
void CUserInterface::DeleteGUIObject(unsigned int id)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.find(id);

	if(it != m_mapObjects.end())
	{
		SAFE_DELETE((*it).second);
		m_mapObjects.erase(it);
	}
}

void CUserInterface::EnableObject(unsigned int id, bool bDisable, bool bKeepDrawing)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.find(id);
	if(it != m_mapObjects.end())
	{
		(*it).second->Enable(bDisable, bKeepDrawing);
	}
}

void CUserInterface::ResChangeNotify(void)
{
	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
	while(it != m_mapObjects.end())
	{
		(*it).second->ResChangeNotify();
		it++;
	}
}


bool InRect(int x, int y, const RECT &r);

std::list<unsigned int> CUserInterface::SpriteCollision(unsigned int id)
{
	std::list<unsigned int> retVal;

	CGUISprite *pSprite = (CGUISprite *)FindObject(id);
	if(pSprite && (strcmp(pSprite->GetObjectTypeName(), kpSpriteName)==0))
	{
		// the passed GUI object is indeed a sprite
		RECT rOne = pSprite->GetBoundingRect();
		std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
		while(it != m_mapObjects.end())
		{
			bool bBoom = false;
			if(((*it).first != id) && strcmp((*it).second->GetObjectTypeName(), kpSpriteName)==0)
			{
				// this one is not the same as the first and is a sprite
				CGUISprite *pTarget = (CGUISprite *) (*it).second;
				RECT rTwo = pTarget->GetBoundingRect();
				// see if the rects collide
				if(InRect(rTwo.right, rTwo.top, rOne))
				{
					bBoom = true;
				}
				else
				{
					if(InRect(rTwo.right, rTwo.bottom, rOne))
					{
						bBoom = true;
					}
					else
					{
						if(InRect(rTwo.left, rTwo.top, rOne))
						{
							bBoom = true;
						}
						else
						{

							if(InRect(rTwo.left, rTwo.bottom, rOne))
							{
								bBoom = true;
							}
						}
					}
				}
			}

			if(bBoom)
			{
				retVal.push_back((*it).first);
			}
			it++;
		}
	}

	return retVal;
}

std::list<unsigned int> CUserInterface::HitTest(float x, float y)
{
	std::list<unsigned int> retVal;

	std::map<unsigned int,CGUIObject *>::iterator it = m_mapObjects.begin();
	while(it != m_mapObjects.end())
	{
		CGUIObject *pObject = (*it).second;

		if(InRect(x, y, pObject->GetScreenRect()))
		{
			retVal.push_back((*it).first);
		}


		++it;
	}

	return retVal;
}

