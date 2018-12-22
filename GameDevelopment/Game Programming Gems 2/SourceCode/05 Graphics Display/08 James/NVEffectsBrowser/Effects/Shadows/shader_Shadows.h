/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
       
        
******************************************************************************/

#ifndef __SHADER_SHADOWS_H
#define __SHADER_SHADOWS_H

class CShaderShadows;

#include "NVFileDialog.h"
#include <NVMesh.h>
#include "ModelInstance.h"
#include <vector>
#include "MouseUI.h"

class ShadowsVertex
{
public:

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;

	enum FVF_Flags
	{
		FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	};
};

// device overrides
class NVShadowsDevice : public nv_objects::NVDevice
{
public:
	NVShadowsDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderShadows* pShadows = NULL) 
		: NVDevice(pDev),
			m_pShadowsShader(pShadows)
	{ };
	
	CShaderShadows* m_pShadowsShader;
};

class CShaderShadows: public EBEffect
{
public:

	CShaderShadows();
	~CShaderShadows();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);

			HRESULT CreateShadowMap();
			HRESULT CreateShadows();

	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);

private:

	friend class NVShadowsDevice;

	HRESULT	DrawObjects();

	HRESULT	DrawSeaFloor();

	D3DXMATRIX CreateLightMatrix( const unsigned int& i );

	LPDIRECT3DVERTEXBUFFER8 m_pShowShadowMapVB;

	HRESULT SetTransform();

	NVShadowsDevice*     m_pNVDevice;

	MouseUI*				m_pMouseUI;

	enum Constants
	{
		LIGHT_COUNT = 1,
		BACKGROUND_COLOR =    0x0099aaee
	};

	// The floor
	nv_objects::NVMesh*     m_pFloorMesh;

	nv_objects::NVMesh*     m_pObjectMesh;

	NVXFileDialog			m_XFileDialog;

	D3DLIGHT8				m_Lights[ LIGHT_COUNT ];

	std::vector< ModelInstance > m_Models;

	unsigned int			m_ShadowSize;

    LPDIRECT3DTEXTURE8		m_pShadowMap;
    LPDIRECT3DTEXTURE8		m_pShadowGen;
	LPDIRECT3DTEXTURE8		m_pTigerTexture;
    LPDIRECT3DSURFACE8      m_pShadowZBuffer;
     
    DWORD                   m_numVertices;
    DWORD                   m_numIndices;

    DWORD                   m_dwShadowShowShader;
    DWORD                   m_dwShadowGenShader;
    DWORD                   m_dwDrawObjectShader;

    D3DXMATRIX              m_LastWorldTxf;
    float                   m_OneOverExtent;


    bool                    m_bEnableShadows;
    bool                    m_bShowShadowMap;
    bool                    m_bPause;
    bool                    mbWireFrame;
};

#endif //__SHADER_SHADOWS_H
