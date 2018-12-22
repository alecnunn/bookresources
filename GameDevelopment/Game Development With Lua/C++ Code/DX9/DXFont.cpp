#include "stdafx.h"

Font::Font()
{
	m_hFont = NULL;
	m_pd3dxFont = NULL;
}

Font::~Font()
{
	SAFE_RELEASE(m_pd3dxFont);
	if(m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}


bool Font::Create(int size, const char *typeface)
{
	D3DXFONT_DESC fontDesc;
	memset(&fontDesc, 0, sizeof(fontDesc));
	strcpy(fontDesc.FaceName, typeface);
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.Height = size;
	fontDesc.Weight = FW_NORMAL;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;


	//HFONT hFont = CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, typeface);
	// Create a font for statistics and help output
	HRESULT hr = D3DXCreateFontIndirect( DXBase::GetInstance()->Get3DDevice(), &fontDesc, &m_pd3dxFont );

	if( FAILED( hr ) )
		return false;

	return true;
}

bool Font::DrawText(const char *text, DWORD color, int x, int y, int x2, int y2)
{
	if(m_pd3dxFont)
	{
		RECT destRect;
		SetRect( &destRect, x, y, x2, y2 );

		m_pd3dxFont->DrawText(NULL, text, -1, &destRect, DT_NOCLIP, color );
	}
	return true;
}

