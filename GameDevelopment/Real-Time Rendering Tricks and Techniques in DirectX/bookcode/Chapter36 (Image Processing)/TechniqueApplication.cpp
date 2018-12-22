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

#define D3DFVF_SPRITEVERTEX (D3DFVF_XYZ | D3DFVF_TEX3)

struct SPRITE_VERTEX
{
	float x, y, z;
	float u1, v1;
	float u2, v2;
	float u3, v3;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pSpriteVertexBuffer     = NULL;
	m_pColorCurve             = NULL;
	m_ImageShader             = 0;
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
		m_CreationParameters.hFocusWindow   = m_hWnd;


		D3DCAPS8 Caps;
		m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
		if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1) &&
			Caps.PixelShaderVersion == D3DPS_VERSION(1,4))
		{
			m_CreationParameters.DeviceType     = D3DDEVTYPE_HAL;
			m_CreationParameters.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING |
			                                     D3DCREATE_MULTITHREADED;
		}
		else
		{
			m_CreationParameters.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING |
			                                     D3DCREATE_MULTITHREADED;
			m_CreationParameters.DeviceType     = D3DDEVTYPE_REF;
		}

		if (FAILED(CreateDevice(&m_CreationParameters, &m_PresentParameters)))
			return FALSE;
	}

	//Set the basics
	SetupDevice();
	
	//Create the image processing shader
	if (FAILED(CreateShader()))
		return FALSE;

	//the texture must be created first to get the proper dimensions
	if (FAILED(CreateSpriteBuffer()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\ColorCurve.bmp", 
										 &m_pColorCurve)))
		return FALSE;

	if (FAILED(m_VideoTexture.CreateFromAVIFile(m_pD3DDevice,
												"..\\media\\bikerace.avi")))
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

	//Set the color curve textures for the color curve shader
//	m_pD3DDevice->SetTexture(1, m_pColorCurve);
//	m_pD3DDevice->SetTexture(2, m_pColorCurve);
//	m_pD3DDevice->SetTexture(3, m_pColorCurve);

//  or...

	//Set the other stages for the edge shader
	m_pD3DDevice->SetTexture(1, m_VideoTexture.m_pTexture);
	m_pD3DDevice->SetTexture(2, m_VideoTexture.m_pTexture);


	//These states apply equally well to any filter
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	//Set the current shader. Remember, some shaders require more stages (uncomment
	//lines above). Also, see CreateShaders below for a list of shaders.
	m_pD3DDevice->SetPixelShader(m_ImageShader);

	//The delta only affects brightness and contrast. All others ignore this.
	float Delta = -0.2f;
	m_pD3DDevice->SetPixelShaderConstant(0, 
		                                 &D3DXVECTOR4(Delta, Delta, Delta, 1.0f),
										 1);
	//Draw the sprite the "easy" way
	DrawSpriteEx(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	m_pD3DDevice->SetPixelShader(0);
}

BOOL CTechniqueApplication::PostReset()
{
	SetupDevice();

	return TRUE;
}


BOOL CTechniqueApplication::PreTerminate()
{
	m_pD3DDevice->DeletePixelShader(m_ImageShader);
	
	//Clean up
	if (m_pSpriteVertexBuffer)
	{
		m_pSpriteVertexBuffer->Release();
		m_pSpriteVertexBuffer = NULL;
	}

	if (m_pColorCurve)
	{
		m_pColorCurve->Release();
		m_pColorCurve = NULL;
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

	//Set up the 4 corners of a small square The second and third sets of texture coordinates
	//are ignored by most of the shaders.
	pSpriteVertices[0].x = -0.5f;   pSpriteVertices[0].y =  -0.5f;
	pSpriteVertices[0].z =  1.0f;
	pSpriteVertices[0].u1 =  0.0f;   pSpriteVertices[0].v1 =   0.0f;
	pSpriteVertices[0].u2 = pSpriteVertices[0].u1 - 1.0f / 1024.0f;
	pSpriteVertices[0].v2 = pSpriteVertices[0].v1;
	pSpriteVertices[0].u3 = pSpriteVertices[0].u1 + 1.0f / 1024.0f;
	pSpriteVertices[0].v3 = pSpriteVertices[0].v1;

	pSpriteVertices[1].x = -0.5f;   pSpriteVertices[1].y =   0.5f;
	pSpriteVertices[1].z =  1.0f;
	pSpriteVertices[1].u1 =  0.0f;   pSpriteVertices[1].v1 =   VMax;
	pSpriteVertices[1].u2 = pSpriteVertices[1].u1 - 1.0f / 1024.0f;
	pSpriteVertices[1].v2 = pSpriteVertices[1].v1;
	pSpriteVertices[1].u3 = pSpriteVertices[1].u1 + 1.0f / 1024.0f;
	pSpriteVertices[1].v3 = pSpriteVertices[1].v1;

	pSpriteVertices[2].x =  0.5f;   pSpriteVertices[2].y =  -0.5f;
	pSpriteVertices[2].z =  1.0f;
	pSpriteVertices[2].u1 =  UMax;   pSpriteVertices[2].v1 =   0.0f;
	pSpriteVertices[2].u2 = pSpriteVertices[2].u1 - 1.0f / 1024.0f;
	pSpriteVertices[2].v2 = pSpriteVertices[2].v1;
	pSpriteVertices[2].u3 = pSpriteVertices[2].u1 + 1.0f / 1024.0f;
	pSpriteVertices[2].v3 = pSpriteVertices[2].v1;

	pSpriteVertices[3].x =  0.5f;   pSpriteVertices[3].y =   0.5f;
	pSpriteVertices[3].z =  1.0f;
	pSpriteVertices[3].u1 =  UMax;   pSpriteVertices[3].v1 =   VMax;
	pSpriteVertices[3].u2 = pSpriteVertices[3].u1 - 1.0f / 1024.0f;
	pSpriteVertices[3].v2 = pSpriteVertices[3].v1;
	pSpriteVertices[3].u3 = pSpriteVertices[3].u1 + 1.0f / 1024.0f;
	pSpriteVertices[3].v3 = pSpriteVertices[3].v1;

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

HRESULT CTechniqueApplication::CreateShader()
{	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//The available shaders are:
	// AdjustBrightness.psh
	// AdjustContrast.psh
	// BlackAndWhite.psh
	// ChangeLevels.psh
	// ChangeLevelsGrayScale.psh
	// FindEdges.psh
	// Negative.psh
	// Sepia.psh
	// Solarize.psh

	//See the text for the care and feeding of each shader. If you are using ChangeLevels,
	//you should be careful to set more texture stages during render. This applies
	//to FindEdges as well.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\FindEdges.psh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreatePixelShader(
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_ImageShader)))
		return E_FAIL;

	pShaderBuffer->Release();

	return S_OK;
}

