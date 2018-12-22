/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\ToonShade
File:  shader_ToonShade.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef SHADER_HALO_H
#define SHADER_HALO_H
#include "MouseUI.h"

//Halo Vertex
struct HVertex
{
	float x, y, z;
	float nx, ny, nz;
};

typedef enum tageHaloDisplayOptions
{
	HALO_DISPLAY = 0,
	TOON_DISPLAY = 1,
} eHaloDisplayOptions;

class CShaderHalo : public EBEffect
{
public:	
	CShaderHalo();
	~CShaderHalo();
	
	virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);
	
private:
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 m_pIndexBuffer;
	
	//our matrices
	D3DXMATRIX m_world;
	D3DXMATRIX m_view;
	D3DXMATRIX m_proj;
	
	//number of sections in the mesh
	DWORD m_dwNumSections;
	
	DWORD m_dwNormalShader, m_dwHaloShader, m_dwToonShader;

	DWORD m_dwNumVerts, m_dwNumFaces;
	
	bool m_bWireframe;
	
	MouseUI* m_pUI;

	//bounding sphere attributes
	float m_fRadius;
	D3DXVECTOR3 m_vecCenter;

	//display options
	eHaloDisplayOptions m_eDisplayOption;
	float m_fEdgeSize;
	
	HRESULT LoadXFile(const char* fileName, const DWORD dwFVF);
	HRESULT SetVertexShaderMatrices();
	
};

#endif