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


#ifndef SHADER_PALETTESKIN_H
#define SHADER_PALETTESKIN_H
#include "MouseUI.h"
#include "SkinnedMesh.h"
#include <vector>
#define PALETTE_SIZE 15

//PaletteSkinVertex
struct PSVertex
{
	D3DXVECTOR3 Position;
	float weight0, weight1;
	float index1, index2;  //two indices, one for each bone per vertex
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
	D3DXVECTOR3 S;
	D3DXVECTOR3 T;
    D3DXVECTOR3 SxT;
};

struct MeshVertex
{
	D3DXVECTOR3 Position;
	float weight;
	DWORD indices;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

class CShaderPaletteSkin : public EBEffect
{
public:	
	CShaderPaletteSkin();
	~CShaderPaletteSkin();
	
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

	//vertex buffer for rendering
	LPDIRECT3DVERTEXBUFFER8   m_pRenderVB;
	
	LPDIRECT3DTEXTURE8 m_pBumpMapTexture;
	
	//our matrices
	D3DXMATRIX m_world;
	D3DXMATRIX m_view;
	D3DXMATRIX m_proj;
	
	//number of sections in the mesh
	DWORD m_dwNumSections;
	
	DWORD m_dwCurrentShader;
	
	bool m_bWireframe;
	bool m_bAnimateLight;
	bool m_bAddDecal;
	bool m_bPaused;

	float m_fElapsedTime;
	float m_fLastTime;
	
	MouseUI* m_pUI;

	D3DXVECTOR4 m_lightDirection;

	std::vector<DWORD> m_Declaration;

	HRESULT SetVertexShaderMatrices();

	////////////////////////////////////////////////////////////////////////////////////////
	//functions from MSSDK skinned mesh sample...

	SMeshContainer *m_pmcSelectedMesh;
	SFrame *m_pframeSelected;
	SDrawElement *m_pdeSelected;
	SDrawElement *m_pdeHead;

	HRESULT FindBones(SFrame *pframeCur, SDrawElement *pde);
	HRESULT LoadMeshHierarchy(const char* filePath);
	HRESULT LoadAnimationSet(LPDIRECTXFILEDATA pxofobjCur, SDrawElement *pde,
			              DWORD options, SFrame *pframeParent);
	HRESULT LoadAnimation(LPDIRECTXFILEDATA pxofobjCur, SDrawElement *pde,
						  DWORD options, SFrame *pframeParent);
	HRESULT LoadFrames(LPDIRECTXFILEDATA pxofobjCur, SDrawElement *pde,
					DWORD options, SFrame *pframeParent);
	HRESULT LoadMesh(LPDIRECTXFILEDATA pxofobjCur,
				 DWORD options, SFrame *pframeParent);
	HRESULT DeleteSelectedMesh();
	HRESULT DrawMeshContainer(SMeshContainer *pmcMesh);
	HRESULT UpdateFrames(SFrame *pframeCur, D3DXMATRIX &matCur);
	HRESULT GenerateMesh(SMeshContainer* pmcMesh);
	HRESULT DrawFrames(SFrame *pframeCur, UINT &cTriangles);

	HRESULT CreateBasisVectors(LPDIRECT3DVERTEXBUFFER8 pVertexBuffer, LPDIRECT3DINDEXBUFFER8 pIndexBuffer);
	
};

#endif