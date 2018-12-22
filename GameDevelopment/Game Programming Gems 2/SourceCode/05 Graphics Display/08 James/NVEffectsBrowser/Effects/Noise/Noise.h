/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
      
        
******************************************************************************/
#ifndef __NOISE_H
#define __NOISE_H
#include "mouseui.h"

struct GridVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
};

#define MAX_VKEY 256
class CShaderNoise : public EBEffect
{
public:
	CShaderNoise();
	~CShaderNoise();

    virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void Keyboard(DWORD vKey, UINT nFlags, bool bDown);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);

private:
	float						m_fFrequency;
	float						m_fAmplitude;
	void						PermuteTable();
    DWORD                       m_dwCurrentShader;
	LPDIRECT3DVERTEXBUFFER8		m_pVertexBuffer;
	bool						m_bWireFrame;
	DWORD                       m_dwGridSize;
	float                       m_fGridStep;
	float                       m_fGridExtent;
	MouseUI*					m_pUI;

};


#endif //__NOISE_H