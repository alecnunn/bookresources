//--------------------------------------------------------------------------------------
// File: HeightQuery.cpp
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

#include "dxstdafx.h"
#include "HeightQuery.h"

//----------------------------------------------------------------------------------

HeightQuery::HeightQuery()
	:
	m_pd3dDevice		(0),
	m_pHeightfield		(0),
	m_pTexture			(0),
	m_pSurface			(0),
	m_pSysMemSurface	(0),
	m_pVB				(0),
	m_pRenderToSurface	(0),
	m_pEffect			(0)
{
}

//----------------------------------------------------------------------------------

HeightQuery::~HeightQuery()
{
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::OnResetDevice(IDirect3DDevice9 *pd3dDevice)
{
	if (!pd3dDevice) {
		return E_INVALIDARG;
	}

	SAFE_RELEASE(m_pd3dDevice);
	m_pd3dDevice = pd3dDevice;
	m_pd3dDevice->AddRef();

	if (FAILED(InitializeEffect())) {
		return E_FAIL;
	}
	if (FAILED(InitializeVertexBuffer())) {
		return E_FAIL;
	}
	if (FAILED(InitializeSurfaces())) {
		return E_FAIL;
	}
	if (FAILED(InitializeMatrices())) {
		return E_FAIL;
	}
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::InitializeVertexBuffer()
{
	SAFE_RELEASE(m_pVB);
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &m_pVB, 0))) {
		return E_FAIL;
	}
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::InitializeEffect()
{
	ID3DXBuffer* pErrors = 0;
	SAFE_RELEASE(m_pEffect);
	if (FAILED(D3DXCreateEffectFromFile(m_pd3dDevice, L"HeightQuery.fx", 0, 0, 0, 0, &m_pEffect, &pErrors))) {
		if (pErrors) {
			WCHAR strErr[512];

			// First attempt to look for texture in the same folder as the input folder.
			MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pErrors->GetBufferPointer(), -1, strErr, 512 );
			strErr[511] = 0;

			MessageBox(NULL, strErr, L"GPUTerrain failed", MB_OK | MB_ICONERROR);
			pErrors->Release();
		}
		return E_FAIL;
	}
	
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::InitializeMatrices()
{
	D3DXVECTOR3 zero(0,0,0);
	D3DXVECTOR3 unitY(0,1,0);
	D3DXVECTOR3 unitZ(0,0,1);

	D3DXMatrixLookAtLH(&m_matView, &zero, &unitZ, &unitY);
	D3DXMatrixOrthoLH(&m_matProjection, 1, 1, 0.1f, 10.0f);
	D3DXMatrixTranslation(&m_matWorld, -1/2, -1/2, 0);
	m_Viewport.X = 0;
	m_Viewport.Y = 0;
	m_Viewport.Width = 1;
	m_Viewport.Height = 1;
	m_Viewport.MinZ = 0.01f;
	m_Viewport.MaxZ = 1.0f;
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::InitializeSurfaces()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSurface);
	SAFE_RELEASE(m_pSysMemSurface);
	SAFE_RELEASE(m_pRenderToSurface);

	if (FAILED(D3DXCreateTexture(m_pd3dDevice, 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pTexture))) {
		return E_FAIL;
	}
	if (FAILED(m_pTexture->GetSurfaceLevel(0, &m_pSurface))) {
		return E_FAIL;
	}
	if (FAILED(m_pSurface->GetDesc(&m_Desc))) {
		return E_FAIL;
	}

	if (FAILED(m_pd3dDevice->CreateOffscreenPlainSurface(1, 1, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &m_pSysMemSurface, 0))) {
		return E_FAIL;
	}

	if (FAILED(D3DXCreateRenderToSurface(m_pd3dDevice, 1, 1, D3DFMT_R32F, FALSE, D3DFMT_UNKNOWN, &m_pRenderToSurface))) {
		return E_FAIL;
	}
	return S_OK;
}

//----------------------------------------------------------------------------------

void HeightQuery::SetHeightfield(IDirect3DTexture9 *pHeightfield) 
{
	SAFE_RELEASE(m_pHeightfield);
	if (pHeightfield) {
		m_pHeightfield = pHeightfield;
		m_pHeightfield->AddRef();
	}
}

//----------------------------------------------------------------------------------

void HeightQuery::OnLostDevice()
{
	SAFE_RELEASE(m_pSysMemSurface);
	SAFE_RELEASE(m_pSurface);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pRenderToSurface);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pHeightfield);
	SAFE_RELEASE(m_pd3dDevice);
}

//----------------------------------------------------------------------------------

void HeightQuery::OnDestroyDevice()
{
	SAFE_RELEASE(m_pSysMemSurface);
	SAFE_RELEASE(m_pSurface);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pRenderToSurface);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pHeightfield);
	SAFE_RELEASE(m_pd3dDevice);
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::Render()
{
	if (!m_pd3dDevice || !m_pSurface || !m_pVB || !m_pRenderToSurface || !m_pEffect || !m_pHeightfield) {
		return E_FAIL;
	}

	if (FAILED(m_pd3dDevice->GetTransform( D3DTS_WORLD, &m_matWorldOrig))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->GetTransform( D3DTS_VIEW, &m_matViewOrig))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &m_matProjectionOrig))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection))) {
		return E_FAIL;
	}
	if (FAILED(m_pRenderToSurface->BeginScene( m_pSurface, &m_Viewport ))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00ff0000, 1.0f, 0))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetFVF(D3DFVF_XYZ))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(D3DXVECTOR3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pEffect->SetTexture("g_Heightfield", m_pHeightfield))) {
		return E_FAIL;
	}
	UINT nPasses = 0;
	if (FAILED(m_pEffect->Begin(&nPasses, 0))) {
		return E_FAIL;
	}
	for (UINT i = 0; i < nPasses; i++) {
		m_pEffect->BeginPass(i);
		if (FAILED(m_pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 1))) {
			return E_FAIL;
		}
		m_pEffect->EndPass();
	}
	if (FAILED(m_pEffect->End())) {
		return E_FAIL;
	}
	if (FAILED(m_pRenderToSurface->EndScene( 0 ))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorldOrig))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matViewOrig))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjectionOrig))) {
		return E_FAIL;
	}
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::SetVertexData(float u, float v)
{
	D3DXVECTOR3 *pVertex = 0;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD))) {
		return E_FAIL;
	}
	pVertex->x = u;
	pVertex->y = 0;
	pVertex->z = v;

	m_pVB->Unlock();
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT HeightQuery::QueryPoint(float u, float v, float *height)
{
	if (!m_pd3dDevice || !m_pSurface || !m_pSysMemSurface) {
		return E_FAIL;
	}
	if (FAILED(SetVertexData(u, v))) {
		return E_FAIL;
	}
	if (FAILED(Render())) {
		return E_FAIL;
	}

	// copy the render target to system memory
	if (FAILED(m_pd3dDevice->GetRenderTargetData(m_pSurface, m_pSysMemSurface))) {
		return E_FAIL;
	}

	// extract the queried height
	D3DLOCKED_RECT rect;
	if (FAILED(m_pSysMemSurface->LockRect(&rect, 0, 0))) {
		return E_FAIL;
	}
	*height = ((float*)rect.pBits)[0];
	m_pSysMemSurface->UnlockRect();
	return S_OK;
}
