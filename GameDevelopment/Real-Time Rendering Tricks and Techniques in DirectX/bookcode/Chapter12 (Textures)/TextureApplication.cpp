/***************************************************************
* TextureApplication.cpp                                       *
*                                                              *
* This file contains the implementation of the                 *
* CTextureApplication class.								   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
* gdi32.lib                                                    *
* advapi32.lib                                                 *
*                                                              *
***************************************************************/

#include "TextureApplication.h"

#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

struct TEXTURE_VERTEX
{
	float x, y, z;
	float u, v;
};

CTextureApplication::CTextureApplication()
{
	//Initialize everything
	m_pVertexBuffer   = NULL;
	m_pCheckerTexture = NULL;
	m_pMipMapTexture  = NULL;
	m_pImageTexture   = NULL;
}

CTextureApplication::~CTextureApplication()
{
	DestroyGeometry();
}

BOOL CTextureApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	SetupDevice();

	if (!CreateGeometry())
		return FALSE;
	
	//All of our textures are in managed memory, so no need to 
	//initialize them more than once.
	if (!CreateCustomTextures())
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\light.bmp", 
										 &m_pImageTexture)))
		return FALSE;

	return TRUE;
}

void CTextureApplication::SetupDevice()
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

	//This tells the device to use the texture matrix on 2D coordinates
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 
		                               D3DTTFF_COUNT2);

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
}

BOOL CTextureApplication::PreReset()
{
	DestroyGeometry();
	return TRUE;
}

BOOL CTextureApplication::PostReset()
{
	SetupDevice();
	return CreateGeometry();
}

BOOL CTextureApplication::PreTerminate()
{
	DestroyGeometry();

	//Make sure a texture is not in use
	m_pD3DDevice->SetTexture(0, NULL);

	if (m_pCheckerTexture)
		m_pCheckerTexture->Release();

	if (m_pMipMapTexture)
		m_pMipMapTexture->Release();

	if (m_pImageTexture)
		m_pImageTexture->Release();

	return TRUE;
}

BOOL CTextureApplication::CreateGeometry()
{
	//Same old drill...  Create, lock, fill unlock.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(TEXTURE_VERTEX),
										        D3DUSAGE_WRITEONLY,
										        D3DFVF_TEXTUREVERTEX,
										        D3DPOOL_DEFAULT,
										        &m_pVertexBuffer)))
		return FALSE;

	TEXTURE_VERTEX *pVertices;

	if (FAILED(m_pVertexBuffer->Lock(0, 4 * sizeof(TEXTURE_VERTEX),
					                 (BYTE **)&pVertices,
					                 0)))
	{
		DestroyGeometry();
		return FALSE;
	}

	//we are creating a simple square that will be rendered with a 
	//triangle strip. First set their position.
	pVertices[0].x = -1.0f; pVertices[0].y =  1.0f; pVertices[0].z = 10.0f;
	pVertices[1].x =  1.0f; pVertices[1].y =  1.0f; pVertices[1].z = 10.0f;
	pVertices[2].x = -1.0f; pVertices[2].y = -1.0f; pVertices[2].z = 10.0f;
	pVertices[3].x =  1.0f; pVertices[3].y = -1.0f; pVertices[3].z = 10.0f;

	//Now set the texture coordinates. We'll set them to cover the full
	//texture and use the texture matrix to scale them.
	pVertices[0].u = 0.0f; pVertices[0].v = 0.0f;
	pVertices[1].u = 1.0f; pVertices[1].v = 0.0f;
	pVertices[2].u = 0.0f; pVertices[2].v = 1.0f;
	pVertices[3].u = 1.0f; pVertices[3].v = 1.0f;

	m_pVertexBuffer->Unlock();

	//Since we are not going to use another source...
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(TEXTURE_VERTEX));
	m_pD3DDevice->SetVertexShader(D3DFVF_TEXTUREVERTEX);

	return TRUE;
}

void CTextureApplication::DestroyGeometry()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CTextureApplication::PreRender()
{
	//We have overridden PreRender so that we can change the clear color,
	//but make sure you call BeginScene!!!!
		//Clear the device
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();

}

