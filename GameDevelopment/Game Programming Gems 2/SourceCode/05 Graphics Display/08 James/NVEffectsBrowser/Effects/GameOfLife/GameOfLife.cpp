/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\GameOfLife
File:  GameOfLife.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


Thanks to Matthias Wloka for his FilterBlit example, from which the texel
  offsets and basic vertex and pixel shaders were derived.

The variables:
		m_pTexSrc			== texture source for rendering (step N)
		m_pTexFinalTarg		== target surface for result (step N+1)

		m_pIntermediateTarg	== target surface for rendering intermediate calc
								(neighbor accumulation)
		m_pIntermediateSrc	== same surface as above, this is the texture corresponding
								to it, for use as a rendering source

		m_pOutputSrc		== optional additional color re-map for display
		m_pOutputTarg		== target surface for optional color remapping
  are pointers to the appropriate textures and surfaces for rendering to and
  from to run the game.  Since a texture cannot be used as a render source 
  and target at the same time, the m_pTexSrc and m_pTexFinalTarg variables
  ping-pong back and forth to use the previous result in rendering the next
  frame.


******************************************************************************/


#include <crtdbg.h>


#include "eb_effect.h"
#include "nvtex.h"
#include "GameOfLife.h"
#include "..\..\..\CommonSrc\NV_Error.h"			// for FDebug

#include "Constants.h"
#include "PixelConstants.h"


using namespace nv_objects;
using namespace std;


#define RULE_TABLE_NAME   "RuleTable.bmp";
#define OUTPUT_TABLE_NAME "OUTPUT_map_1.bmp"
#define INITIAL_FILENAME  "start.bmp"



#define ASSERT_IF_FAILED( hres )	\
	if( FAILED(hres) )				\
	   assert( false );				

////////////////////////////////////////////////////////////////////


DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() 
{ 
    return static_cast<unsigned int>(CGameOfLife::NUM_EFFECT_OPTIONS); 
}

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CGameOfLife(static_cast<CGameOfLife::eFilterOptions>(EffectNum));
}

}


////////////////////////////////////////////////////////////////////

CGameOfLife::CGameOfLife( eFilterOptions initDisplayOption )
{
	meDisplayOption			= initDisplayOption;
	m_bWireframe			= false;
	m_pInitialStateTexture	= NULL;
	m_pRulesTexture			= NULL ;
	m_pOutputMapTexture		= NULL;
	m_dwVertexShader		= 0;
	m_dwEqualWeightCombineShader = 0;
	m_dwDependentGB			= 0;
	m_pVertexBuffer			= NULL;
	mpBackbufferColor		= 0 ;
	mpBackbufferDepth		= 0 ;


	for ( int i = 0; i < kMaxNumPasses; ++i )
	{
        mpTextureFiltered[i] = 0;
		mpFilterTarget   [i] = 0;
	}

	m_strEffectLocation     = "Pixel Shaders\\Cellular Automata";
	m_strEffectVertexShader = GetFilePath("GameOfLife.nvv");


    switch (meDisplayOption)
	{
		case GAME_OF_LIFE_EFFECT:
            m_strEffectName        = "Game of Life";
			m_strEffectPixelShader = GetFilePath("EqualWeightCombine.nvp");
			break;
		default:
            m_strEffectName        = "Game of Life (unknown)";
			m_strEffectPixelShader = GetFilePath("EqualWeightCombine.nvp");
		    break;
    }
}

CGameOfLife::~CGameOfLife()
{
	Free();	
}

/////////////////////////////////////////////////////////


HRESULT CGameOfLife::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
    // check vertex shading support
    if(D3DSHADER_VERSION_MAJOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support vertex shaders!";
		return E_FAIL;
	}
    if(D3DSHADER_VERSION_MINOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support 1.1 vertex shaders!";
		return E_FAIL;
	}

    // check simultaneous texture support
    if(pCaps->MaxSimultaneousTextures < 4)
	{
		m_strLastError = "Device does not support 4 simultaneous textures!";
		return E_FAIL;
	}

    // check pixel shader support
    if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}
	if(D3DSHADER_VERSION_MINOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support 1.1 pixel shaders!";
		return E_FAIL;
	}
	if(pCaps->MaxTextureBlendStages < 8)
	{
		m_strLastError = "Device does not support 8 register combiners!";
		return E_FAIL;
	}


	return S_OK;
}


void CGameOfLife::UpdateProperties()
{
	EBEffect::UpdateProperties();

    AddProperty(new EBProperty("W  - Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	AddProperty(new EBProperty("Home  - Restart", OBJECT_MEMBER( m_bReset ), EBTYPE_BOOL_PROP ));
	AddProperty(new EBProperty("G   - Go/Pause anim", OBJECT_MEMBER( m_bAnimate ), EBTYPE_BOOL_PROP ));
	AddProperty(new EBProperty("SPC - Single step", OBJECT_MEMBER( m_bSingleStep ), EBTYPE_BOOL_PROP ));
	AddProperty(new EBProperty("B   - Border wrapping", OBJECT_MEMBER( m_bWrap ), EBTYPE_BOOL_PROP ));
	AddProperty(new EBTriggerProperty("Hit F1 for more options"));


	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Neighbor Calc Offsets",
										GetFilePath("GameOfLife.nvv"), EBTYPE_STRING_PROP));

	// Pixel shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Equal Weight",
										GetFilePath("EqualWeightCombine.nvp"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Dependent GB",
										GetFilePath("DependentGB.nvp"), EBTYPE_STRING_PROP));
}


