/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\BRDF
File:  shader_BRDF.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef SHADER_BRDF_H
#define SHADER_BRDF_H
#include "MouseUI.h"

#define NUM_OBJECTS 2

typedef enum ROTATEENUM {
	WOMAN_OBJECT = 0,
	LIGHT_OBJECT = 1
} ROTATEENUM;

char *LoadObjects[NUM_OBJECTS] = { "woman.x",
								  "dirLight.x"
								};

//BRDFVertex
struct TSVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture0;         //texcoord for stage 0
};

#define TSVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))

typedef enum tageBRDFDisplayOptions
{
	BRDFDISPLAY_MINNAERT = 0,
	BRDFDISPLAY_SHADING = 1,
	BRDFDISPLAY_SILHOUETTESANDSHADING = 2
} eBRDFDisplayOptions;

class CShaderBRDF : public EBEffect
{
public:	
	CShaderBRDF();
	~CShaderBRDF();
	
	virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	
private:
	LPDIRECT3DVERTEXBUFFER8	m_pVertexBuffer[NUM_OBJECTS];
	LPDIRECT3DINDEXBUFFER8  m_pIndexBuffer[NUM_OBJECTS];
	D3DXVECTOR3				m_ObjectCenter[NUM_OBJECTS];
	float					m_ObjectRadius[NUM_OBJECTS];
	D3DXATTRIBUTERANGE		*m_pAttributes[NUM_OBJECTS];
	ID3DXBuffer				*m_pMaterials[NUM_OBJECTS];
	D3DXMATRIX				m_MatObject[NUM_OBJECTS];
	DWORD					m_dwCurrentVShader[NUM_OBJECTS];
	DWORD					m_dwCurrentPShader[NUM_OBJECTS];
	//number of sections in the mesh
	DWORD					m_dwNumSections[NUM_OBJECTS];
	int						m_SaveMouse_X[NUM_OBJECTS];
	int						m_SaveMouse_Y[NUM_OBJECTS];
	bool					m_SaveMouse_bDown[NUM_OBJECTS];

	LPDIRECT3DTEXTURE8 m_pWomanTexture;
	LPDIRECT3DCUBETEXTURE8 m_pNormalizingTexture;
	LPDIRECT3DTEXTURE8 m_pMinnaertTexture;
	LPDIRECT3DTEXTURE8 m_pVelvetBumpMapTexture;

	LPDIRECT3DVERTEXBUFFER8 m_pTexture2DVB;
	LPDIRECT3DVERTEXBUFFER8 m_pTestQuadVB;
	
	double m_MinnaertPower;
	
	D3DVIEWPORT8 m_D3DViewPort;

	//our matrices
	D3DXMATRIX m_MatWorld;
    D3DXMATRIX m_MatView;
	D3DXMATRIX m_MatProj;
	
	D3DXVECTOR3 m_vEyePt;
	D3DXVECTOR3 m_vLookatPt;
	D3DXVECTOR3 m_vUp;

	D3DXVECTOR3 m_LightDir;
	
	ROTATEENUM	m_Rotating;
	ROTATEENUM	m_LastRotating;
	bool		m_bAutoRotate;
	float		m_curYaw;
	float		m_curPitch;
	D3DXVECTOR3 m_VRotation;

	bool m_bWireframe;
	bool m_bUseTrilinear;
	bool m_bSilhouettes;
	bool m_bShading;
	bool m_bShadingSilhouettes;
	MouseUI* m_mUI;

	eBRDFDisplayOptions m_eDisplayOption;
	
	HRESULT LoadXFile(const char* fileName, const DWORD dwFVF, DWORD index);
	HRESULT SetVertexShaderMatrices(DWORD index);
	HRESULT SetTextureStageStates(DWORD, DWORD);
	HRESULT Create2DBitmap(DWORD x, DWORD y, DWORD width, DWORD height);
	HRESULT Draw2DBitmap();
	HRESULT CreateTestQuad();
	void SetupFixedCamera(float radius);
	void UpdateObject(DWORD index);
	void SetLightPosition();

	
};

#endif