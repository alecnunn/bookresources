//--------------------------------------------------------------------------------------
// File: Terrain.cpp
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
#include "Terrain.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "HeightQuery.h"
#include "Camera.h"

//----------------------------------------------------------------------------------

Terrain::Terrain()
	:
	m_pd3dDevice		(0),
	m_pVertexTexture	(0),
	m_pNormalMap		(0),
	m_pEffect			(0),
	m_pDecl				(0),
	m_pVertexBuffer		(new VertexBuffer()),
	m_pIndexBuffer		(new IndexBuffer()),
	m_pHeightQuery		(new HeightQuery()),
	m_iTime				(0),
	m_fNumTrisPerSecond	(0),
	m_fLOD				(100.0f),
	m_bWireframe		(false),
	m_bNormalInVS		(false),
	m_bFrustumCulling	(true),
	m_iMaxLevels		(0),
	m_iBlockSize		(0),
	m_iNumCulled		(0)
{
	// set up the light direction
	m_vLightDir = D3DXVECTOR4(1.0f,-1.0f,0.5f,0.0f);
	D3DXVec4Normalize(&m_vLightDir, &m_vLightDir);
}

//----------------------------------------------------------------------------------

Terrain::~Terrain()
{
	SAFE_DELETE(m_pVertexBuffer);
	SAFE_DELETE(m_pIndexBuffer);
	SAFE_DELETE(m_pHeightQuery);
}

//----------------------------------------------------------------------------------