void CGameOfLife::LoadRulesAndOtherMaps()
{
	// Load "rules" texture which determines how the neighbor accumulation 
	//  maps to new pixels
	// Also reload output color map 
	// Also reload initial conditions map

	EBString filename;
	HRESULT hr;

	if( m_pRulesTexture != NULL )
	{
		SAFE_DELETE( m_pRulesTexture );
		m_pRulesTexture = NULL;
	}

	if( m_pOutputMapTexture != NULL )
	{
		SAFE_DELETE( m_pOutputMapTexture );
		m_pOutputMapTexture = NULL;
	}

	if( m_pInitialStateTexture != NULL )
	{
		SAFE_DELETE( m_pInitialStateTexture );
		m_pInitialStateTexture = NULL;
	}


	filename = RULE_TABLE_NAME;

	m_pRulesTexture = new NVTexture();
	assert( m_pRulesTexture != NULL );

    hr = m_pRulesTexture->CreateFromFile(m_pD3DDev, GetFilePath( filename.c_str() ) );
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't load texture: ";
		m_strLastError += filename;
		assert( false );
		return;
	}


	filename = OUTPUT_TABLE_NAME;

	m_pOutputMapTexture = new NVTexture();
	assert( m_pOutputMapTexture != NULL );

    hr = m_pOutputMapTexture->CreateFromFile(m_pD3DDev, GetFilePath( filename.c_str() ) );
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't load texture: ";
		m_strLastError += filename;
		assert( false );
		return;
	}

	
	filename = INITIAL_FILENAME;

	m_pInitialStateTexture = new NVTexture();
	assert( m_pInitialStateTexture != NULL );

    hr = m_pInitialStateTexture->CreateFromFile(m_pD3DDev, GetFilePath( filename.c_str() ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't load texture: ";
		m_strLastError += filename;
		assert( false );
		return;
	}


	//  Not re-creating render targets.  This could be problem if 
	//   you reload an initial texture with different resolution while
	//   the simulation is running.  The new resolution will not be 
	//   reflected in the render targets

}



HRESULT CGameOfLife::Initialize( LPDIRECT3DDEVICE8 pDev )
{
	// Called when effect is selected from list in browser
	// Free() is called when effect is de-selected

    int     i;
	HRESULT hr;


	// get the device
	m_pD3DDev = pDev;
	pDev->AddRef();			// released on Free()



	m_bReset = true;
	m_bAnimate = true;
	m_bSingleStep = false;

	m_bSlow = false;

	m_bFarther = false;
	m_bWrap = false;

	
	m_eRenderMode = FULLSCREEN_NEIGHBOR_CALC;

	m_nSkipInterval = 0;
	m_nFlipState = 0;


	// create the vertex and pixel shaders
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwVertexShader = 0;
	hr = LoadAndCreateShader(GetFilePath("GameOfLife.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwVertexShader);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;
	

    hr = LoadAndCreateShader(GetFilePath("EqualWeightCombine.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwEqualWeightCombineShader));
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	// Dependent 2D lookup pixel shader for logic rules:
    hr = LoadAndCreateShader(GetFilePath("DependentGB.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwDependentGB ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;



    // get a pointer to the current back-buffer (so we can restore it later)
	m_pD3DDev->GetRenderTarget(        &mpBackbufferColor );
	m_pD3DDev->GetDepthStencilSurface( &mpBackbufferDepth );


//gjdel	EBString filename;


	// load the initial conditions texture map, rules map, and 
	//  optional final color re-mapping map

	// The "rules" texture determines how the neighbor accumulation 
	//  maps to new pixels in the next generation

	LoadRulesAndOtherMaps();

	hr = CreateTextureRenderTarget();
	ASSERT_IF_FAILED(hr);


	////////////////////////////////////////////////////////////////////////////


	D3DSURFACE_DESC ddsd;
    m_pInitialStateTexture->GetTexture()->GetLevelDesc(0, &ddsd);

	// create vertex buffer 
	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVertexBuffer);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	QuadVertex      *pBuff;

	if (m_pVertexBuffer)
	{
		hr = m_pVertexBuffer->Lock(0, 4 * sizeof(QuadVertex),(BYTE**)&pBuff, 0);
		ASSERT_IF_FAILED(hr);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't lock buffer!";
			return hr;
		}


		float uv_base;
		float uv_max;

		uv_base = 0.0f;
		uv_max  = 1.0f; 

        for (i = 0; i < 4; ++i)
        {
            pBuff->Position = D3DXVECTOR3((i==0 || i==3) ? -1.0f : 1.0f,
                                          (i<2)          ? -1.0f : 1.0f,
                                          0.0f);
		    pBuff->Tex      = D3DXVECTOR2((i==0 || i==3) ? uv_base : uv_max , 
                                          (i<2)          ? uv_max : uv_base );
		    pBuff++;
        }
        m_pVertexBuffer->Unlock();
    }

    CreateAndWriteUVOffsets(ddsd.Width, ddsd.Height);

    // set up render state: disable all except texture stage 0 (see below)
    for(i = 0; i < 4; i++ )
    {
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSW,  D3DTADDRESS_CLAMP);

        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_POINT );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE  );

		m_pD3DDev->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,	 D3DTTFF_DISABLE );
		m_pD3DDev->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | i );

		m_pD3DDev->SetTextureStageState(i, D3DTSS_RESULTARG, D3DTA_CURRENT );
    }

	// Set wrap to zero - It will wrap anyway and avoid the problems of
	//   setting cylindrical wrapping (D3DRS_WRAP0, D3DWRAP_U == cylindrical)
	//   for texture coords > 1.0f

	// For wrapped textures, you should use only the D3DTSS_ADDRESSU/V/W
	//   states, unless you are doing cube environment mapping.

    m_pD3DDev->SetRenderState(D3DRS_WRAP0, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP1, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP2, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP3, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP4, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP5, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP6, 0);
    m_pD3DDev->SetRenderState(D3DRS_WRAP7, 0);


    m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);

    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE,	FALSE );
    m_pD3DDev->SetRenderState(D3DRS_ZENABLE,			FALSE );

	m_pD3DDev->SetRenderState(D3DRS_FOGENABLE,			FALSE );
	m_pD3DDev->SetRenderState(D3DRS_DITHERENABLE,		FALSE );
	m_pD3DDev->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE );
	m_pD3DDev->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_ONE  );
	m_pD3DDev->SetRenderState(D3DRS_DESTBLEND,			D3DBLEND_ZERO );

	m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE,		FALSE );

	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE,		FALSE );
	m_pD3DDev->SetRenderState(D3DRS_STENCILENABLE,		FALSE );
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING,			FALSE );

	m_pD3DDev->SetRenderState(D3DRS_COLORVERTEX,		FALSE );

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE,			D3DFILL_SOLID );
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE );


    return hr;
}



