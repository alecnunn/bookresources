//=============================================================================
// DrawableTex2D.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#ifndef DRAWABLE_TEX2D_H
#define DRAWABLE_TEX2D_H

#include "d3dUtil.h"

class DrawableTex2D
{
public:
	DrawableTex2D(UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport);
	~DrawableTex2D();

	IDirect3DTexture9* d3dTex();

	void beginScene();
	void endScene();

	void onLostDevice();
	void onResetDevice();

	void clear(D3DCOLOR color = 0xff000000, float depth = 1.0f, DWORD stencil = 0);

private:
	// This class is not designed to be copied.
	DrawableTex2D(const DrawableTex2D& rhs);
	DrawableTex2D& operator=(const DrawableTex2D& rhs);

private:
	IDirect3DTexture9*    mTex;
	ID3DXRenderToSurface* mRTS;
	IDirect3DSurface9*    mTopSurf;

	UINT         mWidth;
	UINT         mHeight;
	UINT         mMipLevels;
	D3DFORMAT    mTexFormat;
	bool         mUseDepthBuffer;
	D3DFORMAT    mDepthFormat;
	D3DVIEWPORT9 mViewPort;
};

#endif // DRAWABLE_TEX2D_H