// Button.cpp: implementation of the CButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GUIManager.h"
#include "GUIButton.h"

const char *kpButtonName = "Button";
#define MOUSE_LBUTTON_DOWN 1
#define MOUSE_RBUTTON_DOWN 2


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CButton::CButton()
{
	m_buttonState = BUTTON_NORMAL;
	for(int i=0; i<4; i++)
	{
		m_pButtonTextures[i] = NULL;
	}

	m_fButtonHoverCountdown = 0.0f;
}

CButton::~CButton()
{
	for(int i=0; i<4; i++)
	{
		SAFE_DELETE(m_pButtonTextures[i]);
	}

	// the base class with try to delete m_pTexture if it is not NULL
	// since we already killed all out textures, don't do that...
	m_pTexture = NULL;
}

bool CButton::SetButtonTextures(char *normal, char *hover, char *selected, char *disabled)
{
	for(int i=0; i<4; i++)
	{
		SAFE_DELETE(m_pButtonTextures[i]);
	}

	if(disabled)
	{
		m_pTexture = NULL;
		SetTexture(disabled);
		m_pButtonTextures[BUTTON_DISABLED] = m_pTexture;
	}
	
	if(selected)
	{
		m_pTexture = NULL;
		SetTexture(selected);
		m_pButtonTextures[BUTTON_SELECTED] = m_pTexture;
	}

	if(hover)
	{
		m_pTexture = NULL;
		SetTexture(hover);
		m_pButtonTextures[BUTTON_HOVER] = m_pTexture;
	}

	m_pTexture = NULL;
	SetTexture(normal);
	m_pButtonTextures[BUTTON_NORMAL] = m_pTexture;
	return true;
}

void CButton::SetPosition(float fx, float fy, float fw, float fh, float z)
{
	CGUIObject::SetPosition(fx,fy,fw,fh,z);

	for(int i=0; i<4; i++)
	{
		if(m_pButtonTextures[i])
		{
			m_pButtonTextures[i]->SetPosition(fx,fy,z);
			m_pButtonTextures[i]->SetDimensions(fw,fh);

		}
	}

	m_fHotx = fx;
	m_fHoty = fy;
	m_fHotw = fw;
	m_fHoth = fh;
	m_rHotSpot = GetScreenRect();
}

bool InRect(int x, int y, const RECT &r);

extern int g_mouseButtons;
extern POINT g_mousePoint;

bool CButton::Update(float fSecsElapsed, CUserInterface *pParent)
{

	// call the base class to preserve base functionality
	if(!CGUIObject::Update(fSecsElapsed, pParent))
		return false;

	bool retVal = true;

	eButtonState oldState = m_buttonState;

	if(m_buttonState != BUTTON_DISABLED)
	{
		// check mouse position and set state and base texture pointer
		RECT r = m_rHotSpot; //GetScreenRect();
		
		m_buttonState = BUTTON_NORMAL;
		if(InRect(g_mousePoint.x, g_mousePoint.y, r))
		{
			m_buttonState = BUTTON_HOVER;
			if(g_mouseButtons & MOUSE_LBUTTON_DOWN)
			{
				m_buttonState = BUTTON_SELECTED;
			}
		}

	}


	

	m_pTexture = m_pButtonTextures[BUTTON_NORMAL];
	if(m_pButtonTextures[m_buttonState])
		m_pTexture = m_pButtonTextures[m_buttonState];

	if(oldState != m_buttonState)
	{
		if(oldState == BUTTON_SELECTED)
		{
			// button was selected last time, now is not
			CGUIManager::GetInstance()->SendEvent(GUI_EVENT_BUTTON_UP, m_ID);
			retVal = false;
		}

		if(m_buttonState == BUTTON_SELECTED)
		{
			// was unselected and now is selected
			CGUIManager::GetInstance()->SendEvent(GUI_EVENT_BUTTON_DOWN, m_ID);
			retVal = false;
		}

		if(oldState == BUTTON_HOVER)
		{
			CGUIManager::GetInstance()->SendEvent(GUI_EVENT_HOVER_END, m_ID);
		}


		if(m_buttonState == BUTTON_HOVER)
		{
			m_fButtonHoverCountdown = CGUIManager::GetInstance()->GetButtonHoverCountdownTime();
		}

	}
	else
	{
		// old state = current state
		if((m_fButtonHoverCountdown > 0.0f) && (oldState == BUTTON_HOVER))
		{
			m_fButtonHoverCountdown -= fSecsElapsed;
			if(m_fButtonHoverCountdown <= 0.0f)
			{
				CGUIManager::GetInstance()->SendEvent(GUI_EVENT_HOVER_TIMED_START, m_ID);
			}

		}
	}

	return retVal;
}


bool CButton::StealsMouse(int x, int y)
{
	bool bRet = false;

	if (!isDisabled())
	{
		if(InRect(x, y, m_rHotSpot))
			bRet = true;
		else
			bRet = CGUIObject::StealsMouse(x, y);
	}
	return bRet;
}


int CButton::ObjectCommand(const char *pCommand)
{
	cLua	*L = CGUIManager::GetInstance()->GetLuaContext();
	int retVal = 0;
	int stackIndex = 3;

	int sw=800,sh=600;
//	g_pGUIBase->GetScreenDimensions(sw,sh);

	if(strcmp(pCommand, "HotSpot") == 0)
	{
		m_fHotx = (float) L->GetNumberArgument(stackIndex++) / 600.0f; //CGUIManager::GetInstance()->GetCurrentUI()->GetCoordTransWidth();
		m_fHoty = (float) L->GetNumberArgument(stackIndex++) / 800.0f; //CGUIManager::GetInstance()->GetCurrentUI()->GetCoordTransHeight();
		m_fHotw = (float) L->GetNumberArgument(stackIndex++) / 600.0f; //CGUIManager::GetInstance()->GetCurrentUI()->GetCoordTransWidth();
		m_fHoth = (float) L->GetNumberArgument(stackIndex++) / 800.0f; //CGUIManager::GetInstance()->GetCurrentUI()->GetCoordTransHeight();

		m_rHotSpot.left		= (int) (m_fHotx * sw);
		m_rHotSpot.right	= (int) ((m_fHotx * sw) + (m_fHotw * sw));
		m_rHotSpot.top		= (int) (m_fHoty * sh);
		m_rHotSpot.bottom	= (int) ((m_fHoty * sh) + (m_fHoth * sh));
	}

	if(strcmp(pCommand, "Disable") == 0)
	{
		m_buttonState = BUTTON_DISABLED;

		m_pTexture = m_pButtonTextures[BUTTON_NORMAL];
		if(m_pButtonTextures[m_buttonState])
			m_pTexture = m_pButtonTextures[m_buttonState];
	}

	if(strcmp(pCommand, "Enable") == 0)
	{
		m_buttonState = BUTTON_NORMAL;
		m_pTexture = m_pButtonTextures[BUTTON_NORMAL];
	}


	return retVal;
}

