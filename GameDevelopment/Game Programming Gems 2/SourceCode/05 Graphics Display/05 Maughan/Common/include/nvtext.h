/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
	Brought across from the DX8 SDK framework to use the NVSTATEMANAGER.
    

  - cmaughan@nvidia.com
        
******************************************************************************/
#ifndef __NVTEXT_H
#define __NVTEXT_H

#include "nvdebug.h"
#include "nvstatemanager.h"

namespace nv_objects
{

#define MAX_FONT_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

static inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

static inline FONT3DVERTEX InitFont3DVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                                      FLOAT tu, FLOAT tv )
{
    FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
    return v;
}

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002

// Font rendering flags
#define D3DFONT_CENTERED    0x0001
#define D3DFONT_TWOSIDED    0x0002
#define D3DFONT_FILTERED    0x0004

class NVText
{
public:
	NVText(LPDIRECT3DDEVICE8 pD3DDev)
		: m_pD3DDev(pD3DDev),
		m_pFontTexture(NULL),
		m_pFontVB(NULL)
	{
		NVASSERT(m_pD3DDev, "No valid device!");
		m_pD3DDev->AddRef();
	}

	~NVText()
	{
		SAFE_RELEASE(m_pFontVB);
		SAFE_RELEASE(m_pFontTexture);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	bool InitDeviceObjects()
	{
		// Create vertex buffer for the characters
		HRESULT hr = m_pD3DDev->CreateVertexBuffer( MAX_FONT_VERTICES*sizeof(FONT2DVERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                       D3DPOOL_DEFAULT, &m_pFontVB);

		if (FAILED(hr))
			return false;

		return true;
	}

	bool RestoreDeviceObjects()
	{
		SAFE_RELEASE(m_pFontVB);
		InitDeviceObjects();
		
		return true;	
	}

	bool InvalidateDeviceObjects()
	{
		SAFE_RELEASE(m_pFontVB);
		
		return true;	
	}

	bool SetupTextImage(const std::string& strName, DWORD dwFontHeight, DWORD dwFontFlags)
	{
		HRESULT hr;

		m_strFontName = strName;
		m_dwFontHeight = dwFontHeight;
		m_dwFontFlags = dwFontFlags;

		if (m_pFontTexture)
			SAFE_RELEASE(m_pFontTexture);

		// Establish the font and texture size
		m_fFontTextScale  = 1.0f; // Draw fonts into texture without scaling

		// Large fonts need larger textures
		if( m_dwFontHeight > 40 )
			m_dwFontTexWidth = m_dwFontTexHeight = 1024;
		else if( m_dwFontHeight > 20 )
			m_dwFontTexWidth = m_dwFontTexHeight = 512;
		else
			m_dwFontTexWidth  = m_dwFontTexHeight = 256;

		// If requested texture is too big, use a smaller texture and smaller font,
		// and scale up when rendering.
		D3DCAPS8 d3dCaps;
		m_pD3DDev->GetDeviceCaps( &d3dCaps );

		if( m_dwFontTexWidth > d3dCaps.MaxTextureWidth )
		{
			m_fFontTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)m_dwFontTexWidth;
			m_dwFontTexWidth = m_dwFontTexHeight = d3dCaps.MaxTextureWidth;
		}

		// Create a new texture for the font
		hr = m_pD3DDev->CreateTexture( m_dwFontTexWidth, m_dwFontTexHeight, 1,
										  0, D3DFMT_A4R4G4B4,
										  D3DPOOL_MANAGED, &m_pFontTexture );
		if( FAILED(hr) )
			return false;

		// Prepare to create a bitmap
		DWORD*      pBitmapBits;
		BITMAPINFO bmi;
		ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       =  (int)m_dwFontTexWidth;
		bmi.bmiHeader.biHeight      = -(int)m_dwFontTexHeight;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount    = 32;

		// Create a DC and a bitmap for the font
		HDC     hDC       = CreateCompatibleDC( NULL );
		HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
											  (VOID**)&pBitmapBits, NULL, 0 );
		SetMapMode( hDC, MM_TEXT );

		// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
		// antialiased font, but this is not guaranteed.
		INT nHeight    = -MulDiv( m_dwFontHeight, 
			(INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fFontTextScale), 72 );
		DWORD dwBold   = (m_dwFontFlags & D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
		DWORD dwItalic = (m_dwFontFlags & D3DFONT_ITALIC) ? TRUE    : FALSE;
		HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
							  FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							  VARIABLE_PITCH, m_strFontName.c_str());
		if( NULL==hFont )
			return false;

		SelectObject( hDC, hbmBitmap );
		SelectObject( hDC, hFont );

		// Set text properties
		SetTextColor( hDC, RGB(255,255,255) );
		SetBkColor(   hDC, 0x00000000 );
		SetTextAlign( hDC, TA_TOP );

		// Loop through all printable character and output them to the bitmap..
		// Meanwhile, keep track of the corresponding tex coords for each character.
		DWORD x = 0;
		DWORD y = 0;
		TCHAR str[2] = _T("x");
		SIZE size;

		for( TCHAR c=32; c<127; c++ )
		{
			str[0] = c;
			GetTextExtentPoint32( hDC, str, 1, &size );

			if( (DWORD)(x+size.cx+1) > m_dwFontTexWidth )
			{
				x  = 0;
				y += size.cy+1;
			}

			ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

			m_fFontTexCoords[c-32][0] = ((FLOAT)(x+0))/m_dwFontTexWidth;
			m_fFontTexCoords[c-32][1] = ((FLOAT)(y+0))/m_dwFontTexHeight;
			m_fFontTexCoords[c-32][2] = ((FLOAT)(x+0+size.cx))/m_dwFontTexWidth;
			m_fFontTexCoords[c-32][3] = ((FLOAT)(y+0+size.cy))/m_dwFontTexHeight;

			x += size.cx+1;
		}

		// Lock the surface and write the alpha values for the set pixels
		D3DLOCKED_RECT d3dlr;
		m_pFontTexture->LockRect( 0, &d3dlr, 0, 0 );
		WORD* pDst16 = (WORD*)d3dlr.pBits;
		BYTE bAlpha; // 4-bit measure of pixel intensity

		for( y=0; y < m_dwFontTexHeight; y++ )
		{
			for( x=0; x < m_dwFontTexWidth; x++ )
			{
				bAlpha = (BYTE)((pBitmapBits[m_dwFontTexWidth*y + x] & 0xff) >> 4);
				if (bAlpha > 0)
				{
					*pDst16++ = (bAlpha << 12) | 0x0fff;
				}
				else
				{
					*pDst16++ = 0x0000;
				}
			}
		}

		// Done updating texture, so clean up used objects
		m_pFontTexture->UnlockRect(0);
		DeleteObject( hbmBitmap );
		DeleteDC( hDC );
		DeleteObject( hFont );

		return true;
	}

