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
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	D3DDISPLAYMODE CurrentMode;
	if (SUCCEEDED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
								                &CurrentMode)))
	{
		ZeroMemory(&m_PresentParameters, 
				   sizeof(D3DPRESENT_PARAMETERS));
		m_PresentParameters.Windowed = TRUE;
		m_PresentParameters.SwapEffect = D3DSWAPEFFECT_COPY;
		m_PresentParameters.BackBufferFormat = CurrentMode.Format;
		m_PresentParameters.EnableAutoDepthStencil = TRUE;
		m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;

		m_CreationParameters.AdapterOrdinal = 
											D3DADAPTER_DEFAULT;
		m_CreationParameters.DeviceType     = D3DDEVTYPE_HAL;
		m_CreationParameters.hFocusWindow   = m_hWnd;


		D3DCAPS8 Caps;
		m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
		if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1))
			m_CreationParameters.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING |
			                                     D3DCREATE_MULTITHREADED;
		else
			m_CreationParameters.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING |
			                                     D3DCREATE_MULTITHREADED;

		if (FAILED(CreateDevice(&m_CreationParameters, &m_PresentParameters)))
			return FALSE;
	}

	//Set the basics
	SetupDevice();
	
	if (FAILED(m_VideoTexture.CreateFromAVIFile(m_pD3DDevice,
												"..\\media\\bikerace.avi")))
		return FALSE;

	
	//the texture must be created first to get the proper dimensions
	if (FAILED(CreateSpriteBuffer()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{		
	//Set the projection matrix for percent based sprites
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_PercentProjection);


	//Set the background texture.
	m_pD3DDevice->SetTexture(0, m_VideoTexture.m_pTexture);
	m_VideoTexture.CheckForLoop();

	//Draw the sprite the "easy" way
	DrawSpriteEx(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
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

	return TRUE;
}


HRESULT CTechniqueApplication::SetupDevice()
{
	D3DXMatrixOrthoLH(&m_PercentProjection, 
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

	//NOTE!!!  this assumes that the texture has been properly initialized
	float UMax = (float)m_VideoTexture.m_VideoWidth / (float)m_VideoTexture.m_TextureWidth;
	float VMax = (float)m_VideoTexture.m_VideoHeight / (float)m_VideoTexture.m_TextureHeight;

	//Set up the 4 corners of a small square
	pSpriteVertices[0].x = -0.5f;   pSpriteVertices[0].y =  -0.5f;
	pSpriteVertices[0].z =  1.0f;
	pSpriteVertices[0].u =  0.0f;   pSpriteVertices[0].v =   0.0f;

	pSpriteVertices[1].x = -0.5f;   pSpriteVertices[1].y =   0.5f;
	pSpriteVertices[1].z =  1.0f;
	pSpriteVertices[1].u =  0.0f;   pSpriteVertices[1].v =   VMax;

	pSpriteVertices[2].x =  0.5f;   pSpriteVertices[2].y =  -0.5f;
	pSpriteVertices[2].z =  1.0f;
	pSpriteVertices[2].u =  UMax;   pSpriteVertices[2].v =   0.0f;

	pSpriteVertices[3].x =  0.5f;   pSpriteVertices[3].y =   0.5f;
	pSpriteVertices[3].z =  1.0f;
	pSpriteVertices[3].u =  UMax;   pSpriteVertices[3].v =   VMax;

	m_pSpriteVertexBuffer->Unlock();

	//Set the defaults
	m_pD3DDevice->SetVertexShader(D3DFVF_SPRITEVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pSpriteVertexBuffer, sizeof(SPRITE_VERTEX));

	return S_OK;
}

void CTechniqueApplication::DrawSpriteEx(float X, float Y, float Z,
										 float Angle, float XScale, float YScale)
{
	D3DXMatrixTranslation(&m_SpriteTranslation, X, Y, Z);
	D3DXMatrixRotationZ(&m_SpriteRotation, Angle);
	D3DXMatrixScaling(&m_SpriteScaling, XScale, YScale, 1.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, 
		         &(m_SpriteScaling * m_SpriteRotation * m_SpriteTranslation));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);	
}