HRESULT CGameOfLife::Free()
{
	SAFE_RELEASE( m_pVertexBuffer);			// sets pointers to null after delete
	SAFE_DELETE( m_pInitialStateTexture);
	SAFE_DELETE( m_pRulesTexture );
	SAFE_DELETE( m_pOutputMapTexture );
	
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwVertexShader);
		m_pD3DDev->DeletePixelShader(m_dwEqualWeightCombineShader);
		m_pD3DDev->DeletePixelShader(m_dwDependentGB );


        for (int i = 0; i < kMaxNumPasses; ++i)
        {            
            SAFE_RELEASE(mpFilterTarget[i]);
            SAFE_RELEASE(mpTextureFiltered[i]);
        }

        SAFE_RELEASE(mpBackbufferColor);
		SAFE_RELEASE(mpBackbufferDepth);

        SAFE_RELEASE(m_pD3DDev);		// we AddRef()'d in Initialize
	}
	
	return S_OK;
}


HRESULT CGameOfLife::Start()
{
	return S_OK;
}




HRESULT CGameOfLife::CreateTextureRenderTarget()
{
    HRESULT         hr;

	_ASSERT( mpBackbufferColor != NULL );
	_ASSERT( mpBackbufferDepth != NULL );

    // get the description for the texture we want to filter
	D3DSURFACE_DESC ddsd;
    m_pInitialStateTexture->GetTexture()->GetLevelDesc(0, &ddsd);

	int i;

	for( i=0; i < kMaxNumPasses; i++ )
	{
		if( mpFilterTarget[i] != NULL )
		{
            SAFE_RELEASE(mpFilterTarget[i]);
			mpFilterTarget[i] = NULL;
		}

		if( mpTextureFiltered[i] != NULL )
		{            
            SAFE_RELEASE(mpTextureFiltered[i]);
			mpTextureFiltered[i] = NULL;
		}
	}


    // create new textures just like the current texture
    for( i = 0; i < kMaxNumPasses; i++ )
    {
        hr = m_pD3DDev->CreateTexture(ddsd.Width, ddsd.Height, 1, 
                                      D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, 
                                      D3DPOOL_DEFAULT, &mpTextureFiltered[i]);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't CreateTexture!\n";
            assert(false);
            return E_FAIL;
        }

        hr = mpTextureFiltered[i]->GetSurfaceLevel(0, &mpFilterTarget[i]);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't Get to top-level texture!\n";
            assert(false);
            return E_FAIL;
        }

        // set our render target to the new and shiny textures without depth
        hr = m_pD3DDev->SetRenderTarget(mpFilterTarget[i], NULL);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't SetRenderTarget to new surface!\n";
            assert(false);
            return E_FAIL;
        }
    }

    // switch back to conventional back-buffer
    hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
    if (FAILED(hr))
    {
		m_strLastError = "Can't SetRenderTarget to original back-buffer surfaces!\n";
        assert(false);
        return E_FAIL;
    }

    return S_OK;
}