	bool DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, TCHAR* strText, DWORD dwFlags)
	{
		HRESULT hr = S_OK;
		if (!m_pFontVB || !m_pFontTexture)
			return false;

		NVSTATEMANAGER.SaveVertexShader(D3DFVF_FONT2DVERTEX);
		NVSTATEMANAGER.SavePixelShader(0);
		NVSTATEMANAGER.SaveStreamSource(0, m_pFontVB, sizeof(FONT2DVERTEX));
		NVSTATEMANAGER.SaveTexture(0, m_pFontTexture);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x80);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		NVSTATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		NVSTATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		NVSTATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, FALSE);
		NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		NVSTATEMANAGER.SaveRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		NVSTATEMANAGER.SaveRenderState(D3DRS_WRAP0, 0);

		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MINFILTER, dwFlags & D3DFONT_FILTERED ? D3DTEXF_LINEAR : D3DTEXF_POINT);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MAGFILTER, dwFlags & D3DFONT_FILTERED ? D3DTEXF_LINEAR : D3DTEXF_POINT);

		FLOAT fStartX = sx;

		// Fill vertex buffer
		FONT2DVERTEX* pVertices = NULL;
		DWORD         dwNumTriangles = 0;
		m_pFontVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );

		while( *strText )
		{
			TCHAR c = *strText++;

			if( c == _T('\n') )
			{
				sx = fStartX;
				sy += (m_fFontTexCoords[0][3]-m_fFontTexCoords[0][1])*m_dwFontTexHeight;
			}
			if( c < _T(' ') )
				continue;

			FLOAT tx1 = m_fFontTexCoords[c-32][0];
			FLOAT ty1 = m_fFontTexCoords[c-32][1];
			FLOAT tx2 = m_fFontTexCoords[c-32][2];
			FLOAT ty2 = m_fFontTexCoords[c-32][3];

			FLOAT w = (tx2-tx1) *  m_dwFontTexWidth / m_fFontTextScale;
			FLOAT h = (ty2-ty1) * m_dwFontTexHeight / m_fFontTextScale;

			*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
			*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
			*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
			*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
			*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
			*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
			dwNumTriangles += 2;

			if( dwNumTriangles*3 > (MAX_FONT_VERTICES-6) )
			{
				// Unlock, render, and relock the vertex buffer
				m_pFontVB->Unlock();
				NVSTATEMANAGER.DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
				pVertices = NULL;
				m_pFontVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
				dwNumTriangles = 0L;
			}

			sx += w;
		}

		// Unlock and render the vertex buffer
		m_pFontVB->Unlock();
		if( dwNumTriangles > 0 )
		{
			NVSTATEMANAGER.DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
		}

		NVSTATEMANAGER.RestoreVertexShader();
		NVSTATEMANAGER.RestorePixelShader();
		NVSTATEMANAGER.RestoreStreamSource(0);
		NVSTATEMANAGER.RestoreTexture(0);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_FILLMODE);
		NVSTATEMANAGER.RestoreRenderState(D3DRS_WRAP0);

		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG1);
		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
		NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);

		return true;
	}

	DWORD GetFontHeight() const { return m_dwFontHeight; }

private:
	LPDIRECT3DDEVICE8 m_pD3DDev;

	// Font containing alphabet
    LPDIRECT3DTEXTURE8 m_pFontTexture; 

	// Dynamic VB for drawing text
    LPDIRECT3DVERTEXBUFFER8 m_pFontVB; 

	// Font properties
	std::string m_strFontName;
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;
    DWORD   m_dwFontTexWidth;
    DWORD   m_dwFontTexHeight;
    FLOAT   m_fFontTextScale;
    FLOAT   m_fFontTexCoords[128-32][4];
};

}; // namespace nv_objects

#endif // __NVTEXT_H
