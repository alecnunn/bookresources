/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
      
        
******************************************************************************/
#ifndef __SHADER_SINEWAVE_H
#define __SHADER_SINEWAVE_H

#include "MouseUI.h"


struct SineVertex
{
	D3DXVECTOR2 Position;
};

class CShaderSineWave : public EBEffect
{
public:
	CShaderSineWave();
	~CShaderSineWave();

    virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
    virtual void    MouseMove(HWND hWnd, int x, int y);
	virtual void    MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);

private:
	LPDIRECT3DCUBETEXTURE8		m_pCubeTexture;
	LPDIRECT3DVERTEXBUFFER8		m_pVertexBuffer;

    DWORD                       m_dwCurrentShader;
	DWORD                       m_dwGridSize;
	float                       m_fGridStep;
	float                       m_fGridExtent;
	bool                        m_bMipMap;
	bool                        m_bShowNormals;
    bool                        mbWireFrame;
    MouseUI                    *mpMouseUI;

};


#endif //__SHADER_SINEWAVE_H