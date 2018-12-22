/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\BlinnBumpDynamicMap
File:  CA_Water.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
3/17/01 - Greg James - This is fun stuff!

******************************************************************************/


#ifndef __FILTER_CA_Water_H
#define __FILTER_CA_Water_H


#include <windows.h>

#include <d3d8.h>
#include <d3dx8.h>
#include <dxerr8.h>

#include "eb_string.h"


#include "nvtex.h"

using namespace nv_objects;
using namespace std;


////////////////////////////////////////

class EBEffect;

////////////////////////////////////////

#define	SM_NORMAL_GRAY		4
#define SM_RGB_GLOW			0
#define SM_HUE_DROPLETS		1
#define SM_LAST				2


class CA_Water
{

public:
	enum	SimulationMode
	{
		NORMAL_GRAY = 0,
		RGB_GLOW,
		HUE_DROPLETS,
		SM_LASTMODE
	};
private:
    typedef struct tagQuadVertex
    {
	    D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
    } QuadVertex;

    enum 
    {
        kMaxNumTargets = 6,
    };
	
	enum	RenderMode
	{
		FULLSCREEN_RESULT,
		FULLSCREEN_NEIGHBOR_CALC,
		ALL_TOGETHER,
		FULLSCREEN_CURRENT_POSITION
	};


public:

	CA_Water( EBEffect * pEffect );
	~CA_Water();

	virtual HRESULT	Initialize( EBEffect * pEffect, LPDIRECT3DDEVICE8 pDev );


	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick();
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);

    virtual bool    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);


	virtual HRESULT LoadAndCreateShader(const std::string& strFilePath, const DWORD* pDeclaration,
		                                DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle);


	// Calls the GetFilePath of the EBEffect which created this
	std::string GetFilePath( const std::string& strFileName );


	void	LoadRulesAndOtherMaps();
	HRESULT LoadNVTexture( LPDIRECT3DDEVICE8 pD3DDev, nv_objects::NVTexture * pTex, EBString filename );

	void ReleaseNVTextures();
	void ReleaseNVTextures( nv_objects::NVTexture *** pTex, int num );


	HRESULT		DoSingleTimeStep_3Pass();	// single step with 1 less pass

	void		Do_CreateNormalMapSteps();


	HRESULT		SetPixelShader( DWORD dwHandle );
	HRESULT		SetVertexShader( DWORD dwHandle );

	HRESULT		SetStep_Average_4_Nearest(	IDirect3DSurface8 * pTarget,
											IDirect3DTexture8 * pSrcTex  );
	HRESULT		SetStep_Average_4_Diagonal(	IDirect3DSurface8 * pTarget,
											IDirect3DTexture8 * pSrcTex  );

	void		UpdateBlurVertOffset();


	IDirect3DTexture8 *		GetOutputTexture();

	void		DrawDroplet( float x, float y, bool bInlineDroplet = false );
								// start a droplet at x,y from 0 to 1
								// bool = true only when rendering inside physics update
								// bool = false for creating droplet any other time

	void		SetSimMode( int simmode );


	float		m_fPerTexelWidth;
	float		m_fPerTexelHeight;

	D3DXMATRIX	m_mWorldViewProj;



	EBString	m_strLastError;


	bool					m_bAnimate;
	bool					m_bSingleStep;
	bool					m_bSlow;

    bool                    m_bWireframe;		// only affect final rendering
												// not render to texture steps
	bool					m_bReset;


	bool					m_bDrawOutput;
	bool					m_bApplyInteriorBoundaries;





private:
    HRESULT CreateTextureRenderTarget();
    void    CreateAndWriteUVOffsets(int width, int height);

	void	SetInitialRenderStates();




private:


	void DrawInteriorBoundaryObjects();	// draw fixed objects (boundary conditions
										//  interior to the texture

	EBEffect * m_pEBEffect;

	LPDIRECT3DDEVICE8	m_pD3DDev;


	//////////////////////////////////////

	nv_objects::NVTexture	* m_pInitialStateTexture;
	nv_objects::NVTexture	* m_pDropletTexture;

	//////////////////////////////////////
	
	DWORD			m_dwCurrentPixelShader;
	DWORD			m_dwCurrentVertexShader;

	nv_objects::NVTexture	* m_pCurrentRulesTex;

	//////////////////////////////////////

	DWORD					m_dwVertexShader;
	DWORD					m_dwEqualWeight_PostMultShader;

	DWORD					m_dwNeighborForceCalc_1;
	DWORD					m_dwNeighborForceCalc_2;
	DWORD					m_dwApplyForceShader;
	DWORD					m_dwApplyVelocityShader;

	DWORD					m_dwPSH_NormalMapCreate;
	DWORD					m_dwPSH_NormalMapCreate2_Scale;

	RenderMode				m_eRenderMode;

	int						m_SimulationMode;


	bool					m_bFarther;

	bool					m_bCreateNormalMap;

	bool					m_bMulticolor;

	int						m_nThresh;
	int						m_nLowThresh;
	int						m_nThreshCnt;


	float					m_fDropletFreq;
	DWORD					m_dwSleepDelay;


	bool					m_bWrap;

	int						m_nSkipInterval;
	int						m_nFlipState;

	float					m_fBlend;
	float					m_fVelFactor;

	float					m_fBlurDist;		// 0 to 1 for blurring height after
												//   each step

	bool					m_bUseNormalMapMethod2;
	float					m_fNrmlSTScale;		// 0.0 to 1.0f scale for reducing
												// magnitude of normal map s and t
												// used with m_dwPSH_NormalMapCreate2_Scale

	float					m_fhalfoff;


	float					m_fEqRestore_factor;	// scaler [0-1] for reducing 
													//  force pulling texels to equilib

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
	IDirect3DTexture8 * m_pTex_HeightSrc;
	IDirect3DSurface8 * m_pTex_HeightTarg;

	int				m_nTex_HeightTarg;
	int				m_nTex_HeightSrc;

	int				m_nTex_NrmlAccum;
	int				m_nTex_NrmlTarg;

	int				m_nTex_VelSrc;
	int				m_nTex_VelTarg;

	IDirect3DTexture8 * m_pIntermediateSrc;
	IDirect3DSurface8 * m_pIntermediateTarg;

	IDirect3DTexture8 * m_pInt_FinalSrc;
	IDirect3DSurface8 * m_pInt_FinalTarg;

	IDirect3DTexture8 * m_pTex_VelPrevSrc;		// velocity in last time step
	IDirect3DTexture8 * m_pTex_VelCurSrc;		// velocity for current time step
	IDirect3DSurface8 * m_pTex_VelTarg;			// targ for writing velocity of 
												//  current time step (same as VelCurCrc)


	///////////////////////////////////////////////////////////

};




#endif