HRESULT Terrain::Initialize(IDirect3DDevice9 *pd3dDevice, int iBlockSize)
{
	HRESULT hr = CheckCapabilities(pd3dDevice);
	if (FAILED(hr)) {
		OnDestroyDevice();
		return hr;
	}
	m_pd3dDevice = pd3dDevice;

	if (FAILED(InitializeEffect())) {
		OnDestroyDevice();
		return E_FAIL;
	}

	if (FAILED(InitializeHeightfield())) {
		OnDestroyDevice();
		return E_FAIL;
	}
	if (FAILED(SetBlockSize(iBlockSize))) {
		OnDestroyDevice();
		return E_FAIL;
	}

	// create the vertex declaration
	D3DVERTEXELEMENT9 decl[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		D3DDECL_END()
	};
	if (FAILED(m_pd3dDevice->CreateVertexDeclaration(decl, &m_pDecl))) {
		OnDestroyDevice();
		return E_FAIL;
	}

	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT Terrain::CheckCapabilities(IDirect3DDevice9 *pd3dDevice)
{
	if (!pd3dDevice) {
		return E_INVALIDARG;
	}

	if (FAILED(pd3dDevice->GetDeviceCaps(&m_Caps))) {
		return E_FAIL;
	}

	// Test for vs 3.0
	if (m_Caps.VertexShaderVersion < D3DVS_VERSION(3,0)) {
 		MessageBox(NULL, L"GPUTerrain cannot run. Vertex shader 3.0 is not supported.", L"GPUTerrain failed", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// Test for ps 3.0
	if(m_Caps.PixelShaderVersion < D3DPS_VERSION(3,0)) {
		MessageBox(NULL, L"GPUTerrain cannot run. Pixel shader 3.0 is not supported.", L"GPUTerrain failed", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	
	// verify that D3DFMT_R32F can actually be used for vertex textures
	IDirect3D9 *pD3D = 0;
	if (FAILED(pd3dDevice->GetDirect3D(&pD3D))) {
		SAFE_RELEASE(pD3D);
		return E_FAIL;
	}
	D3DDISPLAYMODE mode;
	if (FAILED(pd3dDevice->GetDisplayMode(0, &mode))) {
		SAFE_RELEASE(pD3D);
		return E_FAIL;
	}
	if (FAILED(pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, D3DFMT_R32F))) {
		MessageBox(NULL, L"GPUTerrain cannot run. D3DFMT_R32F cannot be used for vertex textures.", L"GPUTerrain failed", MB_OK | MB_ICONERROR);
		SAFE_RELEASE(pD3D);
		return E_FAIL;
	}
	SAFE_RELEASE(pD3D);
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT Terrain::OnResetDevice(IDirect3DDevice9 *pd3dDevice)
{
	if (m_pVertexBuffer && FAILED(m_pVertexBuffer->OnResetDevice(pd3dDevice))) {
		return E_FAIL;
	}
	if (m_pEffect && FAILED(m_pEffect->OnResetDevice())) {
		return E_FAIL;
	}
	if (m_pHeightQuery && FAILED(m_pHeightQuery->OnResetDevice(pd3dDevice))) {
		return E_FAIL;
	}
	m_pHeightQuery->SetHeightfield(m_pVertexTexture);
	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT Terrain::InitializeEffect()
{
	SAFE_RELEASE(m_pEffect);

	ID3DXBuffer *pErrors = 0;
	DWORD dwShaderFlags = 0;

	if (FAILED(D3DXCreateEffectFromFile(m_pd3dDevice, L"GPUTerrain.fx", NULL, NULL, dwShaderFlags, NULL, &m_pEffect, &pErrors))) {
		if (pErrors) {
			WCHAR strErr[512];
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

HRESULT Terrain::InitializeHeightfield()
{
	if (!m_pd3dDevice) {
		return E_INVALIDARG;
	}

	// load the FMT_R32F height field
	SAFE_RELEASE(m_pVertexTexture);
	if (FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice, L"heightfield.dds", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_R32F, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &m_pVertexTexture))) {
		return E_FAIL;
	}

	if (FAILED(m_pVertexTexture->GetLevelDesc(0, &m_Desc))) {
		return E_FAIL;
	}

	// load the FMT_V8U8 normal map 
	SAFE_RELEASE(m_pNormalMap);
	if (FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice, L"normalmap.dds", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 0, 0, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &m_pNormalMap))) {
		return E_FAIL;
	}

	// Setup the camera's world matrix
	float sx = (float)m_Desc.Width;
	float sy = (float)MAX_ELEVATION; // the world vertical extent - the elevation span
	float sz = (float)m_Desc.Height;
;
	// the world matrix is a non-uniform scaling matrix
	D3DXMatrixScaling(&g_Camera.GetWorldMatrix(), sx, sy, sz);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &g_Camera.GetWorldMatrix());

	// set render state for vertex texture
	if (FAILED(m_pd3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP))) {
		return E_FAIL;
	} 
	if (FAILED(m_pd3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP))) {
		return E_FAIL;
	} 
	if (FAILED(m_pd3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MINFILTER, D3DTEXF_POINT))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAGFILTER, D3DTEXF_POINT))) {
		return E_FAIL;
	}

	return S_OK;
}

//----------------------------------------------------------------------------------

HRESULT Terrain::SetBlockSize(int iBlockSize)
{
	if (iBlockSize != m_iBlockSize) {

		// the block size has changed, must recreate the 
		// vertex buffer and index buffer

		m_iBlockSize = iBlockSize;
		if (FAILED(m_pIndexBuffer->Initialize(m_pd3dDevice, m_iBlockSize, m_iBlockSize, true))) {
			OnDestroyDevice();
			return E_FAIL;
		}
		if (FAILED(m_pVertexBuffer->Initialize(m_pd3dDevice, m_iBlockSize, m_iBlockSize, true))) {
			OnDestroyDevice();
			return E_FAIL;
		}
		m_iMaxLevels = CalculateMaxLevels(m_iBlockSize, m_Desc.Width);
	}
	return S_OK;
}

//----------------------------------------------------------------------------------

