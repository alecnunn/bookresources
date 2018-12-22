/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\Aniso
File:  shader_Aniso.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef SHADER_Aniso_H
#define SHADER_Aniso_H
#include "MouseUI.h"
#include <string>

struct AnisoVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u0;         //texcoord for stage 0
	float u1;         //texcoord for stage 1
	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE1( 0 ) | D3DFVF_TEXCOORDSIZE1( 1 )
	};
};

typedef enum tageAnisoDisplayOptions
{
	AnisoDisplay_SILHOUETTES = 0,
	AnisoDisplay_SHADING = 1,
	AnisoDisplay_SILHOUETTESANDSHADING = 2
} eAnisoDisplayOptions;

class CShaderAniso : public EBEffect
{
public:	
	CShaderAniso();
	~CShaderAniso();

	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten );

	virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	
private:

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pIndexBuffer;

	LPDIRECT3DTEXTURE8 m_pShadeTexture;
	LPDIRECT3DTEXTURE8 m_pEdgeTexture;
	
	D3DXATTRIBUTERANGE *m_pAttributes;
	
	//our matrices
	D3DXMATRIX m_world;
	D3DXMATRIX m_view;
	D3DXMATRIX m_proj;
	
	//number of sections in the mesh
	DWORD m_dwNumSections;
	
	DWORD m_dwCurrentShader;

	std::string m_strFileName;

	bool m_bWireframe;
	bool m_bUseTrilinear;
	MouseUI* m_pUI;

	eAnisoDisplayOptions m_eDisplayOption;
	
	HRESULT LoadXFile(const char* fileName, const DWORD dwFVF);
	HRESULT SetVertexShaderMatrices();
	HRESULT SetTextureStageStates();
	
};

#endif