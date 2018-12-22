/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __SHADER_DOT3_DIRECTIONAL_H
#define __SHADER_DOT3_DIRECTIONAL_H

#include "shader_Dot3_Util.h"
#include "MouseUI.h"

class CShaderDot3Directional;

// Fog device overrides
class NVDot3DirectionalDevice : public nv_objects::NVDevice
{
public:
	NVDot3DirectionalDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderDot3Directional* pDot3 = NULL)
		: NVDevice(pDev),
			m_pDot3Shader(pDot3)
	{ };

	CShaderDot3Directional* m_pDot3Shader;
};

typedef enum tageDisplayOptions
{
	DISPLAY_BUMPMAP = 0,
	DISPLAY_LIGHTVECTOR = 1,
	DISPLAY_BASETEXTURE = 2,
	DISPLAY_RESULT =3,
	DISPLAYOPTIONS_FORCEDWORD = 0xFFFFFFFF
} eDisplayOptions;

class CShaderDot3Directional : public EBEffect
{
public:
	CShaderDot3Directional();
	~CShaderDot3Directional();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehaviour, D3DFORMAT Format);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);


protected:
	friend NVDot3DirectionalDevice;
	NVDot3DirectionalDevice* m_pNVDevice;

	HRESULT SetTransform();
	HRESULT GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius, WORD wNumRings, WORD wNumSections, FLOAT sx, FLOAT sy, FLOAT sz);
	HRESULT DrawDirectionalLightObject(D3DXVECTOR3 vecLightDirection);
	HRESULT DrawDirectional(D3DXVECTOR3 vecLightDirection);

	nv_objects::NVMesh*	m_pLightMesh;
	MouseUI* m_pUI;
	bool m_bWireframe;
	bool m_bPause;
	
	LPDIRECT3DTEXTURE8 m_pBumpMap;
	LPDIRECT3DTEXTURE8 m_pBaseTexture;

	IDirect3DVertexBuffer8* m_pSphereVertexBuffer;
	IDirect3DIndexBuffer8* m_pSphereIndexBuffer;
	
	DWORD m_dwNumFaces;
	DWORD m_dwNumVertices;
	DWORD m_dwNumIndices;
	
	DWORD m_dwDot3Shader;
	DWORD m_dwLightShader;

	eDisplayOptions m_eDisplayOption;
	float m_fAngle;
};


#endif // __SHADER_DOT3_DIRECTIONAL_H