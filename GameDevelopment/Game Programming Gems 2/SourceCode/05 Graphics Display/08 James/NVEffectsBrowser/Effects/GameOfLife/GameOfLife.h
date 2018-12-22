/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\GameOfLife
File:  GameOfLife.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#ifndef __FILTER_BLIT_H
#define __FILTER_BLIT_H

class CGameOfLife : public EBEffect
{
public:
    typedef enum 
    {
        FIRST_EFFECT_OPTION = 0,
	    GAME_OF_LIFE_EFFECT = 0,
        NUM_EFFECT_OPTIONS
    } eFilterOptions;

public:
	CGameOfLife( eFilterOptions initDisplayOption );
	~CGameOfLife();

    virtual void    UpdateProperties();
	virtual void    PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

	void LoadRulesAndOtherMaps();

	HRESULT		DoSingleTimeStep();			// single step of the game
	HRESULT		DoSingleTimeStep_3Pass();	// single step with 1 less pass


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
        kMaxNumPasses = 4,
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

	nv_objects::NVTexture	* m_pInitialStateTexture;
	nv_objects::NVTexture	* m_pRulesTexture;

	nv_objects::NVTexture	* m_pOutputMapTexture;


	DWORD					m_dwVertexShader;
	DWORD					m_dwEqualWeightCombineShader;

	DWORD					m_dwDependentGB;

	bool					m_bReset;
	RenderMode				m_eRenderMode;

	bool					m_bAnimate;
	bool					m_bSingleStep;
	bool					m_bSlow;
	bool					m_bFarther;

	bool					m_bWrap;

	int						m_nSkipInterval;
	int						m_nFlipState;


	LPDIRECT3DVERTEXBUFFER8  m_pVertexBuffer;

    IDirect3DSurface8       *mpBackbufferColor; 
    IDirect3DSurface8       *mpBackbufferDepth; 

    IDirect3DTexture8       *mpTextureFiltered[kMaxNumPasses];       
    IDirect3DSurface8       *mpFilterTarget   [kMaxNumPasses]; 

	///////////////////////////////////////////////////////////
	// State pointers to mark source and dest textures
	//  on each pass.
	// Do not free or allocate to these - they are just copies
	//  of pointers elsewhere.
	IDirect3DTexture8 * m_pTexSrc;
	IDirect3DTexture8 * m_pIntermediateSrc;

	IDirect3DSurface8 * m_pTexFinalTarg;
	IDirect3DSurface8 * m_pIntermediateTarg;

	// these two are for a final lookup into colormap for the output
	IDirect3DTexture8 * m_pOutputSrc;
	IDirect3DSurface8 * m_pOutputTarg;


	///////////////////////////////////////////////////////////

};

#endif