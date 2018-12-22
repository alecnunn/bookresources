/*************************************************************************
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\CubeMapLighting
File:  CubeMapLighting.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef CUBEMAPLIGHTING_H
#define CUBEMAPLIGHTING_H
#include "MouseUI.h"

typedef enum OBJECTNUMS {
	TERRAIN_OBJECT = 0,
	SPHERE_OBJECT = 1,
	NUM_OBJECTS
} OBJECTNUMS;


#define CHECKHRESULT(x)							\
{												\
	DWORD hr;									\
	hr = (x);									\
	if (hr != D3D_OK)							\
	{											\
		TCHAR errorString[256];					\
		TCHAR message[1024];					\
		D3DXGetErrorString(hr, errorString, 256); \
		wsprintf(message, "D3D Failure!\nError #: %d\nError: %s", hr, errorString); \
		MessageBox(0, message, "DX8 Error!", MB_OK); \
		return hr;								\
	}											\
}	

struct TSVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture0;         //texcoord for stage 0
};

#define TSVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))

class CCubeMapLighting : public EBEffect
{
public:	
	CCubeMapLighting();
	~CCubeMapLighting();
	
	virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void Resize(HWND hWnd, int w, int h, bool fUpdateViewport);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);

	HRESULT SetVertexShaderGlobalConstants(D3DXMATRIX &world);
	HRESULT RenderTerrain(EBTimer *pTimer);
	HRESULT RenderSkySphere(EBTimer *pTimer);

private:
	bool		m_Stepping;

	D3DVIEWPORT8 m_D3DViewPort;

	//our matrices
	D3DXMATRIX m_MatTerrain;
    D3DXMATRIX m_MatView;
	D3DXMATRIX m_MatProj;
	
	D3DXVECTOR3 m_vEyePt;
	D3DXVECTOR3 m_vLookatPt;
	D3DXVECTOR3 m_vUp;
	bool m_bWireframe;
	LPDIRECT3DVERTEXBUFFER8	m_pVertexBuffer[NUM_OBJECTS];	// objects vertex buffers.
	LPDIRECT3DINDEXBUFFER8	m_pIndexBuffer[NUM_OBJECTS];	//  "      index buffers.
	D3DXVECTOR3				m_ObjectCenter[NUM_OBJECTS];
	float					m_ObjectRadius[NUM_OBJECTS];
	D3DXATTRIBUTERANGE		*m_pAttributes[NUM_OBJECTS];
	DWORD					m_dwNumSections[NUM_OBJECTS];
	ID3DXBuffer				*m_pMaterials[NUM_OBJECTS];
	D3DXMATRIX				m_MatSphereRot;

	DWORD m_dwDirLightVShader;
	DWORD m_dwTerrainPShader;
	DWORD m_dwSphereVShader;
	DWORD m_dwSpherePShader;
	LPDIRECT3DTEXTURE8 m_pDecalTexture;	// decal texture texture pointer
	LPDIRECT3DCUBETEXTURE8 m_pCloudTexture;	// cloud texture pointer

	MouseUI* m_mUI;

	void SetupFixedCamera(float x, float y, float radius);
	HRESULT LoadXFile(const char* fileName, const DWORD dwFVF, DWORD index);

};

#endif