void CGameOfLife::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	// Note - pressing tidle key will not cause this to be called
	//  until the window is resized - Something happens then to 
	//  trigger the Keyboard function.

    unsigned int option = static_cast<unsigned int>(meDisplayOption); 

	if( bDown )
	{
		// Use only capital letters here - these are VK_ codes!

		switch( dwKey )
		{
		case 'B':
			m_bWrap = !m_bWrap;
			FDebug("Wrap mode:  %d\n", m_bWrap );
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;

		case 'M':
		case '.':
		case '>':
			m_nSkipInterval++;
			FDebug("Skip Interval:  %d\n", m_nSkipInterval );
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		case 'N':
		case ',':
		case '<':
			m_nSkipInterval--;
			if( m_nSkipInterval < 0 )
				m_nSkipInterval = 0;
			FDebug("Skip Interval:  %d\n", m_nSkipInterval );
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		case '1' :
			m_eRenderMode = FULLSCREEN_NEIGHBOR_CALC;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;

		case '2':
			m_eRenderMode = FULLSCREEN_RESULT;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;

		case '3' :
			m_eRenderMode = ALL_TOGETHER;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		case '4':
			m_eRenderMode = FULLSCREEN_FINALOUT;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;

		case 'F':
			m_bFarther = !m_bFarther;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;

		case 'R':
		case '`':
		case '~':
			FDebug("Loading new rules texture map\n");
			LoadRulesAndOtherMaps();
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		case 'G':
			// start or stop the animation
			m_bAnimate = !m_bAnimate;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		case VK_SPACE:
			m_bSingleStep = !m_bSingleStep;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		case 'S':
			m_bSlow = !m_bSlow;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			return;
			break;
		}
	}


	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			EBString str;
			str =  " Help : F1 - Help\n\n";
			str += " W    - Wireframe toggle\n\n";

			str += " Home - Restart from initial frame  \n\n";

			str += " R    - Reload \"rules\" and \"output\" maps   \n";
			str += "          from files\n";
			str += " G    - Stop/Go animation toggle\n";
			str += " SPC  - Single frame step when anim stopped   \n";
			str += " B    - Toggle border wrapping\n\n";

			str += " 1    - Draw neighbor accumulation map\n";
			str += " 2    - Draw current generation\n";
			str += " 3    - Draw initial, temp, and output frames\n";
			str += " 4    - Draw color re-map if enabled\n";
			str += " F    - Toggle additional color re-map step\n\n";

			str += " M    - Increase # frames to skip\n";
			str += " N    - Decrease # frames to skip\n";
			str += " S    - Slow updates to ~20 fps\n\n";

			::MessageBoxEx( NULL, str.c_str(),
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_RESET:
        {
			m_bReset = true;

        }
		break;

        case EB_ADD:
            break;

        case EB_SUBTRACT:
            break;

        default:
            break;
    }
}





// Our properties were updated, set the correct shader to match
void CGameOfLife::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	// If somebody changed the filter we are doing, set the dirty flag and change the 
    // pixel shader enum to reflect this.
	if (pProperty->IsKindOf(EBTYPE_ENUM_PROP))
	{
		if (bWritten)
		{
			if (pProperty->GetOffset() == OBJECT_MEMBER(m_strEffectPixelShader))
			{
				if (m_strEffectPixelShader.compare(GetFilePath("EqualWeightCombine.nvp")) == 0)
				{
					meDisplayOption = GAME_OF_LIFE_EFFECT;
				}
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			}
			else
			{
				switch (meDisplayOption)
				{
					case GAME_OF_LIFE_EFFECT:
						m_strEffectPixelShader = GetFilePath("EqualWeightCombine.nvp");
						break;
					default:
					;
				}

				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PIXELSHADERS;
			}
		}		
	}
}


