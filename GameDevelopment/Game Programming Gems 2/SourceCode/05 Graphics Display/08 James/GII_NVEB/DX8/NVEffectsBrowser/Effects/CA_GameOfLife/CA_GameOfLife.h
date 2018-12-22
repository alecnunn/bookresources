/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_GameOfLife
File:  CA_GameOfLife.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef __CA_GAMEOFLIFE_GJ_H
#define __CA_GAMEOFLIFE_GJ_H


#include "ShaderManager.h"
#include "NV_Common.h"



/////////////////////////////////////////////////////////

class CA_GameOfLife
{
private:
    typedef struct tagQuadVertex
    {
	    D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
    } QuadVertex;

    enum 
    {
        kMaxNumTargets = 2,
    };
	

public:
    typedef enum 
    {
        FIRST_EFFECT_OPTION = 0,
	    EFFECT_FIRE_01 = 0,
        NUM_EFFECT_OPTIONS
    } eFilterOptions;

	enum	RenderMode
	{
		FULLSCREEN_RESULT,
		FULLSCREEN_NEIGHBOR_CALC,
		ALL_TOGETHER,
		FULLSCREEN_FINALOUT,
		DO_NOT_RENDER,
		RM_FORCEDWORD = 0xFFFFFFFF
	};

public:
	CA_GameOfLife();
	~CA_GameOfLife();

    virtual void    UpdateProperties();

	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev, ShaderManager * pManager );


	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick();
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);

    virtual bool	Keyboard(DWORD dwKey, UINT nFlags, bool bDown);


	virtual IDirect3DTexture8 *	GetOutputTexture();


	std::string GetFilePath(const std::string& strFileName);


	void		LoadRulesAndOtherMaps();

	HRESULT		DoSingleTimeStep_3Pass();	// single step with 1 less pass

	HRESULT		SetPixelShader( SHADER_INDEX dwIndex );
	HRESULT		SetVertexShader( SHADER_INDEX dwIndex );


	float		m_fPerTexelWidth;
	float		m_fPerTexelHeight;

	int			m_nSlowDelay;

	ShaderManager	*	m_pShaderManager;

    bool		m_bWireframe;		// only affects final rendering
									//   not render to texture steps
	bool		m_bReset;
	bool		m_bOccasionalExcitation;  // occasionaly offset and add cell field
										  //   to itself to keep things lively

	//@@ clean this up - better error reporting scheme
	EBString		m_strLastError;		


	RenderMode		m_eRenderMode;	// Mode for drawing / not drawing to screen
									//  after each Tick()


private:
    HRESULT CreateTextureRenderTarget();
    void    CreateAndWriteUVOffsets(int width, int height);


private:

	LPDIRECT3DDEVICE8		m_pD3DDev;

	//////////////////////////////////////

	IDirect3DTexture8	* m_pInitialStateTexture;
	IDirect3DTexture8	* m_pRulesTexture;

	//////////////////////////////////////	

	SHADER_INDEX			m_SHI_4Offset_VertexShader;
	SHADER_INDEX			m_SHI_EqualWeightCombineShader;

	SHADER_INDEX			m_SHI_DependentGB;
	SHADER_INDEX			m_SHI_TexelsToBlueWithBias;

	bool					m_bAnimate;
	bool					m_bSingleStep;
	bool					m_bSlow;


	bool					m_bWrap;

	int						m_nSkipInterval;
	int						m_nFlipState;


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

	IDirect3DTexture8 * m_pTexFinalSrc;


	int m_nTargetIntermediate;
	int	m_nTargetCellField;


	///////////////////////////////////////////////////////////

};

#endif				// __CA_GAMEOFLIFE_GJ_H