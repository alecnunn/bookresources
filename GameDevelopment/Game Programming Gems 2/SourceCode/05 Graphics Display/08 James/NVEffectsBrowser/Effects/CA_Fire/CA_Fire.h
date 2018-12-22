/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_Fire
File:  CA_Fire.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#ifndef __CA_FIRE_EBEFFECT_GJ_H
#define __CA_FIRE_EBEFFECT_GJ_H

#include "MouseUI.h"


class CA_Fire : public EBEffect
{
public:
    typedef enum 
    {
        FIRST_EFFECT_OPTION = 0,
	    EFFECT_FIRE_01 = 0,
        NUM_EFFECT_OPTIONS
    } eFilterOptions;

public:
	CA_Fire( eFilterOptions initDisplayOption );
	~CA_Fire();

    virtual void    UpdateProperties();
	virtual void    PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);

	virtual void	MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void	MouseMove(HWND hWnd, int x, int y);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

	void LoadRulesAndOtherMaps();

	HRESULT		DoSingleTimeStep_3Pass();	// single step with 1 less pass

	HRESULT		SetPixelShader( DWORD dwHandle );
	HRESULT		SetVertexShader( DWORD dwHandle );

	HRESULT		SetStep_Average_4_Nearest(	IDirect3DSurface8 * pTarget,
											IDirect3DTexture8 * pSrcTex  );
	HRESULT		SetStep_Average_4_Diagonal(	IDirect3DSurface8 * pTarget,
											IDirect3DTexture8 * pSrcTex  );

	float		m_fPerTexelWidth;
	float		m_fPerTexelHeight;

	int			m_nSlowDelay;


	MouseUI * 	m_pUI;


private:
    HRESULT CreateTextureRenderTarget();
    void    CreateAndWriteUVOffsets(int width, int height);

private:
    typedef struct tagQuadVertex
    {
	    D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
    } QuadVertex;

    enum 
    {
        kMaxNumTargets = 4,
    };
	
	enum	RenderMode
	{
		FULLSCREEN_RESULT,
		FULLSCREEN_NEIGHBOR_CALC,
		ALL_TOGETHER,
		FULLSCREEN_FINALOUT
	};

private:
    eFilterOptions           meDisplayOption;
    bool                     m_bWireframe;		// only affect final rendering
												// not render to texture steps

	//////////////////////////////////////

	nv_objects::NVTexture	* m_pInitialStateTexture;
	nv_objects::NVTexture	* m_pRulesTexture;

	nv_objects::NVTexture	* m_pOutputMapTexture;

	//////////////////////////////////////
	
	DWORD			m_dwCurrentPixelShader;
	DWORD			m_dwCurrentVertexShader;

	//////////////////////////////////////

	DWORD					m_dwVertexShader;
	DWORD					m_dwEqualWeightCombineShader;
	DWORD					m_dwEqualWeight_PostMultShader;

	DWORD					m_dwDependentGB;
	DWORD					m_dwDependentAR;

	bool					m_bReset;
	RenderMode				m_eRenderMode;

	bool					m_bAnimate;
	bool					m_bSingleStep;
	bool					m_bSlow;
	bool					m_bFarther;

	bool					m_bMulticolor;


	bool					m_bWrap;

	int						m_nSkipInterval;
	int						m_nFlipState;

	float					m_fColorAtten;
	float					m_fTranslateAmount;


	LPDIRECT3DVERTEXBUFFER8  m_pVertexBuffer;

    IDirect3DSurface8       *mpBackbufferColor; 
    IDirect3DSurface8       *mpBackbufferDepth; 

    IDirect3DTexture8       *mpTextureFiltered[kMaxNumTargets];       
    IDirect3DSurface8       *mpFilterTarget   [kMaxNumTargets]; 

	///////////////////////////////////////////////////////////
	// State pointers to mark source and dest textures
	//  on each pass.
	// Do not free or allocate to these - they are just copies
	//  of pointers elsewhere.
	IDirect3DTexture8 * m_pTexSrc;

	IDirect3DTexture8 * m_pIntermediateSrc;
	IDirect3DTexture8 * m_pTexFinalSrc;

	IDirect3DSurface8 * m_pTexFinalTarg;
	IDirect3DSurface8 * m_pIntermediateTarg;

	// these two are for a final lookup into colormap for the output
	IDirect3DTexture8 * m_pOutputSrc;
	IDirect3DSurface8 * m_pOutputTarg;

	IDirect3DTexture8 * m_pOutSrc2;

	int	m_nTarget4;
//	int	m_nSource4;

	int	m_nTarget3;
//	int	m_nSource3;


	///////////////////////////////////////////////////////////

};

#endif