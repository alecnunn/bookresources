/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __SHADER_DOT3_POINT_H
#define __SHADER_DOT3_POINT_H

#include "shader_Dot3_Util.h"
#include "MouseUI.h"

class CShaderDot3Point;

// Fog device overrides
class NVDot3PointDevice : public nv_objects::NVDevice
{
public:
	NVDot3PointDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderDot3Point* pDot3 = NULL)
		: NVDevice(pDev),
			m_pDot3Shader(pDot3)
	{ };

	CShaderDot3Point* m_pDot3Shader;
};

typedef enum tagePointDisplayOptions
{
	POINTDISPLAY_LIGHTATTENUATION = 0,
	POINTDISPLAY_LIGHTBUMP = 1,
	POINTDISPLAY_RESULT = 2,
	POINTDISPLAY_LIGHTBUMPBASE = 3,
	POINTDISPLAY_BUMPMAP = 4,
	POINTDISPLAY_BASETEXTURE = 5,
	POINTDISPLAY_TEXTURESPACELIGHTVECTOR = 6,
	POINTDISPLAYOPTIONS_FORCEDWORD = 0xFFFFFFFF
} ePointDisplayOptions;

#define NUM_DOT3_POINTLIGHTS 1
class CShaderDot3Point : public EBEffect
{
public:
	CShaderDot3Point();
	~CShaderDot3Point();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

protected:
	friend NVDot3PointDevice;
	NVDot3PointDevice* m_pNVDevice;

	HRESULT SetTransform();
	HRESULT GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius, WORD wNumRings, WORD wNumSections, FLOAT sx, FLOAT sy, FLOAT sz);
	HRESULT DrawPointLightObjects();
	HRESULT DrawPoints();
	HRESULT FixAlpha(LPDIRECT3DTEXTURE8 pTexture);

	nv_objects::NVMesh*	m_pLightMesh;

	MouseUI* m_pUI;
	bool m_bWireframe;
	bool m_bPause;
	
	LPDIRECT3DTEXTURE8 m_pBumpMap;
	LPDIRECT3DTEXTURE8 m_pBaseTexture;
	LPDIRECT3DTEXTURE8 m_pAttenuate2D;
	LPDIRECT3DCUBETEXTURE8 m_pNormalizationCubeMap;

	LPDIRECT3DVERTEXBUFFER8 m_pSphereVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 m_pSphereIndexBuffer;
	
	DWORD m_dwNumFaces;
	DWORD m_dwNumVertices;
	DWORD m_dwNumIndices;
	
	DWORD m_dwDot3PointLightAttenuationShader;
	DWORD m_dwDot3PointLightBumpShader;
	DWORD m_dwDot3DirectionalLightBumpShader;
	DWORD m_dwLightShader;
	DWORD m_dwTransformShader;

	D3DXVECTOR4 m_LightPosition[NUM_DOT3_POINTLIGHTS];
	D3DXVECTOR4 m_LightColor[NUM_DOT3_POINTLIGHTS];

	ePointDisplayOptions m_eDisplayOption;
	float m_fAngle;
	float m_fDuration;
};


#endif // __SHADER_DOT3_POINT_H