HRESULT CGameOfLife::Tick(EBTimer* pTimer)
{	
	// Render a new frame

	HRESULT hr;



	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;

    // write to constant memory which uv-offsets to use to accumulate neighbor information
    D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	// Disable culling
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // set render state 
	m_pD3DDev->SetVertexShader(m_dwVertexShader);
	m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));


	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// Set World, View, Projection, and combination matrices.
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);

    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

    // draw a single quad to texture:
    // the quad covers the whole "screen" exactly

	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);



	if( m_bAnimate == true  )
	{
		// Update the textures for one step of the 
		//   game or cellular automata 

		//DoSingleTimeStep();
		DoSingleTimeStep_3Pass();

	}
	else if( m_bSingleStep == true )
	{
		//DoSingleTimeStep();
		DoSingleTimeStep_3Pass();

		m_bSingleStep = false;

		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
	}
	else
	{
		// slow down the rendering and give any other apps a chance
		Sleep(50);
	}

	if( m_bSlow )
	{
		Sleep(50);
	}




	static int nSkip = 0;

	if( nSkip >= m_nSkipInterval )
	{
		nSkip = 0;

		// Switch back to normal rendering to display the results of 
		//   the rendering to texture

		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


		hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
		ASSERT_IF_FAILED( hr );

		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0,0,0 ), 1.0, 0);
		ASSERT_IF_FAILED(hr);


		// turn off pixel shading
		m_pD3DDev->SetPixelShader(0);	

		

		if( m_bWireframe )
		{
			m_pD3DDev->SetRenderState(D3DRS_FILLMODE,	D3DFILL_WIREFRAME );

			// chances are the texture will be all dark, so render in solid color
			m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR,  0xFFFFFFFF );

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR	);

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
			m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		}
		else
		{
			m_pD3DDev->SetRenderState(D3DRS_FILLMODE,	D3DFILL_SOLID );

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
			m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		}

		switch( m_eRenderMode )
		{
		case FULLSCREEN_FINALOUT :

			// Draw quad over full display
			D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);

			D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, m_pOutputSrc );
			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			break;

		case FULLSCREEN_RESULT :
			// Draw quad over full display
			D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);

			D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, m_pTexSrc );
			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			break;
		case FULLSCREEN_NEIGHBOR_CALC:

			// Draw quad over full display
			D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);

			D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, m_pIntermediateSrc );
			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			break;
		case ALL_TOGETHER :
		   // draw quad in upper left corner: original texture
			D3DXMatrixTranslation(&matWorld, -1.0f, 1.0f, 0.0f);
			D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, m_pInitialStateTexture->GetTexture());
			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);


			// draw quads in the other corners, use generated textures
			for (int j = 0; j < 3; ++j)
			{
				D3DXMatrixTranslation(&matWorld, (j == 2) ? -1.0f :  1.0f, 
												 (j == 0) ?  1.0f : -1.0,
												 0.0f);
				D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
				D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
				m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

				m_pD3DDev->SetTexture(0, mpTextureFiltered[j]);
				hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
			}

			break;
		};		// switch for display config
	}
	else
	{
		// skip rendering this frame
		// Set normal render target in case the app is doing more rendering
		//   to the window
		hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);

		nSkip++;
	}


	return( hr );
}




