//--------------------------------------------------------------------------------------
// File: HeightQuery.h
//
// Copyright (C) 2005 - Harald Vistnes (harald@vistnes.org)
// All rights reserved worldwide.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (C) Harald Vistnes, 2005"
//--------------------------------------------------------------------------------------

#ifndef __HEIGHT_QUERY_H_
#define __HEIGHT_QUERY_H_

class HeightQuery
{
public:
	HeightQuery();
	~HeightQuery();

	HRESULT OnResetDevice(IDirect3DDevice9 *pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

	void SetHeightfield(IDirect3DTexture9 *pHeightfield);

	HRESULT QueryPoint(float u, float v, float *height);

protected:
	HRESULT InitializeEffect();
	HRESULT InitializeVertexBuffer();
	HRESULT InitializeSurfaces();
	HRESULT InitializeMatrices();

	HRESULT Render();

	HRESULT SetVertexData(float u, float v);

private:
	IDirect3DDevice9*		m_pd3dDevice;
	IDirect3DTexture9*		m_pHeightfield;
	IDirect3DTexture9*		m_pTexture;
	IDirect3DSurface9*		m_pSurface;
	IDirect3DSurface9*		m_pSysMemSurface;
	IDirect3DVertexBuffer9*	m_pVB;
	ID3DXRenderToSurface*	m_pRenderToSurface;
	ID3DXEffect*			m_pEffect;
	D3DSURFACE_DESC			m_Desc;

	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProjection;
	D3DXMATRIX				m_matWorldOrig;
	D3DXMATRIX				m_matViewOrig;
	D3DXMATRIX				m_matProjectionOrig;
	D3DVIEWPORT9			m_Viewport;
};

#endif // __HEIGHT_QUERY_H_