bool Terrain::Draw(float fBiasU, float fBiasV, float fScale, int iLevel) 
{
	float fTexelSizeU, fTexelSizeV;
	fTexelSizeU = (1<<iLevel) / float(m_Desc.Width-1);
	fTexelSizeV = (1<<iLevel) / float(m_Desc.Height-1);

	bool bSuccess = true;

	D3DXVECTOR4 vTexelSize(fTexelSizeU, fTexelSizeV, 1.0f, 0.0f);

	m_pEffect->SetFloat("fBiasU", fBiasU);
	m_pEffect->SetFloat("fBiasV", fBiasV);
	m_pEffect->SetFloat("fScale", fScale);
	m_pEffect->SetFloat("fNormalScale", 2.0f * (1<<iLevel) / MAX_ELEVATION);
	m_pEffect->SetVector("vTexelSize", &vTexelSize);

	UINT iPass, cPasses;
	m_pEffect->SetTechnique(m_bNormalInVS ? "TShaderVSNormal" : "TShaderPSNormal");
	m_pEffect->Begin(&cPasses, 0);
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		m_pEffect->BeginPass(iPass);
		if (FAILED(m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_pVertexBuffer->GetNumVertices(), 0, m_pIndexBuffer->GetNumIndices()-2))) {
			bSuccess = false;
		}
		m_pEffect->EndPass();
	}
	m_pEffect->End();

	m_iNumTriangles += m_pIndexBuffer->GetNumIndices()-2;
	return bSuccess;
}

//----------------------------------------------------------------------------------

void Terrain::Render(float fMinU, float fMinV, float fMaxU, float fMaxV, int iLevel, float fScale)
{
	float fHalfU = (fMinU + fMaxU) / 2.0f;
	float fHalfV = (fMinV + fMaxV) / 2.0f;

	if (m_bFrustumCulling) {
		if (m_Frustum.CullAABB(fMinU, 0.0f, fMinV, fMaxU, MAX_ELEVATION, fMaxV)) {
			m_iNumCulled++;
			return;
		}
	}

	float d = (fMaxU-fMinU) * m_Desc.Width / (m_iBlockSize-1.0f);
	float d2 = d * d;

	D3DXVECTOR3 v = D3DXVECTOR3(fHalfU*m_Desc.Width, 0, fHalfV*m_Desc.Height) - g_Camera.GetEyePt();
	float l2 = D3DXVec3LengthSq(&v);

	// use distances squared
	float f2 = l2 / d2;

	if (f2 > m_fLOD*m_fLOD || iLevel < 1) {
		Draw(fMinU, fMinV, fScale, iLevel);
	} else {
		fScale = fScale / 2.0f;
		Render(fMinU, fMinV, fHalfU, fHalfV, iLevel-1, fScale);
		Render(fHalfU, fMinV, fMaxU, fHalfV, iLevel-1, fScale);
		Render(fMinU, fHalfV, fHalfU, fMaxV, iLevel-1, fScale);
		Render(fHalfU, fHalfV, fMaxU, fMaxV, iLevel-1, fScale);
	}
}

//----------------------------------------------------------------------------------

bool Terrain::OnRender()
{
	if (FAILED(m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID))) {
		return false;
	}
	if (FAILED(m_pd3dDevice->SetVertexDeclaration(m_pDecl))) {
		return false;
	}
	if (FAILED(m_pd3dDevice->SetIndices(m_pIndexBuffer->GetIndexBuffer()))) {
		return false;
	}
	if (FAILED(m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer->GetVertexBuffer(), 0, sizeof(D3DXVECTOR3)))) {
		return false;
	}
	if (FAILED(m_pd3dDevice->SetTexture(D3DVERTEXTEXTURESAMPLER0, m_pVertexTexture))) {
		return false;
	}

	m_iNumCulled = 0;
	Render(0.0f, 0.0f, 1.0f, 1.0f, m_iMaxLevels, 1.0f);

	return true;
}

//----------------------------------------------------------------------------------

