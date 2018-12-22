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
#include "ShaderManager.h"


class EBEffect;
class SimpleVBObject8;


/////////////////////////////////////////////////////////

class CA_Fire
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
	
	enum	RenderMode
	{
		FULLSCREEN_FINALOUT,
		FULLSCREEN_EMBER_TEXTURE,
		ALL_TOGETHER,
		DO_NOT_RENDER,
		RM_FORCEDWORD = 0xFFFFFFFF
	};

public:
    typedef enum 
    {
        FIRST_EFFECT_OPTION = 0,
	    EFFECT_FIRE_01 = 0,
        NUM_EFFECT_OPTIONS
    } eFilterOptions;

public:
	CA_Fire();
	~CA_Fire();


	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev, ShaderManager * pManager,
								IDirect3DTexture8 * pInitialTexture = NULL );


	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick();
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);

	virtual void	MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void	MouseMove(HWND hWnd, int x, int y);
    virtual bool	Keyboard(DWORD dwKey, UINT nFlags, bool bDown);


	virtual IDirect3DTexture8 *	GetOutputTexture();

							// Texture rendered in each frame as the 
							//  hot source points of fire & smoke
	void	SetInputTexture( IDirect3DTexture8 * pTex );

	std::string GetFilePath(const std::string& strFileName);


	void		LoadRulesAndOtherMaps( IDirect3DTexture8 * pInitialTexture );


	HRESULT		DoSingleTimeStep();	// single step with 1 less pass

	HRESULT		SetPixelShader( SHADER_INDEX dwIndex );
	HRESULT		SetVertexShader( SHADER_INDEX dwIndex );


	float		m_fPerTexelWidth;
	float		m_fPerTexelHeight;

	int			m_nSlowDelay;

	ShaderManager	*	m_pShaderManager;

	MouseUI			* 	m_pUI;

    bool		m_bWireframe;		// only affects final rendering
									//   not render to texture steps
	bool		m_bReset;


	//@@ clean this up - better error reporting scheme
	EBString			m_strLastError;		

	RenderMode			m_eRenderMode;


private:
    HRESULT CreateTextureRenderTargets();
    void    CreateAndWriteUVOffsets(int width, int height);


private:

	LPDIRECT3DDEVICE8		m_pD3DDev;

	//////////////////////////////////////

	IDirect3DTexture8		* m_pInitialStateTexture;
	IDirect3DTexture8		* m_pOutputMapTexture;
	
	IDirect3DTexture8		* m_pInputTexture;

	//////////////////////////////////////

	SHADER_INDEX			m_dwVertexShader;
	SHADER_INDEX			m_SHI_SimpleObjVShader;
	SHADER_INDEX			m_dwTwoWeight_PostMultShader;

	SHADER_INDEX			m_dwDependentAR;


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

	SimpleVBObject8	*	m_pObj;		// object to render embers onto.  Ultimately this
									//  class shouldn't know or care about the nature
									//  of this object & provide a better inteface.


    IDirect3DSurface8       *mpBackbufferColor; 
    IDirect3DSurface8       *mpBackbufferDepth; 

    IDirect3DTexture8       *mpTextureFiltered[kMaxNumTargets];       
    IDirect3DSurface8       *mpFilterTarget   [kMaxNumTargets]; 

	///////////////////////////////////////////////////////////
	// State pointers or indices to mark source and dest textures
	//  on each pass.
	// Do not free or allocate to these - they are just copies
	//  of pointers elsewhere.

	// Used to hold pointer to output texture
	IDirect3DTexture8 * m_pOutputSrc;

	int	m_nTarget1;
	int	m_nTarget2;


	///////////////////////////////////////////////////////////

};

#endif