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

class CShaderReflect : public EBEffect
{
public:
	CShaderReflect();
	~CShaderReflect();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

private:
	bool m_bMipMap;
	bool m_bWireframe;
	MouseUI* m_pUI;
	nv_objects::NVDevice* m_pNVDevice;
	nv_objects::NVMesh* m_pTeapot;
	LPDIRECT3DCUBETEXTURE8 m_pCubeTexture;
	DWORD m_dwCurrentShader;
};

#endif