void CTextureApplication::Render()
{
	//This is our matrix for texture transformations
	D3DXMATRIX TextureMatrix;
	//Set the texture transform back to the identity matrix
	D3DXMatrixIdentity(&TextureMatrix);
	m_pD3DDevice->SetTransform(D3DTS_TEXTURE0, &TextureMatrix);

	m_pD3DDevice->SetTexture(0, m_pCheckerTexture);
	
	//The first square shows the checkerboard pattern as it 
	//actually appears in memory.
	D3DXMatrixTranslation(&m_WorldMatrix, -2.0f, 2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//The second square shows how the texture can be repeated
	//to turn a simple texture into a more useful one. Scaling
	//the texture matrix by 8.0 has the same effect as setting
	//the maximum texture coordinates to (8.0, 8.0) if we had 
	//set them directly in the vertices.
	D3DXMatrixScaling(&TextureMatrix, 8.0f, 8.0f, 1.0f);
	m_pD3DDevice->SetTransform(D3DTS_TEXTURE0, &TextureMatrix);

	D3DXMatrixTranslation(&m_WorldMatrix, 2.0f, 2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	//Set up a scaling factor for the texture matrix. This is
	//just an easy way to experiment with new texture coordinates
	//without changing vertex data.
	float ScaleFactor = (cos((float)GetTickCount() / 1000.0f) + 1.0) * 10.0f;
	D3DXMatrixScaling(&TextureMatrix, ScaleFactor, ScaleFactor, 1.0f);
	m_pD3DDevice->SetTransform(D3DTS_TEXTURE0, &TextureMatrix);

	//We are setting the texture coordinates to force the device to use 
	//different mipmap levels. On the left we show an image that was
	//loaded from a bitmap file with mipmaps automatically generated.
	//On the left, it's easier to explicitly see the individual levels
	//because the colors correspond to the size of the mipmap.

	//use the image texture
	m_pD3DDevice->SetTexture(0, m_pImageTexture);
	D3DXMatrixTranslation(&m_WorldMatrix, -2.0f, -2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//Use the mip map texture
	m_pD3DDevice->SetTexture(0, m_pMipMapTexture);
	D3DXMatrixTranslation(&m_WorldMatrix, 2.0f, -2.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

BOOL CTextureApplication::CreateCustomTextures()
{
	//These are some working variables we'll use for various operations
	LPDIRECT3DSURFACE8 pWorkSurface;
	D3DLOCKED_RECT     WorkRect;

	//Create the simple checkerboard texture. This is a simple 2x2
	//texture. We will request a full 32bit texture
	//in managed memory. 32bit is overkill for this, but I'm using it
	//for consistency.
	if (FAILED(m_pD3DDevice->CreateTexture(2, 2, 0, 0, 
		                                   D3DFMT_A8R8G8B8,
										   D3DPOOL_MANAGED,
										   &m_pCheckerTexture)))
		return FALSE;

	//Set the checker pattern. First get the surface and lock the 
	//whole rectangle.
	m_pCheckerTexture->GetSurfaceLevel(0, &pWorkSurface);
	pWorkSurface->LockRect(&WorkRect, NULL, 0);

	//set the first 4 bytes to 0xff (white) and 
	//the second 4 bytes to 0x00 (black), then increment by the pitch
	//and so the reverse for the second row. In most cases, the pitch
	//will probably be 8, but that's not guaranteed.
	memset((BYTE *)WorkRect.pBits, 0xff, 4);
	memset((BYTE *)WorkRect.pBits + 4, 0x00, 4);
	memset((BYTE *)WorkRect.pBits + WorkRect.Pitch, 0x00, 4);
	memset((BYTE *)WorkRect.pBits + WorkRect.Pitch + 4, 0xff, 4);

	//Now unlock the rect and release the surface.
	pWorkSurface->UnlockRect();
	pWorkSurface->Release();


	//The mipmap texture is created to show off mipmaps in a very
	//obvious way. Assuming the texture is created successfully, we
	//will run through each surface and set all the bits in each surface
	//to descending levels of gray. Not particularly interesting, but
	//to shows how and when different levels are being used.
	if (FAILED(m_pD3DDevice->CreateTexture(256, 256, 0, 0, 
		                                   D3DFMT_A8R8G8B8,
										   D3DPOOL_MANAGED,
										   &m_pMipMapTexture)))
		return FALSE;

	//Loop through the levels and set each one to a gray color. This time, 
	//we do things a little differently. We call LockRect directly on the
	//texture, which has the same effect as above without all the steps.
	//We don't have direct access to the surface object, 
	//but we don't need it...
	for (long Level = 0; Level < m_pMipMapTexture->GetLevelCount(); Level++)
	{
		//Get the level description so that we know how many bytes to set.
		D3DSURFACE_DESC LevelDescription;
		m_pMipMapTexture->GetLevelDesc(Level, &LevelDescription);

		//use the width of the level to define the color
		BYTE Color = (BYTE)LevelDescription.Width - 1;

		m_pMipMapTexture->LockRect(Level, &WorkRect, NULL, 0);

		//Because we are using a 256x256 texture, in most cases, we can assume
		//that the pitch is simply 4 * our width, but it's not a guarantee.
		//To show the good practice way, here we are filling in each row, but
		//only the number of bytes that we know are valid pixels and not
		//in the cache.
		for (long Row = 0; Row < LevelDescription.Height; Row++)
		{
			memset((BYTE *)WorkRect.pBits + (Row * WorkRect.Pitch), Color,
			       LevelDescription.Width * 4);
		}

		m_pMipMapTexture->UnlockRect(Level);
	}

	return TRUE;
}