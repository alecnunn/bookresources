/***************************************************************
* TechniqueApplication.cpp                                     *
*                                                              *
* This file contains the implementation of the                 *
* TechniqueApplication class.    	        				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "TechniqueApplication.h"

#define D3DFVF_SPRITEVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

struct SPRITE_VERTEX
{
	float x, y, z;
	float u, v;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pSpriteVertexBuffer     = NULL;
	m_pSpriteTexture          = NULL;
	m_pBackTexture            = NULL;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						          D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	//Set the basics
	SetupDevice();
	
	if (FAILED(CreateSpriteBuffer()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\crosshair.dds", 
										 &m_pSpriteTexture)))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\tile.dds", 
										 &m_pBackTexture)))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{		
	//Set the projection matrix for proportion based sprites
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProportionalProjection);

	//Set the background texture.
	m_pD3DDevice->SetTexture(0, m_pBackTexture);

	//Turn off blending when drawing the background
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   FALSE);

	//Draw the sprite the "easy" way
	DrawSpriteEx(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//Set the projection matrix for pixel based sprites
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_PixelProjection);

	//Set the texture.
	m_pD3DDevice->SetTexture(0, m_pSpriteTexture);

	//Enable alpha blending
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Draw the first sprite the long way
	TranslateSprite(200.0f, 0.0f, 0.0f);
	RotateSprite((float)GetTickCount() / 1000.0f);
	ScaleSprite(100.0f, 200.0f);
	DrawSprite();

	//Draw the second sprite the "easy" way
	DrawSpriteEx(-100.0f, 0.0f, 0.0f, 0.0f, 100.0f, 100.0f);
}

BOOL CTechniqueApplication::PostReset()
{
	SetupDevice();

	return TRUE;
}


BOOL CTechniqueApplication::PreTerminate()
{
	//Clean up
	if (m_pSpriteVertexBuffer)
	{
		m_pSpriteVertexBuffer->Release();
		m_pSpriteVertexBuffer = NULL;
	}

	if (m_pSpriteTexture)
	{
		m_pSpriteTexture->Release();
		m_pSpriteTexture = NULL;
	}

	if (m_pBackTexture)
	{
		m_pBackTexture->Release();
		m_pBackTexture = NULL;
	}

	return TRUE;
}


HRESULT CTechniqueApplication::SetupDevice()
{
	//Create a pixel based ortho matrix and a percentage based matrix
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
	D3DXMatrixOrthoLH(&m_PixelProjection, 
		             (float)WindowRect.right, 
					 (float)WindowRect.bottom, 
					 0.0f, 100.0f);

	D3DXMatrixOrthoLH(&m_ProportionalProjection, 
		             (float)1.0f, 
					 (float)1.0f, 
					 0.0f, 100.0f);

	//Initialize everything
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_SpriteTranslation);
	D3DXMatrixIdentity(&m_SpriteRotation);
	D3DXMatrixIdentity(&m_SpriteScaling);

	//Set the default view transform
	m_pD3DDevice->SetTransform(D3DTS_VIEW,  &m_ViewMatrix);

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CTechniqueApplication::CreateSpriteBuffer()
{
	//Also, create the mask vertex buffer
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(SPRITE_VERTEX), 
		                                        0, D3DFVF_SPRITEVERTEX,
									            D3DPOOL_MANAGED,
												&m_pSpriteVertexBuffer)))
		return E_FAIL;

	//Create a set of 4 vertices for the texture viewing surface
	SPRITE_VERTEX *pSpriteVertices;

	//Lock the vertex buffer, but allow writing.
	m_pSpriteVertexBuffer->Lock(0, 4 * sizeof(SPRITE_VERTEX), (BYTE **)&pSpriteVertices, 0);

	//Set up the 4 corners of a small square
	pSpriteVertices[0].x = -0.5f;   pSpriteVertices[0].y =  -0.5f;
	pSpriteVertices[0].z =  1.0f;
	pSpriteVertices[0].u =  0.0f;   pSpriteVertices[0].v =   1.0f;

	pSpriteVertices[1].x = -0.5f;   pSpriteVertices[1].y =   0.5f;
	pSpriteVertices[1].z =  1.0f;
	pSpriteVertices[1].u =  0.0f;   pSpriteVertices[1].v =   0.0f;

	pSpriteVertices[2].x =  0.5f;   pSpriteVertices[2].y =  -0.5f;
	pSpriteVertices[2].z =  1.0f;
	pSpriteVertices[2].u =  1.0f;   pSpriteVertices[2].v =   1.0f;

	pSpriteVertices[3].x =  0.5f;   pSpriteVertices[3].y =   0.5f;
	pSpriteVertices[3].z =  1.0f;
	pSpriteVertices[3].u =  1.0f;   pSpriteVertices[3].v =   0.0f;
	m_pSpriteVertexBuffer->Unlock();

	//Set the defaults
	m_pD3DDevice->SetVertexShader(D3DFVF_SPRITEVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pSpriteVertexBuffer, sizeof(SPRITE_VERTEX));

	return S_OK;
}

void CTechniqueApplication::TranslateSprite(float X, float Y, float Z)
{
	D3DXMatrixTranslation(&m_SpriteTranslation, X, Y, Z);
}

void CTechniqueApplication::RotateSprite(float Angle)
{
	D3DXMatrixRotationZ(&m_SpriteRotation, Angle);
}

void CTechniqueApplication::ScaleSprite(float X, float Y)
{
	D3DXMatrixScaling(&m_SpriteScaling, X, Y, 1.0f);
}

void CTechniqueApplication::DrawSprite()
{
	//Do the movement
	m_pD3DDevice->SetTransform(D3DTS_WORLD, 
		         &(m_SpriteScaling * m_SpriteRotation * m_SpriteTranslation));

	//Draw a rectangle showing the rendering
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);	
}

void CTechniqueApplication::DrawSpriteEx(float X, float Y, float Z,
										 float Angle, float XScale, float YScale)
{
	TranslateSprite(X, Y, Z);
	RotateSprite(Angle);
	ScaleSprite(XScale, YScale);

	SPRITE_VERTEX *pSpriteVertices;
	m_pSpriteVertexBuffer->Lock(0, 4 * sizeof(SPRITE_VERTEX), (BYTE **)&pSpriteVertices, 0);
	m_pSpriteVertexBuffer->Unlock();

	DrawSprite();
}
