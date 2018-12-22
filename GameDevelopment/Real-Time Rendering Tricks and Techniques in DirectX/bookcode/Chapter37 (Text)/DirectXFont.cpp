/***************************************************************
* DirectXFont.cpp                                              *
*                                                              *
* This file contains the implementation of the                 *
* CDirectXFont class.      									   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "DirectXFont.h"

#define MAX_TEXT_LENGTH 100
#define MAX_TEXT_VERTICES MAX_TEXT_LENGTH * 6

CDirectXFont::CDirectXFont()
{
	m_TextureSize   = 0;
	m_pTexture      = NULL;
	m_pVertexBuffer = NULL;
}

CDirectXFont::~CDirectXFont()
{
}

HRESULT CDirectXFont::CreateFont(LPDIRECT3DDEVICE8 pDevice, 
								 TCHAR *pFontName, long FontSize)
{
	//Keep a copy of the device for later...
	m_pDevice = pDevice;

	//Create a DC for GDI instructions. This is only used as a target
	//for GDI calls within this function.
	HDC hDC = CreateCompatibleDC(NULL);

	//Setup the DC for text drawing
	SetTextColor(hDC, RGB(255,255,255));
	SetBkColor(hDC, 0x00000000);
	SetTextAlign(hDC, TA_TOP);
	SetMapMode(hDC, MM_TEXT);

	//Create the actual font
	INT FontHeight = MulDiv(FontSize, (INT)GetDeviceCaps(hDC, LOGPIXELSY), 72);

	HFONT hFont = ::CreateFont(FontHeight, 0, 0, 0, 0, 0, FALSE, FALSE,
							   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
							   CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							   VARIABLE_PITCH, pFontName);
	if(NULL == hFont)
		return E_FAIL;

	//Select the font into the DC
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	//Figure out how big the bitmap and texture should be to hold the font.
	if (FontSize > 40)
		m_TextureSize = 1024;
	else if (FontSize > 20)
		m_TextureSize = 512;
	else
		m_TextureSize = 256;


	//First, create a bitmap of the characters using GDI calls. The 
	//height is negated because of the way bitmaps are encoded.
	DWORD     *pBitmapBits;
	BITMAPINFO BitmapInfo;
	ZeroMemory(&BitmapInfo.bmiHeader,    sizeof(BITMAPINFOHEADER));
	BitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth       =  (int)m_TextureSize;
	BitmapInfo.bmiHeader.biHeight      = -(int)m_TextureSize;
	BitmapInfo.bmiHeader.biPlanes      = 1;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	BitmapInfo.bmiHeader.biBitCount    = 32;

	HBITMAP hBitmap = CreateDIBSection(hDC, &BitmapInfo,
									   DIB_RGB_COLORS,
									   (VOID**)&pBitmapBits,
									   NULL, 0);

	//Select the new bitmap into the DC.
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);

	//Now, loop through all the printable ASCII characters (32 - 127) and 
	//draw them to the bitmap. As they are being drawn, record their
	//positions in our array of texture coordinates.
	long XPos = 0;
	long YPos = 0;
	SIZE CharExtent;
	long CharIndex;

	for(char CurrentChar = 32; CurrentChar < 127; CurrentChar++)
	{
		//Get the character size
		GetTextExtentPoint32(hDC, (LPCTSTR)&CurrentChar, 1, &CharExtent);

		//Move to the next line if need be.
		if(XPos + CharExtent.cx + 1 > m_TextureSize)
		{
			XPos  = 0;
			YPos += FontHeight + 1;
		}

		//Draw the character
		ExtTextOut(hDC, XPos, YPos, ETO_OPAQUE, NULL, 
				   (LPCTSTR)&CurrentChar, 1, NULL);

		CharIndex = CurrentChar - 32;

		//Record the texture coordinates
		m_TexCoords[CharIndex][0] = (float)XPos / (float)m_TextureSize;
		m_TexCoords[CharIndex][1] = (float)YPos / (float)m_TextureSize;
		m_TexCoords[CharIndex][2] = (float)(XPos + CharExtent.cx) / 
									(float)m_TextureSize;
		m_TexCoords[CharIndex][3] = (float)(YPos + CharExtent.cy) / 
									(float)m_TextureSize;

		//Update the x position, but make sure there's enough space so
		//that characters don't overlap.
		XPos += CharExtent.cx + 10;
	}

	//Now, create the actual texture
	if (FAILED(pDevice->CreateTexture(m_TextureSize, m_TextureSize, 1,
									  0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
									  &m_pTexture)))
		return E_FAIL;

	//Lock the entire texture surface. Since the texture was rendered in black
	//and white, we can use the color values of the bitmap to set the alpha
	//value of the texture.
	D3DLOCKED_RECT LockedRect;
	m_pTexture->LockRect(0, &LockedRect, 0, 0);

	for (long Index = 0; Index < m_TextureSize * m_TextureSize * 4; Index += 4)
	{
		BYTE Value = (BYTE)*((BYTE *)pBitmapBits + Index);
		*((BYTE *)LockedRect.pBits + Index + 0) = Value;
		*((BYTE *)LockedRect.pBits + Index + 1) = Value;
		*((BYTE *)LockedRect.pBits + Index + 2) = Value;
		*((BYTE *)LockedRect.pBits + Index + 3) = Value;
	}

	m_pTexture->UnlockRect(0);

	//Clean up all the GDI objects. This is the end of our GDI usage
	SelectObject(hDC, hOldBitmap);
	DeleteObject(hBitmap);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	DeleteDC(hDC);

	//Create the vertex buffer to hold all the vertices.
	if(FAILED(pDevice->CreateVertexBuffer(MAX_TEXT_VERTICES * sizeof(FONTVERTEX),
										  D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										  D3DFVF_FONTVERTEX, D3DPOOL_DEFAULT,
										  &m_pVertexBuffer)))
		return E_FAIL;

	//Create state blocks to change the render states
	for(long Block = 0; Block < 2; Block++)
	{
		m_pDevice->BeginStateBlock();
		m_pDevice->SetTexture(0, m_pTexture);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,  TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF,         0x10);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL);
		m_pDevice->SetRenderState(D3DRS_ZENABLE,          FALSE);
		m_pDevice->SetVertexShader(D3DFVF_FONTVERTEX);
		m_pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(FONTVERTEX));

		if(Block == 0)
			m_pDevice->EndStateBlock(&m_TextStates);
		else
			m_pDevice->EndStateBlock(&m_SavedStates);
	}

	return S_OK;
}

void CDirectXFont::DrawDebug(float XPosition, float YPosition)
{
	if(m_pTexture == NULL)
		return;

	//Set up the device
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTexture(0, m_pTexture);
	m_pDevice->SetVertexShader(D3DFVF_FONTVERTEX);
	m_pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(FONTVERTEX));

	// Fill vertex buffer to draw one big textured rectangle with the
	//entire texture. Due to stretching this may not be a good indication
	//of quality.
	FONTVERTEX* pVertices = NULL;
	m_pVertexBuffer->Lock(0, 4 * sizeof(FONTVERTEX), (BYTE**)&pVertices, D3DLOCK_DISCARD);

	pVertices[0].x = XPosition;
	pVertices[0].y = YPosition;
	pVertices[0].z = 1.0f;
	pVertices[0].u = 0.0f;
	pVertices[0].v = 0.0f;

	pVertices[1].x = XPosition;
	pVertices[1].y = YPosition + 300.0f;
	pVertices[1].z = 1.0f;
	pVertices[1].u = 0.0f;
	pVertices[1].v = 1.0f;

	pVertices[2].x = XPosition + 300.0f;
	pVertices[2].y = YPosition;
	pVertices[2].z = 1.0f;
	pVertices[2].u = 1.0f;
	pVertices[2].v = 0.0f;

	pVertices[3].x = XPosition + 300.0f;
	pVertices[3].y = YPosition + 300.0f;
	pVertices[3].z = 1.0f;
	pVertices[3].u = 1.0f;
	pVertices[3].v = 1.0f;

	pVertices[0].rhw = pVertices[1].rhw = 
	pVertices[2].rhw = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color = pVertices[1].color = 
	pVertices[2].color = 
	pVertices[3].color = 0xffffffff;

	m_pVertexBuffer->Unlock();

	//Draw the sample rectangle
	m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void CDirectXFont::DrawText(float XPosition, float YPosition, TCHAR *pText, DWORD Color)
{
	if(m_pTexture == NULL)
		return;

	//Record and set the render states
	m_pDevice->CaptureStateBlock(m_SavedStates);
	m_pDevice->ApplyStateBlock(m_TextStates);

	//record the starting position
	float XStartPosition = XPosition;

	// Fill vertex buffer with two triangles for each character
	FONTVERTEX* pVertices = NULL;
	DWORD       NumTriangles = 0;
	m_pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD);

	while(*pText)
	{
		FillCharacter(*pText++, &pVertices, XStartPosition, &XPosition,
					  &YPosition, Color);

		NumTriangles += 2;

		//If we've already exceeded the number of available vertices, 
		//draw what we have so far and keep going.
		if(NumTriangles * 3 > (MAX_TEXT_VERTICES - 6))
		{
			m_pVertexBuffer->Unlock();
			m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, NumTriangles);
									 pVertices = NULL;
			m_pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD);
			NumTriangles = 0;
		}
	}

	//Unlock the buffer and draw.
	m_pVertexBuffer->Unlock();

	if(NumTriangles > 0)
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, NumTriangles);

	//Restore the states
	m_pDevice->ApplyStateBlock(m_SavedStates);
}

LPDIRECT3DVERTEXBUFFER8 CDirectXFont::CreateStaticText(float XPosition,
													   float YPosition,
													   TCHAR *pText,
													   DWORD Color)
{
	long NumChars = strlen(pText);

	LPDIRECT3DVERTEXBUFFER8 pStaticBuffer;

	//Create the vertex buffer to hold all the vertices.
	if(FAILED(m_pDevice->CreateVertexBuffer(NumChars * 6 * sizeof(FONTVERTEX),
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											D3DFVF_FONTVERTEX, D3DPOOL_DEFAULT,
											&pStaticBuffer)))
		return NULL;


	//record the starting position
	float XStartPosition = XPosition;

	// Fill vertex buffer with two triangles for each character
	FONTVERTEX* pVertices = NULL;
	pStaticBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD);

	while(*pText)
	{
		FillCharacter(*pText++, &pVertices, XStartPosition, &XPosition,
					  &YPosition, Color);
	}

	//Unlock the buffer and draw.
	pStaticBuffer->Unlock();

	//This buffer can be released in the application, but it is
	//probably better to pass it to this class to be released.
	return pStaticBuffer;
}

void CDirectXFont::DrawStaticText(LPDIRECT3DVERTEXBUFFER8 pStaticText, 
								  long StartChar, long NumChars)
{
	if(m_pTexture == NULL && pStaticText == NULL)
		return;

	//Record and set the render states
	m_pDevice->CaptureStateBlock(m_SavedStates);
	m_pDevice->ApplyStateBlock(m_TextStates);

	//Set up the device.
	m_pDevice->SetStreamSource(0, pStaticText, sizeof(FONTVERTEX));

	m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6 * StartChar,
							 NumChars * 2);

	//Restore the states
	m_pDevice->ApplyStateBlock(m_SavedStates);
}

void CDirectXFont::DestroyStaticText(LPDIRECT3DVERTEXBUFFER8 pBuffer)
{
	if (pBuffer)
	{
		pBuffer->Release();
		pBuffer = NULL;
	}
}

void CDirectXFont::DestroyFont()
{
	if (m_pDevice)
	{
		if(m_TextStates)
			m_pDevice->DeleteStateBlock(m_TextStates);
		
		if (m_SavedStates)
			m_pDevice->DeleteStateBlock(m_SavedStates);
	}
	
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

//Create a character in a static buffer or when drawing dynamically.
void CDirectXFont::FillCharacter(TCHAR Character, FONTVERTEX **ppVertices, 
								 float XStartPosition, float *pXPosition,
								 float *pYPosition, DWORD Color)
{
	if(Character == ('\n'))
	{
		//If the character was a newline, return back to the starting X position
		//and use the dimensions of the font to determine the new Y position.
		*pXPosition = XStartPosition;
		*pYPosition += (long)((m_TexCoords[0][3]-m_TexCoords[0][1]) * (float)m_TextureSize);
	}

	//If this isn't a valid character, move on...
	if(Character < (' '))
		return;

	long CharIndex = Character - 32;

	FLOAT UMin = m_TexCoords[CharIndex][0];
	FLOAT VMin = m_TexCoords[CharIndex][1];
	FLOAT UMax = m_TexCoords[CharIndex][2];
	FLOAT VMax = m_TexCoords[CharIndex][3];

	float Width  = (UMax - UMin) * (float)m_TextureSize;
	float Height = (VMax - VMin) * (float)m_TextureSize;

	//Fill the three vertices for the first triangle
	FillVertex((*ppVertices)++, *pXPosition, *pYPosition + Height,
				UMin, VMax, Color);
	FillVertex((*ppVertices)++, *pXPosition, *pYPosition, 
				UMin, VMin, Color);
	FillVertex((*ppVertices)++, *pXPosition + Width, *pYPosition, 
				UMax, VMin, Color);

	//Fill the vertices for the second triangle
	FillVertex((*ppVertices)++, *pXPosition + Width, *pYPosition, 
				UMax, VMin, Color);
	FillVertex((*ppVertices)++, *pXPosition + Width, *pYPosition + Height,
				UMax, VMax, Color);
	FillVertex((*ppVertices)++, *pXPosition , *pYPosition + Height,
				UMin, VMax, Color);

	//Update the XPosition
	*pXPosition += Width;
}

//Just a helper function...
void CDirectXFont::FillVertex(FONTVERTEX *pVertex, float x, float y, float u, 
							  float v, DWORD Color)
{
	pVertex->x     = x;
	pVertex->y     = y;
	pVertex->z     = 1.0f;
	pVertex->rhw   = 1.0f;
	pVertex->color = Color;
	pVertex->u     = u;
	pVertex->v     = v;
}