HRESULT CGameOfLife::DoSingleTimeStep()
{
	HRESULT hr;


    // write to constant memory which uv-offsets to use
    D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);


	if( m_bReset == true )
	{
		m_bReset = false;
		m_nFlipState = 0;
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
	}


	float pix_mult[4] = { 0.0f, 0.125f, 0.0f, 0.0f };

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);


	// select source texture:

	// targ ind 0 = neighbor calc
	// targ ind 1 = result storage #1
	// targ ind 2 = result storage #2
	// targ ind 3 = result with re-mapped colors for display only

	switch( m_nFlipState )
	{
	case 0:
		m_pTexSrc			= m_pInitialStateTexture->GetTexture();
		m_pTexFinalTarg		= mpFilterTarget[1];

		m_pIntermediateTarg	= mpFilterTarget[0];
		m_pIntermediateSrc	= mpTextureFiltered[0];

		m_pOutputSrc		= mpTextureFiltered[3];
		m_pOutputTarg		= mpFilterTarget[3];


		break;
	case 1:
		m_pTexSrc			= mpTextureFiltered[1];
		m_pTexFinalTarg		= mpFilterTarget[2];

		m_pIntermediateTarg	= mpFilterTarget[0];
		m_pIntermediateSrc	= mpTextureFiltered[0];

		m_pOutputSrc		= mpTextureFiltered[3];
		m_pOutputTarg		= mpFilterTarget[3];

		break;
	case 2:
		m_pTexSrc			= mpTextureFiltered[2];
		m_pTexFinalTarg		= mpFilterTarget[1];

		m_pIntermediateTarg	= mpFilterTarget[0];
		m_pIntermediateSrc	= mpTextureFiltered[0];

		m_pOutputSrc		= mpTextureFiltered[3];
		m_pOutputTarg		= mpFilterTarget[3];
		
		break;
	}


	// Clear intermediate target
	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);
	ASSERT_IF_FAILED( hr );
	// Clear 1st target to small fraction of green and blue to offset the
	//  dependent lookup to the proper values
	//  0xff / 16 = 0x10 
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x04, 0x02 ), 1.0, 0);


	// even if wireframe mode, render to texture as solid
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE,			D3DFILL_SOLID );


	/////////////////////////////////////////////////////////
	// First, write the source texture into the blue channel
	// I do this in preparation for a 2D dependent green-blue lookup
	//   into a "rules" texture which governs how old pixels spawn
	//   or die into new pixels.  
	// The logic for the game of life depends on 9 pixels:  the source
	//   pixel and it's 8 neightbors.  These are accumulated in three
	//   passes.


	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);


	m_pD3DDev->SetPixelShader( 0 );		// no pixel shader
	// Add into dest texture - texture cleared to small fraction of 
	//  green and blue to offset the dependent lookup to the proper values

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Setup traditional pipeline to copy the white source pixels
	//  into just the blue of the dest:

	m_pD3DDev->SetTexture(0, m_pTexSrc );

	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR,			0x000000FF );		// ARGB

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR	);	// blue only

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );


	// Render using offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	// draw the fan with normal texture coords
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);



	/////////////////////////////////////////////////////////////////
	// Setup neighbor blending modes:

	hr = m_pD3DDev->SetPixelShader( m_dwEqualWeightCombineShader );	
	ASSERT_IF_FAILED( hr );



	DWORD wrapval = m_bWrap ?  D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV,  wrapval);
	
	/////////////////////////////////////////////////////////////////
	// Render 1st set of neighbors
	// Add in nearest neighbors


	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);
	ASSERT_IF_FAILED(hr);

	// Add result of pixel operations into the dest texture:
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Render 1st using offset set 1 - for nearest neighbor pixels
	offset.x = 1.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	m_pD3DDev->SetTexture(0, m_pTexSrc );
    m_pD3DDev->SetTexture(1, m_pTexSrc );
    m_pD3DDev->SetTexture(2, m_pTexSrc );
    m_pD3DDev->SetTexture(3, m_pTexSrc );

	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	/////////////////////////////////////////////////////////////////
	// Render 2nd set of neighbors
	// Add diagonal neighbors 

	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);
	ASSERT_IF_FAILED(hr);
	
	// Don't clear - we're still adding the 8 neighbors

	// Render 2nd using offsets set 2 - for diagonal pixels
	offset.x = 2.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	m_pD3DDev->SetTexture(0, m_pTexSrc );
    m_pD3DDev->SetTexture(1, m_pTexSrc );
    m_pD3DDev->SetTexture(2, m_pTexSrc );
    m_pD3DDev->SetTexture(3, m_pTexSrc );

	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);
    
	// If wrapping was on, reset to clamp now for the rest of the 
	//   rendering
	if( m_bWrap )
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
	}


	/////////////////////////////////////////////////////////////////
	// Now do dependent 2D lookup into rules texture to set new
	//   source pixels;

	hr = m_pD3DDev->SetPixelShader( m_dwDependentGB );
	ASSERT_IF_FAILED( hr );


	hr = m_pD3DDev->SetRenderTarget( m_pTexFinalTarg, NULL);
	ASSERT_IF_FAILED(hr);


	// Render using offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	m_pD3DDev->SetTexture(0, m_pIntermediateSrc );
	m_pD3DDev->SetTexture(1, m_pRulesTexture->GetTexture() );


	ASSERT_IF_FAILED( hr );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);


	/////////////////////////////////////////////////////////////////
	// If further processing is requested, do 1 more dependent 
	//   lookup into a colormap for display to the user only.
	//   This remapping doesn't affect the logic at all, it's only
	//   a prettier version for the user to see.

	if( m_bFarther )
	{

		hr = m_pD3DDev->SetPixelShader( m_dwDependentGB );
		ASSERT_IF_FAILED( hr );


		hr = m_pD3DDev->SetRenderTarget( m_pOutputTarg, NULL);
		ASSERT_IF_FAILED(hr);


		// Render using offsets of zero
		offset.x = 0.0f;
		m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


		hr = m_pD3DDev->SetTexture(0, m_pIntermediateSrc );
		hr = m_pD3DDev->SetTexture(1, m_pOutputMapTexture->GetTexture() );

		ASSERT_IF_FAILED( hr );

		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


		// draw the fan with displaced texture coordinates
		hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		ASSERT_IF_FAILED(hr);

	}




	// Flip the state variable for the next round of rendering
	switch( m_nFlipState )
	{
	case 0:
		m_nFlipState = 1;
		break;
	case 1:
		m_nFlipState = 2;
		break;
	case 2:
		m_nFlipState = 1;
		break;
	}


	return hr;
}



