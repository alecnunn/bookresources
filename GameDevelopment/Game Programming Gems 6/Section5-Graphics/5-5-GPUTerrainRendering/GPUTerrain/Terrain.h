//--------------------------------------------------------------------------------------
// File: Terrain.h
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

#ifndef __TERRAIN_H_
#define __TERRAIN_H_

#include "Frustum.h"

#define MAX_ELEVATION 200

class IndexBuffer;
class VertexBuffer;
class HeightQuery;

class Terrain
{
public:
	Terrain();
	~Terrain();

	HRESULT Initialize(IDirect3DDevice9 *pd3dDevice, int iBlockSize);
	HRESULT OnResetDevice(IDirect3DDevice9 *pd3dDevice);
	bool OnRender();
	void OnFrameMove(double fTime);
	void OnLostDevice();
	void OnDestroyDevice();

	double GetNumTrianglesPerSecond() const { return m_fNumTrisPerSecond; }

	IDirect3DDevice9* GetDevice() const { return m_pd3dDevice; }

	float GetLOD() const { return m_fLOD; }
	void SetLOD(float fLOD) { m_fLOD = fLOD; }

	HRESULT SetBlockSize(int iBlockSize);
	HRESULT CreateScreenShot();

	int GetBlockSize() const { return m_iBlockSize; }
	int GetNumCulled() const { return m_iNumCulled; }
	bool IsCulling() const { return m_bFrustumCulling; }

	void ToggleCulling() { m_bFrustumCulling = !m_bFrustumCulling; }
	void ToggleWireframe() { m_bWireframe = !m_bWireframe; }
	void ToggleNormalConstruction() { m_bNormalInVS = !m_bNormalInVS; }

private:
	HRESULT CheckCapabilities(IDirect3DDevice9 *pd3dDevice);
	HRESULT InitializeEffect();
	HRESULT InitializeHeightfield();

	void Render(float fMinU, float fMinV, float fMaxU, float fMaxV, int iLevel, float fScale);
	bool Draw(float fBiasU, float fBiasV, float fScale, int iLevel);

	static int CalculateMaxLevels(int iBlockSize, int iHeigthfieldSize);

	void AvoidCollision();

private:
	IDirect3DDevice9*				m_pd3dDevice;
	IDirect3DTexture9*				m_pVertexTexture;
	IDirect3DTexture9*				m_pNormalMap;
	ID3DXEffect*					m_pEffect;
	IDirect3DVertexDeclaration9*	m_pDecl;
	D3DCAPS9						m_Caps;
	D3DSURFACE_DESC					m_Desc;
	D3DXVECTOR4						m_vLightDir;
	IndexBuffer*					m_pIndexBuffer;
	VertexBuffer*					m_pVertexBuffer;
	HeightQuery*					m_pHeightQuery;
	Frustum							m_Frustum;
	int								m_iMaxLevels;
	int								m_iBlockSize;
	int								m_iNumTriangles;
	int								m_iTime;
	double							m_fNumTrisPerSecond;
	float							m_fLOD;
	bool							m_bWireframe;
	bool							m_bNormalInVS;
	bool							m_bFrustumCulling;
	int								m_iNumCulled;
};

#endif // __TERRAIN_H_
