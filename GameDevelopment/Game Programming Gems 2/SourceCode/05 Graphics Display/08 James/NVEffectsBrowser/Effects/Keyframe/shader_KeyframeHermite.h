/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __SHADER_KEYFRAMEHERMITE_H
#define __SHADER_KEYFRAMEHERMITE_H

#include "md2file.h"

class MouseUI;
struct Extents;
struct KeyframeVertex;

class CShaderKeyframeHermite : public EBEffect
{
public:
	CShaderKeyframeHermite();
	~CShaderKeyframeHermite();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	
private:
	DWORD m_dwCurrentShader;
	Md2Model* m_md2Model;
	int m_numFrames;
	int m_frame0, m_frame1, m_frame2, m_frame3;
	int m_baseTime;
	bool m_bWireframe;

	D3DXMATRIX m_view, m_proj;

	MouseUI* m_pUI;

	void SetupMatrices(const Extents& e);
	void SetMatricesBasedOnInput();
	void SetExtents(const KeyframeVertex& vertex, Extents& e);

	LPDIRECT3DVERTEXBUFFER8* m_pVertexBuffers;
	LPDIRECT3DINDEXBUFFER8 m_pIndexBuffer;
	LPDIRECT3DTEXTURE8 m_pTexture;
};

#endif