void Terrain::AvoidCollision()
{
	// eye position in local coordinates
	D3DXVECTOR3 ptEyeLocal = g_Camera.GetEyeLocal();

	float height;

	// query the height of the terrain at the eye position
	if (SUCCEEDED(m_pHeightQuery->QueryPoint(ptEyeLocal.x, ptEyeLocal.z, &height))) {
		// if the camera is below the terrain
		if (height > ptEyeLocal.y) {

			// update the camera to set the elevation 2 meters above the ground
			g_Camera.SetCameraElevation(height, 2.0f);
		}
	}
}

//----------------------------------------------------------------------------------

void Terrain::OnFrameMove(double fTime)
{
	if (fTime - m_iTime > 1.0) {
		m_fNumTrisPerSecond = m_iNumTriangles / (fTime - m_iTime);
		m_iTime = (int)fTime;
		m_iNumTriangles = 0;
	}

	AvoidCollision();

	// update and set the view matrix
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &g_Camera.GetViewMatrix());

	const D3DXMATRIX &matWorldViewProj = g_Camera.GetWorldViewProjMatrix();

	// extract the frustum planes from the world-view-projection matrix
	m_Frustum.SetFromMatrix(matWorldViewProj); 

	// set some uniform parameters in the fx file
	m_pEffect->SetMatrix("matWorldViewProj", &matWorldViewProj);
	m_pEffect->SetVector("vecLightDir", &m_vLightDir);
	m_pEffect->SetTexture("NormalMap", m_pNormalMap);
}

//----------------------------------------------------------------------------------

void Terrain::OnLostDevice()
{
	if (m_pEffect) {
		m_pEffect->OnLostDevice();
	}
	if (m_pHeightQuery) {
		m_pHeightQuery->OnLostDevice();
	}
	if (m_pVertexBuffer) {
		m_pVertexBuffer->OnLostDevice();
	}
}

//----------------------------------------------------------------------------------

void Terrain::OnDestroyDevice()
{
	if (m_pVertexBuffer) {
		m_pVertexBuffer->OnDestroyDevice();
	}
	if (m_pIndexBuffer) {
		m_pIndexBuffer->OnDestroyDevice();
	}
	if (m_pHeightQuery) {
		m_pHeightQuery->OnDestroyDevice();
	}
	SAFE_RELEASE(m_pVertexTexture);
	SAFE_RELEASE(m_pNormalMap);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
}

//----------------------------------------------------------------------------------

int Terrain::CalculateMaxLevels(int iBlockSize, int iHeightfieldSize)
{
	int iPow2 = (iHeightfieldSize-1) / (iBlockSize-1);
	int iLevels = 0;
	for (int i = 1; i < iPow2; i*=2) {
		iLevels++;
	}
	return iLevels;
}

//----------------------------------------------------------------------------------

HRESULT Terrain::CreateScreenShot()
{
	::CreateDirectory(L"Screenshots", NULL);

	int i = 0;
	WCHAR szPath[MAX_PATH];
	while (1) {
		swprintf(szPath, L"Screenshots\\GPUTerrain%03d.TGA", i++);
		if (GetFileAttributes(szPath) == 0xffffffff) {
			break;
		}
	}

	D3DDISPLAYMODE disp;
	if (FAILED(m_pd3dDevice->GetDisplayMode(0, &disp))) {
		return E_FAIL;
	}
	IDirect3DSurface9 *pSurface=0;
	if (FAILED(m_pd3dDevice->CreateOffscreenPlainSurface(disp.Width, disp.Height, disp.Format, D3DPOOL_SCRATCH, &pSurface, 0))) {
		return E_FAIL;
	}
	if (FAILED(m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface))) {
		SAFE_RELEASE(pSurface);
		return E_FAIL;
	}
	if (FAILED(D3DXSaveSurfaceToFile(szPath, D3DXIFF_TGA, pSurface, 0, 0))) {
		SAFE_RELEASE(pSurface);
		return E_FAIL;
	}
	
	SAFE_RELEASE(pSurface);
	return S_OK;
}
