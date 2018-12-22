// GUIObject.cpp: implementation of the CGUIObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GUIObject.h"

#include "GUIObject.h"
#include "GUIManager.h"
#include "UserInterface.h"

const char *kpBaseName   = "BASE";


std::string CGUIObject::m_sTexturePath = "";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIObject::CGUIObject()
{
	m_pTexture = NULL;
	m_pFont = NULL;

	m_isDisabled = false;
	m_bDrawWhenDisabled = false;

	m_fNormWidth = 1.0f;
	m_fNormHeight = 1.0f;

}

CGUIObject::~CGUIObject()
{
	SAFE_DELETE(m_pTexture);
	SAFE_DELETE(m_pFont);
}


bool CGUIObject::SetTexture(char *pTextureName, bool bSetPos)
{
	if(!pTextureName)
		return true;

	float rotx=0.0f, roty=0.0f, rot=0.0f;
	float oldx=0.0f, oldy=0.0f, oldz=0.0f, oldw=0.0f, oldh=0.0f;

	if(m_pTexture)
	{
		m_pTexture->GetRotation(rotx, roty, rot);
		m_pTexture->GetPosition(oldx, oldy, oldz);
		m_pTexture->GetDimensions(oldw, oldh);
		
		SAFE_DELETE(m_pTexture);
	}
	m_pTexture = new Sprite();
	m_pTexture->SetTexture(pTextureName);
	if(bSetPos)
	{
		m_pTexture->SetPosition(0, 0, 0);

		int w, h;
		DXBase::GetInstance()->GetScreenDimensions(w, h);
		m_pTexture->SetDimensions((float) w, (float) h);
	}
	else
	{
		// reset the location and dims
		float w, h;
		w = (float) m_rScreen.right - m_rScreen.left;
		h = (float) m_rScreen.bottom - m_rScreen.top;

		m_pTexture->SetPosition(oldx, oldy, oldz);
		m_pTexture->SetDimensions(oldw, oldh);
		m_pTexture->SetRotation(rotx, roty, rot);
	}

	return true;
}


void CGUIObject::SetPosition(float fx, float fy, float fw, float fh, float z)
{
#if 0
	int32 w,h;
	g_pGUIBase->GetScreenDimensions(w,h);

	if(m_pTexture)
	{
		int tw, th;
		m_pTexture->GetTexture()->GetTexture(0)->DimensionsGet(tw, th);
		if(fw < 0)	// get for pixel size translation flag
		{
			fw = (float)tw / (-fw);
		}

		if(fh < 0)	// get for pixel size translation flag
		{
			fh = (float)th / (-fh);
		}

		m_pTexture->PositionSet(Vector3d(fx*w,fy*h,z));
		m_pTexture->DimensionsSet(fw*w,fh*h);
	}

	// set screen rect
	m_rScreen.top		= (long) ((fy*h)+0.49f);
	m_rScreen.left		= (long) (fx*w);
	m_rScreen.bottom	= m_rScreen.top  + (long) (fh*h);
	m_rScreen.right		= m_rScreen.left + (long) (fw*w);

	m_vNormPosition = Vector3d(fx,fy,z);
	m_fNormWidth = fw;
	m_fNormHeight = fh;
#endif

	if(m_pTexture)
	{
		m_pTexture->SetPosition(fx, fy, z);
		m_pTexture->SetDimensions(fw, fh);
	}

	// set screen rect
	m_rScreen.top		= (long) (fy+0.49f);
	m_rScreen.left		= (long) (fx);
	m_rScreen.bottom	= m_rScreen.top  + (long) (fh);
	m_rScreen.right		= m_rScreen.left + (long) (fw);

}

bool CGUIObject::Render(void)
{
	if(m_pTexture)
		m_pTexture->Render();
	return true;
}


bool CGUIObject::SetFont(char *pFontName, float normalSize)
{
	// replace anything that was there
	SAFE_DELETE(m_pFont);

	m_sFontName = pFontName;
	m_fontNormalSize = normalSize;

	m_pFont = new Font;
	if(m_pFont->Create((int) normalSize, pFontName))
	{
		return true;
	}

#if 0
	IGraphicsRenderer *pRenderer = GraphicsRendererFactory::GetInstance();
	if(pRenderer->CreateFont(pFontName, m_pFont, normalSize))
	{
		return true;
	}
#endif
	return false;
}

void CGUIObject::ResChangeNotify(void)
{
#if 0
	int32 w,h;
	g_pGUIBase->GetScreenDimensions(w,h);

	if(m_pTexture)
	{
		m_pTexture->PositionSet(Vector3d(m_vNormPosition.x*w,m_vNormPosition.y*h,m_vNormPosition.z));
		m_pTexture->DimensionsSet(m_fNormWidth*w,m_fNormHeight*h);
	}

	// set screen rect
	m_rScreen.top		= (long) (m_vNormPosition.y*h);
	m_rScreen.left		= (long) (m_vNormPosition.x*w);
	m_rScreen.bottom	= m_rScreen.top  + (long) (m_fNormHeight*h);
	m_rScreen.right		= m_rScreen.left + (long) (m_fNormWidth*w);

	if(m_pFont)
	{
		SetFont((char *)m_sFontName.c_str(), m_fontNormalSize);
	}
#endif
}

bool InRect(int x, int y, const RECT &r);

bool CGUIObject::StealsMouse(int x, int y)
{
	if (isDisabled())
		return false;
	else
		return InRect(x, y, GetScreenRect());
}


#if 0
void CGUIObject::SetTextureUV(Vector2d  *puv)
{
	if(m_pTexture)
	{
		m_pTexture->UVSet(puv);
	}
}
#endif