HRESULT CGameOfLife::DoSingleTimeStep_3Pass()
{

	HRESULT hr;
	int i;

    // write to constant memory which uv-offsets to use
    D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);


	if( m_bReset == true )
	{
		m_bReset = false;
		m_nFlipState = 0;
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
	}





	// select source texture:

	// targ ind 0 = neighbor calc
	// targ ind 1 = result storage #1
	// targ ind 2 = result storage #2
	// targ ind 3 = result with re-mapped colors for display only

	switch( m_nFlipState )
	{
	case 0:
		m_pTexSrc			= m_pInitialStateTexture->GetTexture();
		m_pTexFinalTarg		= mpFilterTarget[1];

		m_pIntermediateTarg	= mpFilterTarget[0];
		m_pIntermediateSrc	= mpTextureFiltered[0];

		m_pOutputSrc		= mpTextureFiltered[3];
		m_pOutputTarg		= mpFilterTarget[3];


		break;
	case 1:
		m_pTexSrc			= mpTextureFiltered[1];
		m_pTexFinalTarg		= mpFilterTarget[2];

		m_pIntermediateTarg	= mpFilterTarget[0];
		m_pIntermediateSrc	= mpTextureFiltered[0];

		m_pOutputSrc		= mpTextureFiltered[3];
		m_pOutputTarg		= mpFilterTarget[3];

		break;
	case 2:
		m_pTexSrc			= mpTextureFiltered[2];
		m_pTexFinalTarg		= mpFilterTarget[1];

		m_pIntermediateTarg	= mpFilterTarget[0];
		m_pIntermediateSrc	= mpTextureFiltered[0];

		m_pOutputSrc		= mpTextureFiltered[3];
		m_pOutputTarg		= mpFilterTarget[3];
		
		break;
	}


	// Clear intermediate target
	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);
	ASSERT_IF_FAILED( hr );
	// Clear 1st target to small fraction of green and blue to offset the
	//  dependent lookup to the proper values
	//  0xff / 16 = 0x10 
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x04, 0x02 ), 1.0, 0);


	// even if wireframe mode, render to texture as solid
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE,			D3DFILL_SOLID );


	/////////////////////////////////////////////////////////
	// First, write the source texture into the blue channel
	// I do this in preparation for a 2D dependent green-blue lookup
	//   into a "rules" texture which governs how old pixels spawn
	//   or die into new pixels.  
	// The logic for the game of life depends on 9 pixels:  the source
	//   pixel and it's 8 neightbors.  These are accumulated in three
	//   passes.


	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);


	m_pD3DDev->SetPixelShader( 0 );		// no pixel shader
	// Add into dest texture - texture cleared to small fraction of 
	//  green and blue to offset the dependent lookup to the proper values

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Setup traditional pipeline to copy the white source pixels
	//  into just the blue of the dest:

	m_pD3DDev->SetTexture(0, m_pTexSrc );

	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR,			0x000000FF );		// ARGB

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR	);	// blue only

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );


	// Render using offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	// draw the fan with normal texture coords
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);



	/////////////////////////////////////////////////////////////////
	// Setup neighbor blending modes:

	hr = m_pD3DDev->SetPixelShader( m_dwEqualWeightCombineShader );	
	ASSERT_IF_FAILED( hr );



	DWORD wrapval = m_bWrap ?  D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV,  wrapval);
	
	/////////////////////////////////////////////////////////////////
	// Render 1st set of neighbors
	// Add in nearest neighbors


	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);
	ASSERT_IF_FAILED(hr);

	// Add result of pixel operations into the dest texture:
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Render set of neighbors using bilinear filtering to sample
	//  equal weight of all 8 neighbors in just 4 texture samples

	offset.x = 3.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	float pix_mult[4] = { 0.0f, 0.25f, 0.0f, 0.0f };
//@@
	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);


	m_pD3DDev->SetTexture(0, m_pTexSrc );
    m_pD3DDev->SetTexture(1, m_pTexSrc );
    m_pD3DDev->SetTexture(2, m_pTexSrc );
    m_pD3DDev->SetTexture(3, m_pTexSrc );

	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR		);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR		);
	}


	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);



	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_POINT	);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_POINT	);
	}

/*
	/////////////////////////////////////////////////////////////////
	// Render 2nd set of neighbors
	// Add diagonal neighbors 

	hr = m_pD3DDev->SetRenderTarget( m_pIntermediateTarg, NULL);
	ASSERT_IF_FAILED(hr);
	
	// Don't clear - we're still adding the 8 neighbors

	// Render 2nd using offsets set 2 - for diagonal pixels
	offset.x = 2.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	m_pD3DDev->SetTexture(0, m_pTexSrc );
    m_pD3DDev->SetTexture(1, m_pTexSrc );
    m_pD3DDev->SetTexture(2, m_pTexSrc );
    m_pD3DDev->SetTexture(3, m_pTexSrc );

	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);
*/
    
	// If wrapping was on, reset to clamp now for the rest of the 
	//   rendering
	if( m_bWrap )
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
		m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
	}


	/////////////////////////////////////////////////////////////////
	// Now do dependent 2D lookup into rules texture to set new
	//   source pixels;

	hr = m_pD3DDev->SetPixelShader( m_dwDependentGB );
	ASSERT_IF_FAILED( hr );


	hr = m_pD3DDev->SetRenderTarget( m_pTexFinalTarg, NULL);
	ASSERT_IF_FAILED(hr);


	// Render using offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	m_pD3DDev->SetTexture(0, m_pIntermediateSrc );
	m_pD3DDev->SetTexture(1, m_pRulesTexture->GetTexture() );


	ASSERT_IF_FAILED( hr );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);


	/////////////////////////////////////////////////////////////////
	// If further processing is requested, do 1 more dependent 
	//   lookup into a colormap for display to the user only.
	//   This remapping doesn't affect the logic at all, it's only
	//   a prettier version for the user to see.

	if( m_bFarther )
	{

		hr = m_pD3DDev->SetPixelShader( m_dwDependentGB );
		ASSERT_IF_FAILED( hr );


		hr = m_pD3DDev->SetRenderTarget( m_pOutputTarg, NULL);
		ASSERT_IF_FAILED(hr);


		// Render using offsets of zero
		offset.x = 0.0f;
		m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


		hr = m_pD3DDev->SetTexture(0, m_pIntermediateSrc );
		hr = m_pD3DDev->SetTexture(1, m_pOutputMapTexture->GetTexture() );

		ASSERT_IF_FAILED( hr );

		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


		// draw the fan with displaced texture coordinates
		hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		ASSERT_IF_FAILED(hr);

	}




	// Flip the state variable for the next round of rendering
	switch( m_nFlipState )
	{
	case 0:
		m_nFlipState = 1;
		break;
	case 1:
		m_nFlipState = 2;
		break;
	case 2:
		m_nFlipState = 1;
		break;
	}


	return hr;



}



