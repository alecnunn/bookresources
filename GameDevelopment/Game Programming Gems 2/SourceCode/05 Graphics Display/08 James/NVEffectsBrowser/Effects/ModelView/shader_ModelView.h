/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\ModelView
File:  ModelView.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef __SHADER_MODELVIEW_H
#define __SHADER_MODELVIEW_H
#include "MouseUI.h"
#include <string>

class CShaderModelView : public EBEffect
{
public:	
	CShaderModelView();
	~CShaderModelView();

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

	HRESULT UpdateMeshMaterials();
	
	
private:
	//our matrices
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProjection;
	
	DWORD m_dwDirectionalLightShader;
	DWORD m_dwDirectionalLightBumpShader;
	std::string m_strFileName;

	bool m_bHasTexture;
	bool m_bWireframe;
	bool m_bDirectionalBump;
	bool m_bPause;
	MouseUI* m_pUI;

	nv_objects::NVComplexMesh* m_pMesh;

	virtual HRESULT LoadMesh(const std::string& strFileName);
	virtual HRESULT SetVertexShaderMatrices();

};

#endif