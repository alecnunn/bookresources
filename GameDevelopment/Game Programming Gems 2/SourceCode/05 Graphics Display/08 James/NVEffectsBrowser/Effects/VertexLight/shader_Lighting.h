/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __SHADER_LIGHTING_H
#define __SHADER_LIGHTING_H

#include "MouseUI.h"

typedef enum tagLightingType
{
	LIGHTTYPE_POINT = 0,
	LIGHTTYPE_DIRECTIONAL,
	LIGHTTYPE_MANYPOINT,
	LIGHTTYPE_TWOSIDE,
} LightType;

// A special vertex with a face normal
class SphereVertex
{
public:
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 FaceNormal;
};

class CShaderLighting;

// Fog device overrides
class NVLightingDevice : public nv_objects::NVDevice
{
public:
	NVLightingDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderLighting* pLighting = NULL) 
		: NVDevice(pDev),
			m_pLightingShader(pLighting)
	{ };

	// Override setmaterial to apply light colors
	virtual HRESULT SetMaterial(D3DMATERIAL8* pMat);

	CShaderLighting* m_pLightingShader;
};

class CShaderLighting : public EBEffect
{
public:
	CShaderLighting(LightType Type);
	~CShaderLighting();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void UpdateProperties();
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);


protected:
	friend NVLightingDevice;
	NVLightingDevice* m_pNVDevice;

	HRESULT SetTransform();
	HRESULT GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius, WORD wNumRings, WORD wNumSections, FLOAT sx, FLOAT sy, FLOAT sz);

	nv_objects::NVMesh* m_pShapeMesh;
	nv_objects::NVMesh*	m_pLightMesh;

	MouseUI* m_pUI;
	
	IDirect3DVertexBuffer8* m_pSphereBuffer;
	DWORD m_dwSphereFaces;
	DWORD m_dwCurrentShader;
	DWORD m_dwFixedColorShader;
	DWORD m_dwNumLights;

	// The light we are currently drawing
	DWORD m_dwCurrentLightDraw;	

	// Light colors
	std::vector<D3DXCOLOR> m_LightColorDiffuse;
	std::vector<D3DXCOLOR> m_LightColorSpecular;
	std::vector<D3DXCOLOR> m_LightColorAmbient;
	
	LightType m_LightType;
	
	bool m_bWireframe;
	bool m_bPause;

	float m_fAngle;
};

class CShaderPointLight : public CShaderLighting
{
public:
	CShaderPointLight()
		: CShaderLighting(LIGHTTYPE_POINT)
	{
		m_strEffectName = "Point Light";
		m_strEffectLocation = "Vertex Shaders\\Per-Vertex Lighting";
		m_strEffectPixelShader = "";
		m_strEffectVertexShader = GetFilePath("PointLight.nvv");
	}

	virtual void UpdateProperties()
	{
		CShaderLighting::UpdateProperties();
		m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Point Light", GetFilePath("PointLight.nvv"), EBTYPE_STRING_PROP));
	}
};

class CShaderDirectionalLight : public CShaderLighting
{
public:
	CShaderDirectionalLight()
		: CShaderLighting(LIGHTTYPE_DIRECTIONAL)
	{
		m_strEffectName = "Directional Light";
		m_strEffectLocation = "Vertex Shaders\\Per-Vertex Lighting";
		m_strEffectPixelShader = "";
		m_strEffectVertexShader = GetFilePath("DirectionalLight.nvv");
	}

	virtual void UpdateProperties()
	{
		CShaderLighting::UpdateProperties();
		m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Directional Light", GetFilePath("DirectionalLight.nvv"), EBTYPE_STRING_PROP));
	}

};

class CShaderTwoSideLight : public CShaderLighting
{
public:
	CShaderTwoSideLight()
		: CShaderLighting(LIGHTTYPE_TWOSIDE)
	{
		m_strEffectName = "Two-Sided Light";
		m_strEffectLocation = "Vertex Shaders\\Per-Vertex Lighting";
		m_strEffectPixelShader = "";
		m_strEffectVertexShader = GetFilePath("TwoSide.nvv");
	}

	virtual void UpdateProperties()
	{
		CShaderLighting::UpdateProperties();
		m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Two-Side lighting (tri-lists)", GetFilePath("TwoSide.nvv"), EBTYPE_STRING_PROP));
	}

};

class CShaderManyPointLight : public CShaderLighting
{
public:
	CShaderManyPointLight()
		: CShaderLighting(LIGHTTYPE_MANYPOINT)
	{
		m_strEffectName = "Many Point Lights";
		m_strEffectLocation = "Vertex Shaders\\Per-Vertex Lighting";
		m_strEffectPixelShader = "";
		m_strEffectVertexShader = GetFilePath("ManyPoint.nvv");
	}

	virtual void UpdateProperties()
	{
		CShaderLighting::UpdateProperties();
		m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Simple Point Lights", GetFilePath("ManyPoint.nvv"), EBTYPE_STRING_PROP));

	}

};

#endif // __SHADER_LIGHTING_H