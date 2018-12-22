//=============================================================================
// DrawableTex2D.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "DrawableTex2D.h"
#include "d3dApp.h"

DrawableTex2D::DrawableTex2D(UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport)
: mTex(0), mRTS(0), mTopSurf(0), mWidth(width), mHeight(height), 
  mMipLevels(mipLevels), mTexFormat(texFormat), mUseDepthBuffer(useDepthBuffer),
  mDepthFormat(depthFormat), mViewPort(viewport)
{
}

DrawableTex2D::~DrawableTex2D()
{
	onLostDevice();
}

IDirect3DTexture9* DrawableTex2D::d3dTex()
{
	return mTex;
}

void DrawableTex2D::onLostDevice()
{
	ReleaseCOM(mTex);
	ReleaseCOM(mRTS);
	ReleaseCOM(mTopSurf);
}

void DrawableTex2D::onResetDevice()
{
	UINT usage = D3DUSAGE_RENDERTARGET;
	D3DCAPS9 caps;
	gd3dDevice->GetDeviceCaps(&caps);
	if( caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP )
		usage |= D3DUSAGE_AUTOGENMIPMAP; 

	HR(D3DXCreateTexture(gd3dDevice, mWidth, mHeight, mMipLevels, usage, mTexFormat, D3DPOOL_DEFAULT, &mTex));
	HR(D3DXCreateRenderToSurface(gd3dDevice, mWidth, mHeight, mTexFormat, mUseDepthBuffer, mDepthFormat, &mRTS));
	HR(mTex->GetSurfaceLevel(0, &mTopSurf));
}

void DrawableTex2D::beginScene()
{
	mRTS->BeginScene(mTopSurf, &mViewPort);
}

void DrawableTex2D::endScene()
{
	mRTS->EndScene(D3DX_FILTER_NONE);
}
 
void DrawableTex2D::clear(D3DCOLOR color, float depth, DWORD stencil)
{
	beginScene();

	DWORD clearFlags = D3DCLEAR_TARGET;
	if(mUseDepthBuffer)
		clearFlags |= D3DCLEAR_ZBUFFER;

	gd3dDevice->Clear(0, 0, clearFlags, color, depth, stencil);
	endScene();
}