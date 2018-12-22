/***************************************************************
* TextureApplication.cpp                                       *
*                                                              *
* This file contains the implementation of the                 *
* CTextureStateApplication class.							   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
* gdi32.lib                                                    *
* advapi32.lib                                                 *
*                                                              *
***************************************************************/

#include "TextureStateApplication.h"

#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

struct TEXTURE_VERTEX
{
	float x, y, z;
	float u, v;
};

CTextureStateApplication::CTextureStateApplication()
{
	//Initialize everything
	m_pVertexBuffer   = NULL;
	m_pCheckerTexture = NULL;

	//Set the default modes
	m_CurrentFilterMode = 1;
	m_CurrentAddressMode = 1;
}

CTextureStateApplication::~CTextureStateApplication()
{
	DestroyGeometry();
}

BOOL CTextureStateApplication::PostInitialize()
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
		                                 "..\\media\\Big_Checker.bmp", 
										 &m_pCheckerTexture)))
		return FALSE;

	VerifyModes();


	return TRUE;
}

void CTextureStateApplication::SetupDevice()
{
	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0f, 0.25f, 2.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
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
}

BOOL CTextureStateApplication::HandleMessage(MSG *pMessage)
{
	//Handle user input.  F1 sets the current addressing mode.
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_F1)
	{
		if (++m_CurrentAddressMode > 5)
			m_CurrentAddressMode = 1;

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, 
			                               (D3DTEXTUREADDRESS)m_CurrentAddressMode);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, 
			                               (D3DTEXTUREADDRESS)m_CurrentAddressMode);
	}

	//F2 runs through the different filtering modes
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_F2)
	{
		if (++m_CurrentFilterMode > 5)
			m_CurrentFilterMode = 1;

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER,
			                               (D3DTEXTUREFILTERTYPE)m_CurrentFilterMode);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER,
			                               (D3DTEXTUREFILTERTYPE)m_CurrentFilterMode);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER,
			                               (D3DTEXTUREFILTERTYPE)m_CurrentFilterMode);
	}

	//Mke sure the base class receives any messages
	return CHostApplication::HandleMessage(pMessage);
}


BOOL CTextureStateApplication::PreReset()
{
	DestroyGeometry();
	return TRUE;
}

BOOL CTextureStateApplication::PostReset()
{
	SetupDevice();
	return CreateGeometry();
}

BOOL CTextureStateApplication::PreTerminate()
{
	DestroyGeometry();

	//Make sure a texture is not in use
	m_pD3DDevice->SetTexture(0, NULL);

	if (m_pCheckerTexture)
		m_pCheckerTexture->Release();

	return TRUE;
}

BOOL CTextureStateApplication::CreateGeometry()
{
	//Same old drill...  Create, lock, fill unlock.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(6 * sizeof(TEXTURE_VERTEX),
										        D3DUSAGE_WRITEONLY,
										        D3DFVF_TEXTUREVERTEX,
										        D3DPOOL_DEFAULT,
										        &m_pVertexBuffer)))
		return FALSE;

	TEXTURE_VERTEX *pVertices;

	if (FAILED(m_pVertexBuffer->Lock(0, 6 * sizeof(TEXTURE_VERTEX),
					                 (BYTE **)&pVertices,
					                 0)))
	{
		DestroyGeometry();
		return FALSE;
	}

	//we are creating a "floor and wall" arrangement that will be rendered with a 
	//triangle strip. First set their position.
	pVertices[0].x = -1.0f; pVertices[0].y = 0.0f; pVertices[0].z =  1.0f;
	pVertices[1].x =  1.0f; pVertices[1].y = 0.0f; pVertices[1].z =  1.0f;
	pVertices[2].x = -1.0f; pVertices[2].y = 0.0f; pVertices[2].z = -1.0f;
	pVertices[3].x =  1.0f; pVertices[3].y = 0.0f; pVertices[3].z = -1.0f;
	pVertices[4].x = -1.0f; pVertices[4].y = 1.0f; pVertices[4].z = -1.0f;
	pVertices[5].x =  1.0f; pVertices[5].y = 1.0f; pVertices[5].z = -1.0f;

	//Now set the texture coordinates. 
	pVertices[0].u = -10.0f; pVertices[0].v = -10.0f;
	pVertices[1].u =  10.0f; pVertices[1].v = -10.0f;
	pVertices[2].u = -10.0f; pVertices[2].v =  10.0f;
	pVertices[3].u =  10.0f; pVertices[3].v =  10.0f;
	pVertices[4].u = -10.0f; pVertices[4].v = -10.0f;
	pVertices[5].u =  10.0f; pVertices[5].v = -10.0f;

	m_pVertexBuffer->Unlock();

	//Since we are not going to use another source...
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(TEXTURE_VERTEX));
	m_pD3DDevice->SetVertexShader(D3DFVF_TEXTUREVERTEX);

	return TRUE;
}

void CTextureStateApplication::DestroyGeometry()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CTextureStateApplication::PreRender()
{
	//We have overridden PreRender so that we can change the clear color,
	//but make sure you call BeginScene!!!!
		//Clear the device
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();

}

void CTextureStateApplication::Render()
{
	m_pD3DDevice->SetTexture(0, m_pCheckerTexture);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 4);

}

void CTextureStateApplication::VerifyModes()
{
	//Get the device caps
	D3DCAPS8 Caps;
	m_pD3DDevice->GetDeviceCaps(&Caps);

	//This is a fairly sloppy way to do this, but we don't have
	//a better method to do UI (yet). You may want to comment
	//these lines out once you understand the limitations of your device.
	if (!(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER))
		MessageBox(m_hWnd, "The Border Addressing mode is not available.", "", MB_OK);

	if (!(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP))
		MessageBox(m_hWnd, "The Clamp Addressing mode is not available.", "", MB_OK);

	if (!(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR))
		MessageBox(m_hWnd, "The Mirror Addressing mode is not available.", "", MB_OK);

	if (!(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE))
		MessageBox(m_hWnd, "The Mirror Once Addressing mode is not available.", "", MB_OK);

	if (!(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_WRAP))
		MessageBox(m_hWnd, "The Wrap Addressing mode is not available.", "", MB_OK);

	//We are going to assume that if the device supports a given filter
	//type for the magnification filter that it will support it for the 
	//minification and mip map filters as well. THIS IS NOT NECESSARILY
	//GUARANTEED TO BE TRUE, but it should give you a general feel for
	//what your device is capable of. If you need to check, replicate these
	//lines replacing MAG with MIN and/or MIP.
	if (!(Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
		MessageBox(m_hWnd, "The Point Filtering mode is not available.", "", MB_OK);

	if (!(Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
		MessageBox(m_hWnd, "The Linear Filtering mode is not available.", "", MB_OK);

	if (!(Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC))
		MessageBox(m_hWnd, "The Anisotropic Filtering mode is not available.", "", MB_OK);

	if (!(Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFAFLATCUBIC))
		MessageBox(m_hWnd, "The Cubic Filtering mode is not available.", "", MB_OK);

	if (!(Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC))
		MessageBox(m_hWnd, "The Gaussian Cubic Filtering mode is not available.", "", MB_OK);
}
