/***************************************************************
* TextureApplication.cpp                                       *
*                                                              *
* This file contains the implementation of the                 *
* CTestingApplication class.		     					   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
* gdi32.lib                                                    *
* advapi32.lib                                                 *
*                                                              *
***************************************************************/

#include "TestingApplication.h"

#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

struct TEXTURE_VERTEX
{
	float x, y, z;
	float u, v;
};

CTestingApplication::CTestingApplication()
{
	//Initialize everything
	m_pVertexBuffer   = NULL;
	m_pFrontTexture = NULL;
	m_pBackTexture = NULL;
}

CTestingApplication::~CTestingApplication()
{
	DestroyGeometry();
}

BOOL CTestingApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	SetupDevice();

	if (!CreateGeometry())
		return FALSE;
	
	//All of our textures are in managed memory, so no need to 
	//initialize them more than once.
	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\Front.dds", 
										 &m_pFrontTexture)))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFileEx(m_pD3DDevice, 
		                                   "..\\media\\Back.bmp",
										   0, 0, 0, 0, D3DFMT_A8R8G8B8,
										   D3DPOOL_MANAGED, D3DX_DEFAULT,
										   D3DX_DEFAULT, 0xFFFFFFFF,
										   NULL, NULL,
										   &m_pBackTexture)))
		return FALSE;

	return TRUE;
}

void CTestingApplication::SetupDevice()
{
	D3DXMatrixIdentity(&m_ViewMatrix);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_ViewMatrix);

	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 4,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 100.0f);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProjectionMatrix);

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000088);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
}

BOOL CTestingApplication::PreReset()
{
	DestroyGeometry();
	return TRUE;
}

BOOL CTestingApplication::PostReset()
{
	SetupDevice();
	return CreateGeometry();
}

BOOL CTestingApplication::PreTerminate()
{
	DestroyGeometry();

	//Make sure a texture is not in use
	m_pD3DDevice->SetTexture(0, NULL);

	if (m_pFrontTexture)
		m_pFrontTexture->Release();

	if (m_pBackTexture)
		m_pBackTexture->Release();

	return TRUE;
}

BOOL CTestingApplication::CreateGeometry()
{
	//Same old drill...  Create, lock, fill unlock.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(8 * sizeof(TEXTURE_VERTEX),
										        D3DUSAGE_WRITEONLY,
										        D3DFVF_TEXTUREVERTEX,
										        D3DPOOL_DEFAULT,
										        &m_pVertexBuffer)))
		return FALSE;

	TEXTURE_VERTEX *pVertices;

	if (FAILED(m_pVertexBuffer->Lock(0, 8 * sizeof(TEXTURE_VERTEX),
					                 (BYTE **)&pVertices,
					                 0)))
	{
		DestroyGeometry();
		return FALSE;
	}

	//we are creating two simple squares that will be rendered with 
	//triangle strips. First set their position. This is the front
	//square (closer to the eye)
	pVertices[0].x = -1.0f; pVertices[0].y =  1.0f; pVertices[0].z = 10.0f;
	pVertices[1].x =  1.0f; pVertices[1].y =  1.0f; pVertices[1].z = 10.0f;
	pVertices[2].x = -1.0f; pVertices[2].y = -1.0f; pVertices[2].z = 10.0f;
	pVertices[3].x =  1.0f; pVertices[3].y = -1.0f; pVertices[3].z = 10.0f;

	//This is the back square (farther from the eye
	pVertices[4].x = -1.5f; pVertices[4].y =  1.5f; pVertices[4].z = 11.0f;
	pVertices[5].x =  0.5f; pVertices[5].y =  1.5f; pVertices[5].z = 11.0f;
	pVertices[6].x = -1.5f; pVertices[6].y = -0.5f; pVertices[6].z = 11.0f;
	pVertices[7].x =  0.5f; pVertices[7].y = -0.5f; pVertices[7].z = 11.0f;

	//Now set the texture coordinates. We'll set them to cover the full
	//texture. They are both the same.
	pVertices[0].u = 0.0f; pVertices[0].v = 0.0f;
	pVertices[1].u = 1.0f; pVertices[1].v = 0.0f;
	pVertices[2].u = 0.0f; pVertices[2].v = 1.0f;
	pVertices[3].u = 1.0f; pVertices[3].v = 1.0f;

	pVertices[4].u = 0.0f; pVertices[4].v = 0.0f;
	pVertices[5].u = 1.0f; pVertices[5].v = 0.0f;
	pVertices[6].u = 0.0f; pVertices[6].v = 1.0f;
	pVertices[7].u = 1.0f; pVertices[7].v = 1.0f;

	m_pVertexBuffer->Unlock();

	//Since we are not going to use another source...
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(TEXTURE_VERTEX));
	m_pD3DDevice->SetVertexShader(D3DFVF_TEXTUREVERTEX);

	return TRUE;
}

void CTestingApplication::DestroyGeometry()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CTestingApplication::PreRender()
{
	//We have overridden PreRender so that we can change the clear color,
	//but make sure you call BeginScene!!!!
	//Clear the device
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(0, 255, 255), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();

}

void CTestingApplication::Render()
{
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	D3DXMatrixTranslation(&m_WorldMatrix, -2.0f, 2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->SetTexture(0, m_pFrontTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pD3DDevice->SetTexture(0, m_pBackTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);

	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	D3DXMatrixTranslation(&m_WorldMatrix, 2.0f, 2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->SetTexture(0, m_pFrontTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pD3DDevice->SetTexture(0, m_pBackTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	D3DXMatrixTranslation(&m_WorldMatrix, -2.0f, -2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->SetTexture(0, m_pFrontTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pD3DDevice->SetTexture(0, m_pBackTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
	
	BYTE CurrentColor = (BYTE)(127.0f * (cos((float)GetTickCount() / 1000.0f) + 1.0));

	m_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 
		                         D3DCOLOR_ARGB(CurrentColor, CurrentColor, 
								               CurrentColor, CurrentColor));

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	D3DXMatrixTranslation(&m_WorldMatrix, 2.0f, -2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->SetTexture(0, m_pFrontTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 
		                         D3DCOLOR_ARGB(0, 0, 0, 0));
	m_pD3DDevice->SetTexture(0, m_pBackTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}