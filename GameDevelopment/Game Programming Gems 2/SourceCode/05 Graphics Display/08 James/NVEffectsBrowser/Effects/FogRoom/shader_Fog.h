/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __SHADER_FOG_H
#define __SHADER_FOG_H

#define MAX_VKEY 256

#include "NVWalkCamera.h"

class WorldBoxVertex
{
public:
	WorldBoxVertex(const D3DXVECTOR3& vecPosition, const D3DXVECTOR2& vecTexture)
		: m_vecPosition(vecPosition),
			m_vecTexture(vecTexture)
	{};

	D3DXVECTOR3 m_vecPosition;
	D3DXVECTOR2 m_vecTexture;
};

class FogVertex
{
public:
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

#define FOGVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#define NUM_CHAIRS 8

class CShaderFog;

// Fog device overrides
class NVFogDevice : public nv_objects::NVDevice
{
public:
	NVFogDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderFog* pFogShader = NULL)
		: NVDevice(pDev),
			m_pFogShader(pFogShader)
	{ };

	virtual HRESULT SetVertexShader(DWORD dwVertexShader);
	virtual HRESULT SetMaterial(D3DMATERIAL8* pMat);

	// Device API
	virtual void SetWorldTransform(const D3DXMATRIX* pWorld);

	CShaderFog* m_pFogShader;
};

class CShaderFog : public EBEffect
{
public:
	CShaderFog();
	~CShaderFog();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);


	// Render callbacks
	HRESULT SetVertexShader(DWORD dwVertexShader);

	// Overrides
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);

private:
	friend NVFogDevice;

	HRESULT SetTransform();
	
	HRESULT DrawRoom();
	HRESULT DrawWorldBox();

	HRESULT CreateCube(WorldBoxVertex* pVertices, WORD* pIndices);
	HRESULT BuildFogMap();
	HRESULT DrawFogMap();

	bool m_bKey[MAX_VKEY];

	bool m_bShowFogMap;
	bool m_bEnableFog;
	bool m_bWireframe;

	D3DXVECTOR3 m_CameraLookDirection;

	D3DXVECTOR3 m_vecRoomDimensions;
	D3DXVECTOR3 m_vecChairDimensions;
	D3DXVECTOR3 m_vecTableDimensions;

	DWORD m_dwFogShader;
	DWORD m_dwWorldShader;
	DWORD m_dwCurrentShader;

	DWORD m_dwFogMapWidth;
	DWORD m_dwFogMapHeight;

	RECT m_WinRect;
	
	nv_objects::NVFile* m_pRoom;
	nv_objects::NVBounds m_RoomBounds;

	nv_objects::NVFile* m_pChair;
	nv_objects::NVBounds m_ChairBounds;

	nv_objects::NVFile* m_pTable;
	nv_objects::NVBounds m_TableBounds;

	D3DXVECTOR4 m_vecChairs[NUM_CHAIRS];

	LPDIRECT3DVERTEXBUFFER8 m_pWorldBoxVertices;
	LPDIRECT3DINDEXBUFFER8 m_pWorldBoxIndices;

	LPDIRECT3DVERTEXBUFFER8 m_pFogMapVB;
	LPDIRECT3DTEXTURE8 m_pWorldTextures[6];
	LPDIRECT3DTEXTURE8 m_pFogMap;

	nv_objects::NVWalkCamera* m_pCamera;

	NVFogDevice* m_pNVDevice;
};
#endif // _SHADER_FOG_H