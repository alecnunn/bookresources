/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __SHADER_QUAD2_H
#define __SHADER_QUAD2_H
#include "MouseUI.h"

class CShaderQuad2 : public EBEffect
{
public:
	CShaderQuad2();
	~CShaderQuad2();
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void UpdateProperties();
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	
private:
	DWORD m_dwCurrentShader;
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DTEXTURE8 m_pLogoTexture;
	bool m_bWireframe;
	MouseUI* m_pUI;

};

#endif