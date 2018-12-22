// TextField.cpp: implementation of the CTextField class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GUIManager.h"
#include "GUITextField.h"

const char *kpTextFieldName = "TextField";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextField::CTextField()
{
	m_color = D3DCOLOR_ARGB(255, 255,255,255);
	m_bKeyboardFocus = false;
	m_bIsEditable = false;

	m_bMouseDownLastTime = false;

	 
	m_fSavedScrollSpeed	=
		m_fScrollSpeed = 0.0f;		// in Chars per Sec
	m_fCurrentPosition	= 0.0f;
	m_iStringPixels = 0;
}

CTextField::~CTextField()
{

}

bool InRect(int x, int y, const RECT &r);

bool CTextField::StealsMouse(int x, int y) 
{
	// this is a comment
	if (isDisabled() || !m_bIsEditable)
		return false;
	else
		return InRect(x, y, GetScreenRect());
}

int CTextField::ObjectCommand(const char *pCommand)
{
	cLua	*L = CGUIManager::GetInstance()->GetLuaContext();
	int retVal = 0;

	if(strcmp(pCommand, "SetString") == 0)
	{
		// process "SetString" command
		m_string = L->GetStringArgument(3);
		m_iStringPixels = 0;
		if(m_pFont)
		{
			m_pFont->GetTextExtent((char *) m_string.c_str(), &m_iStringPixels);
		}
	}

	if(strcmp(pCommand, "AddString") == 0)
	{
		std::string aStr = L->GetStringArgument(3);
		// this ADDS a string to the scrolling list
		if(m_iStringPixels == 0)
		{
			m_string = aStr;
			m_iStringPixels = 0;
			if(m_pFont)
			{
				m_pFont->GetTextExtent((char *) m_string.c_str(), &m_iStringPixels);
			}
		}
		else
			m_lstStrings.push_back(aStr);

		m_fScrollSpeed = m_fSavedScrollSpeed;
	}

	if(strcmp(pCommand, "GetString") == 0)
	{
		// process "GetString" command
		L->PushString(m_string.c_str());
		retVal = 1;
	}

	if(strcmp(pCommand, "SetColor") == 0)
	{
		// process "SetColor" command
		int a,r,g,b;

		r = (int) L->GetNumberArgument(3);
		g = (int) L->GetNumberArgument(4);
		b = (int) L->GetNumberArgument(5);
		a = (int) L->GetNumberArgument(6, 255);
		m_color = D3DCOLOR_ARGB(a, r, g, b);
	}

	if(strcmp(pCommand, "Focus") == 0)
	{
		// process "SetString" command
		m_bKeyboardFocus = L->GetNumberArgument(3) != 0;
	}

	if(strcmp(pCommand, "SetEditability") == 0)
	{
		// process "SetString" command
		m_bIsEditable = L->GetNumberArgument(3) != 0;
	}

	return retVal;
}

bool CTextField::Render(void)
{
	// call the base class in case there is a texture loaded
	CGUIObject::Render(); 
	if(m_pFont)
	{
		if(m_iStringPixels == 0)
		{
			m_pFont->GetTextExtent((char *) m_string.c_str(), &m_iStringPixels, NULL);
		}

		std::string orig = m_string;

		if(m_bKeyboardFocus)
		{
			// check to see if we need a cursor at the end
			if((timeGetTime() / 100) & 1)
				orig += "|";
		}

	 	m_pFont->DrawText((char *) orig.c_str(), m_color, m_rScreen.left, m_rScreen.top, m_rScreen.right, m_rScreen.bottom);

	}
	return true;
}

bool CTextField::KeyHit(int ascii)
{
	if(m_bKeyboardFocus)
	{
		switch(ascii)
		{
		case VK_BACK:
			{
				if(m_string.size())
				{
					char *str = new char[m_string.size()+1];
					strcpy(str, m_string.c_str());
					str[strlen(str)-1] = 0;
					m_string = str;
					delete[] str;
				}
			}
			break; 
		case VK_CLEAR:
			m_string = "";
			break;
		case VK_RETURN:
			CGUIManager::GetInstance()->SendEvent(GUI_EVENT_TEXTFIELD_RETURN, m_ID);
			break;
		default:
			{
				unsigned char in = (unsigned char) ascii;
				if(isprint(in))
				{
					// add char to string
					m_string += in;
					CGUIManager::GetInstance()->SendEvent(GUI_KEY_PRESS, in, (float) m_ID);
				}
				else
					return false;
			}
			break;
		}
		return true;
	}

	return false;
}

bool CTextField::Update(float fSecsElapsed, CUserInterface *pParent) 
{
	// call the parent class...
	CGUIObject::Update(fSecsElapsed, pParent);

#if 0
	// check of mouse clicked on the text field area and set event
	int x, y, buttons;
	g_pGUIBase->MouseStatus(x, y, buttons);


	if(buttons & MOUSE_LBUTTON_DOWN)
	{
		RECT r = GetScreenRect();
		
		if(InRect(x,y,r))
		{
			m_bMouseDownLastTime = true;
		}
		else
			m_bMouseDownLastTime = false;

	}
	else
	{
		if(m_bMouseDownLastTime)
		{
			// check mouse position and set state
			RECT r = GetScreenRect();
			
			if(InRect(x,y,r))
			{
				CGUIManager::GetInstance()->SendEvent(GUI_EVENT_TEXTFIELD_CLICKED, m_ID);
			}
		}

		m_bMouseDownLastTime = false;
	}

#endif
	return true;
}