void CGameOfLife::CreateAndWriteUVOffsets(int width, int height)
{

	// This sets vertex shader constants used to displace the
	//  source texture over several additive samples.  This is 
	//  used to accumulate neighboring texel information that we
	//  need to run the game - the 8 surrounding texels, and the 
	//  single source texel which will either spawn or die in the 
	//  next generation.
	// Label the texels as follows, for a source texel "e" that
	//  we want to compute for the next generation:
	//		
    //          abc
    //          def
    //          ghi:


    // first the easy one: no offsets for sampling center
	//  occupied or unoccupied
	// Use index offset value 0.0 to access these in the 
	//  vertex shader.

    float      noOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f}; 
    float      noOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

    float      kPerTexelWidth; 
    float      kPerTexelHeight;
    
    kPerTexelWidth  = 1.0f/static_cast<float>(width);
    kPerTexelHeight = 1.0f/static_cast<float>(height);
	

	HRESULT hr;

	// Change the multiplier to 2 or 3 to spread the texel sampling.  This
	//  has the effect of running several independent "games" in neighboring
	//  texels, which do not affect eachother.

	kPerTexelWidth	*= 1;	// 1 = no change - run standard game of life
    kPerTexelHeight *= 1;



	// Offset set 1:  Nearest neighbors -  b,d,f,h texels
	// Use index offset 1.0 to access these

    float		type1OffsetX[4] = { -  kPerTexelWidth, 
                                       kPerTexelWidth,  
                                       0.0f,
									   0.0f };

    float		type1OffsetY[4] = {    0.0f, 
                                       0.0f,
									   kPerTexelHeight, 
                                    -  kPerTexelHeight  };


	// These are a,c,g,i texels == diagonal neightbors
	// Use index offset 2.0 to use these

    float		type2OffsetX[4] = { -  kPerTexelWidth, 
                                    -  kPerTexelWidth,  
                                       kPerTexelWidth,   
                                       kPerTexelWidth  };
    float		type2OffsetY[4] = { -  kPerTexelHeight, 
                                       kPerTexelHeight, 
                                       kPerTexelHeight, 
                                    -  kPerTexelHeight };
	
	// These offsets are for use with bilinear filtering
	//   of the neighbors, to sample all 8 neighbors in
	//   one pass instead of two.  Bilinear averages the
	//   two bordering texels, but the coordinate must be
	//   exactly on the texel border to make this work.
	//		[0] = on the border of the ab texels
	//      [1] = between cf texels
	//		[2] = between ih texels
	//		[3] = between gd texels


	float woff, hoff;
	woff =  kPerTexelWidth/2.0f;
	hoff =  kPerTexelHeight/2.0f;

    float      type3OffsetX[4] = {	- kPerTexelWidth/2.0f	+ woff,
									  kPerTexelWidth		+ woff,
									  kPerTexelWidth/2.0f	+ woff,
									- kPerTexelWidth		+ woff	};
	
    float      type3OffsetY[4] = {	- kPerTexelHeight		+ hoff,
									- kPerTexelHeight/2.0f	+ hoff,
									  kPerTexelHeight		+ hoff,
									  kPerTexelHeight/2.0f	+ hoff	};


    float      type4OffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f}; 
    float      type4OffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f};


    // write all these offsets to constant memory
    for (int i = 0; i < 4; ++i)
    {
        D3DXVECTOR4  noOffset(      noOffsetX[i],    noOffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type1Offset(type1OffsetX[i], type1OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type2Offset(type2OffsetX[i], type2OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type3Offset(type3OffsetX[i], type3OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type4Offset(type4OffsetX[i], type4OffsetY[i], 0.0f, 0.0f);

    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_NO_OFFSET + 5*i, &noOffset,    1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE1     + 5*i, &type1Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE2     + 5*i, &type2Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE3     + 5*i, &type3Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE4     + 5*i, &type4Offset, 1);
    }


	// Set pixel shader consts:

	// This constant is a color mask - It is used to only keep the 
	//    green component for the accumulation of neighbor texel info.
	//    Mult the source green by 1/8 to get the average of the 8 samples,
	//    with a max of 1.0f
	/// R,G,B,A

	float pix_mult[4] = { 0.0f, 0.125f, 0.0f, 0.0f };

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);


}






