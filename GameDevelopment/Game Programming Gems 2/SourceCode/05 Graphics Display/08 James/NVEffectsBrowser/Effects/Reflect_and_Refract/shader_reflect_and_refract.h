/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __SHADER_REFLECT_H
#define __SHADER_REFLECT_H

#include "MouseUI.h"
#include <string>

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

class CShaderReflectRefract : public EBEffect
{
public:
	CShaderReflectRefract();
	~CShaderReflectRefract();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

private:
	HRESULT DrawWorldBox();
	HRESULT CreateCube(WorldBoxVertex* pVertices, WORD* pIndices);

	bool m_bMipMap;
	bool m_bWireframe;
	nv_objects::NVDevice* m_pNVDevice;
	nv_objects::NVMesh* m_pSphere;
	LPDIRECT3DCUBETEXTURE8 m_pCubeTexture;
	DWORD m_dwCurrentShader;
	MouseUI* m_pUI;
	float m_fRefraction;

	LPDIRECT3DVERTEXBUFFER8 m_pWorldBoxVertices;
	LPDIRECT3DINDEXBUFFER8 m_pWorldBoxIndices;

	LPDIRECT3DTEXTURE8 m_pWorldTextures[6];

};

#endif