/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\ReflectiveBumpDynamic
File:  Effect_ReflectiveBumpDynamic.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef __EFF_REFLBUMPDYNAMIC_GJ
#define __EFF_REFLBUMPDYNAMIC_GJ

#include "MouseUI.h"


///////////////////////////////
// external:

class CA_Water;


///////////////////////////////

class NVEffect_DynamicReflection;


///////////////////////////////


// Fog device overrides
class NVEffectDevice : public nv_objects::NVDevice
{
public:
	NVEffectDevice(LPDIRECT3DDEVICE8 pDev = NULL, NVEffect_DynamicReflection* pDot3 = NULL)
		: NVDevice(pDev),
			m_pEffect( pDot3 )
	{ };

	NVEffect_DynamicReflection* m_pEffect;
};

typedef enum tageBlinnDisplayOptions
{
	DISPLAY_BLINN8BITSIGNED = 0,
	DISPLAY_BLINNHILOSIGNED = 1,
	DISPLAY_BLINNFORCEDWORD = 0xFFFFFFFF
} eBlinnDisplayOptions;



class NVEffect_DynamicReflection : public EBEffect
{
public:
	NVEffect_DynamicReflection();
	~NVEffect_DynamicReflection();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);

	void	TryDrawDroplet( float x, float y );
	void	RestoreRenderState();


protected:
	friend NVEffectDevice;
	NVEffectDevice* m_pNVDevice;

	HRESULT SetTransform();
	HRESULT GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius, WORD wNumRings, WORD wNumSections, FLOAT sx, FLOAT sy, FLOAT sz);
	HRESULT GenerateQuad(D3DXVECTOR3& vCenter, FLOAT fRadius);
    HRESULT GenerateSplinePatch();
    HRESULT AnimateSplinePatch(EBTimer* pTimer);
    void    ElevateToCubicMesh(D3DXVECTOR3 *pControlMesh, unsigned int xOrder, unsigned int yOrder) const;
    void    ElevateToCubicMesh(D3DXVECTOR4 *pControlMesh, unsigned int xOrder, unsigned int yOrder) const;

	HRESULT DrawReflection();


	CA_Water *	m_pCA_Water;		// class generating procedural normal map
									//   animation

	bool m_bWireframe;

	MouseUI* m_pUI;
	
	bool	m_bShowProceduralMaps;

	bool	m_bDrawing;			// drawing on while dragging
	bool	m_bDrawModeOn;

	LPDIRECT3DTEXTURE8 m_pBumpMapQWVU;
	LPDIRECT3DCUBETEXTURE8 m_pCubeTexture;
	
	LPDIRECT3DVERTEXBUFFER8 m_pPatchBuffer;
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
    LPDIRECT3DINDEXBUFFER8  m_pIndexBuffer;
	
	DWORD m_dwNumFaces;
	DWORD m_dwNumVertices;
	DWORD m_dwNumIndices;
	
	eBlinnDisplayOptions m_eDisplayOption;
	float m_fAngle;

	bool m_bSupportsQWVU;
	bool m_bSupportsHILO;

    bool m_bUsePatch;
	bool m_bSupportsPatches;
	bool m_bPause;

	DWORD m_dwBlinnPixelShader;
	DWORD m_dwBlinnVertexShader;
	DWORD m_dwTransformShader;
    DWORD m_dwBlinnPatchVertexShader;
	DWORD m_dwTransformPatchShader;

	// Control points for the patch
	float m_ctrl1;
	float m_ctrl2;
	float m_ctrl3;
	float m_ctrl4;

	float m_fBumpScale;
};


#endif		 // __EFF_REFLBUMPDYNAMIC_GJ
