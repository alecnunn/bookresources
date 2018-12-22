/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_Fire
File:  CA_Fire.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

A class for generating a procedural texture animation fire effect.

The only thing tying it to the Effects Browser code is the GetFilePath()
  function for loading media and other files.
This is very easy to pull out of the code if you want to use the class
  elsewhere.  If all files are in directory of the executable,
  then the GetFilePath() call can be removed.


******************************************************************************/


#include <crtdbg.h>


#include "eb_effect.h"		// need for mouse button ID only
#include "eb_file.h"		// for GetFilePath and Load..Shader(..)
#include "CA_Fire.h"
#include "..\..\..\CommonSrc\NV_Error.h"			// for FDebug

#include "Constants.h"
#include "PixelConstants.h"

#include "ShaderManager.h"

#include "SimpleObject8.h"
#include "SimpleVBObject8.h"

using namespace std;


#define RULE_TABLE_NAME		"RuleTable.bmp"
#define INITIAL_FILENAME	"start.bmp"
#define OUTPUT_MAP_FILENAME	"OutputColors.bmp"


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


CA_Fire::CA_Fire()
{
	m_bWireframe			= false;
	m_pInitialStateTexture	= NULL;
	m_pOutputMapTexture		= NULL;

	m_dwVertexShader		= 0;
	m_SHI_SimpleObjVShader	= 0;
	m_dwTwoWeight_PostMultShader = 0;
	m_dwDependentAR			= 0;

	m_pVertexBuffer			= NULL;
	m_pObj					= NULL;

	mpBackbufferColor		= 0 ;
	mpBackbufferDepth		= 0 ;

	m_pInputTexture			= NULL;

	m_pD3DDev				= NULL;

	m_pUI = NULL;

	for ( int i = 0; i < kMaxNumTargets; ++i )
	{
        mpTextureFiltered[i] = 0;
		mpFilterTarget   [i] = 0;
	}

}

CA_Fire::~CA_Fire()
{
	Free();	
}

/////////////////////////////////////////////////////////


HRESULT CA_Fire::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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




void CA_Fire::LoadRulesAndOtherMaps( IDirect3DTexture8 * pInitialTexture )
{
	// Load "rules" texture which determines how the neighbor accumulation 
	//  maps to new pixels
	// Also reload output color map 
	// Also reload initial conditions map

	EBString filename;
	HRESULT hr;

	
	if( pInitialTexture != NULL )
	{
		// Inc. ref count
		pInitialTexture->AddRef();

		m_pInitialStateTexture = pInitialTexture;

	}
	else
	{
		// load a default initial texture

		filename = INITIAL_FILENAME;

		hr = D3DXCreateTextureFromFile( m_pD3DDev, GetFilePath(filename.c_str()).c_str(),
										&m_pInitialStateTexture );

		ASSERT_IF_FAILED(hr);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't load texture: ";
			m_strLastError += filename;
			assert( false );
			return;
		}
	}


	filename = OUTPUT_MAP_FILENAME;

	hr = D3DXCreateTextureFromFile( m_pD3DDev, GetFilePath(filename.c_str()).c_str(),
									&m_pOutputMapTexture );

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



HRESULT CA_Fire::Initialize( LPDIRECT3DDEVICE8 pDev, ShaderManager * pManager, 
							 IDirect3DTexture8 * pInitialTexture )
{
	// Called when effect is selected from list in browser
	// Free() is called when effect is de-selected

	// pInitialTexture == texture which determines initial state AND
	//   resolution of the render targets on which this is run.  
	//   If NULL then a default start texture is loaded.

	assert( pDev != NULL );
	assert( pManager != NULL );

    int     i;
	HRESULT hr;


	m_pShaderManager = pManager;


	// get the device
	m_pD3DDev = pDev;
	pDev->AddRef();			// released on Free()



	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;

	m_pUI = new MouseUI((const RECT)rect);
	assert( m_pUI != NULL );

	m_pUI->SetTranslationalSensitivityFactor(0.0001f);

    m_pUI->Reset();

	///////////////////////////////////////////////


	m_bReset = true;
	m_bAnimate = true;
	m_bSingleStep = false;

	m_bSlow = false;

	m_bFarther = false;
	m_bWrap = true;

	m_bMulticolor = false;

	m_nSlowDelay = 7;

	m_eRenderMode = FULLSCREEN_FINALOUT;

	m_nSkipInterval = 0;
	m_nFlipState = 0;


	// scales colors during each blurred sampling
	m_fColorAtten = 0.4959f;
	m_fColorAtten = 0.487f;

	// texel scrolling offset per frame
	m_fTranslateAmount = 2.5f;



	m_pOutputMapTexture = NULL;

	/////////////////////////////////

	// create the vertex and pixel shaders
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());


	hr = m_pShaderManager->LoadAndCreateShader( GetFilePath("TexCoord_4_Offset.vso"),
							GetFilePath("TexCoord_4_Offset.nvv"), &Declaration[0], 0,
							SM_SHADERTYPE_VERTEX, &m_dwVertexShader);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;
	
    hr = m_pShaderManager->LoadAndCreateShader(GetFilePath("TwoWeightCombine_PostMult.pso"),
							GetFilePath("TwoWeightCombine_PostMult.nvp"), NULL, 0,
							SM_SHADERTYPE_PIXEL, &(m_dwTwoWeight_PostMultShader));
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	// Dependent 2D lookup pixel shader.
	// This one is used to re-map the color of the smoke

    hr = m_pShaderManager->LoadAndCreateShader(GetFilePath("DependentAR.pso"),
							GetFilePath("DependentAR.nvp"), NULL, 0,
							SM_SHADERTYPE_PIXEL, &(m_dwDependentAR ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	//////////////////////////////////////////////////////////////////////////
	// Another simple transform vertex shader for rendering the Simple..Object
	//  geometry that will be "on fire"
	
	// Fill in the format to correspond to that of a SimpleObjectVertex

	vector< DWORD > Decl2;
	Decl2.push_back( D3DVSD_STREAM(0) );
	Decl2.push_back( D3DVSD_REG(0, D3DVSDT_FLOAT3) );		// pos
	Decl2.push_back( D3DVSD_REG(1, D3DVSDT_FLOAT3) );		// normal
	Decl2.push_back( D3DVSD_REG(2, D3DVSDT_D3DCOLOR) );		// color
	Decl2.push_back( D3DVSD_REG(3, D3DVSDT_FLOAT2) );		// tex coord 2D
	Decl2.push_back( D3DVSD_END()		);


	hr = m_pShaderManager->LoadAndCreateShader( GetFilePath("SimpleObject_TexCoord_4_Offset.vso"),
							GetFilePath("SimpleObject_TexCoord_4_Offset.nvv"), &Decl2[0], 0,
							SM_SHADERTYPE_VERTEX, & m_SHI_SimpleObjVShader );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	/////////////////////////////////////////////////////////////////////////
    // get a pointer to the current back-buffer (so we can restore it later)
	m_pD3DDev->GetRenderTarget(        &mpBackbufferColor );
	m_pD3DDev->GetDepthStencilSurface( &mpBackbufferDepth );



	D3DXVECTOR4  commonconst( 0.0f, 0.5f, 1.0f, 2.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_CONSTS_1, &commonconst,    1);


	/////////////////////////////////


	// load the initial conditions texture map, and 
	//  optional final color re-mapping map

	LoadRulesAndOtherMaps( pInitialTexture );


	// Create additional texture surfaces to use as targets & 
	//  sources in the procedural animation

	hr = CreateTextureRenderTargets();
	ASSERT_IF_FAILED(hr);


	////////////////////////////////////////////////////////////////////////////

	D3DSURFACE_DESC ddsd;
    m_pInitialStateTexture->GetLevelDesc(0, &ddsd);

    CreateAndWriteUVOffsets(ddsd.Width, ddsd.Height);

	/////////////////////////////////////////////////////
	// Create an object for rendering the source embers 
	//  onto.  This could be a game character or anything
	//  "on fire"
	//* Ultimately, this fire effect class should not know
	//  or care about the nature of this object.  It could
	//  be any game object, but the work involved in
	//	abstracting this away is not warranted for this 
	//  first version of the technique.
	//* We should also use a zbuffer when rendering this
	//  object to the texture, but that is not done here.

	SimpleObject8 * pObj = new SimpleObject8;
	assert( pObj != NULL );

	pObj->InitTorus( 0.45f, 0.8f, 10, 15, 1.0f, 4.0f );

	m_pObj = new SimpleVBObject8( );
	assert( m_pObj != NULL );

	hr = m_pObj->CreateFromSimpleObject( pObj, m_pD3DDev );

	delete pObj;
	pObj = NULL;

	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	

	/////////////////////////////////////////////////////
	// Create a single quad object to use for render-to-texture
	//  operations.  The world-view-proj matrices scale this
	//  quad to exactly cover the render target.

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


HRESULT CA_Fire::Free()
{
	SAFE_RELEASE( m_pVertexBuffer);			// sets pointers to null after delete
	SAFE_RELEASE( m_pInitialStateTexture);
	SAFE_RELEASE( m_pOutputMapTexture );

	SAFE_DELETE(m_pUI);

	SAFE_DELETE( m_pObj );

	if (m_pD3DDev)
	{
		m_dwVertexShader = 0;
		m_SHI_SimpleObjVShader = 0;
		m_dwTwoWeight_PostMultShader = 0;
		m_dwDependentAR = 0;


        for (int i = 0; i < kMaxNumTargets; ++i)
        {            
            SAFE_RELEASE(mpFilterTarget[i]);
            SAFE_RELEASE(mpTextureFiltered[i]);
        }

        SAFE_RELEASE(mpBackbufferColor);
		SAFE_RELEASE(mpBackbufferDepth);

        SAFE_RELEASE(m_pD3DDev);		// we AddRef()'d in Initialize
	}
	

	m_pShaderManager = NULL;

	return S_OK;
}


HRESULT CA_Fire::Start()
{
	return S_OK;
}



std::string CA_Fire::GetFilePath( const std::string& strFileName )
{
	return(	effect_api::GetFilePath( strFileName ) );
}



HRESULT CA_Fire::CreateTextureRenderTargets()
{
    HRESULT         hr;

	_ASSERT( mpBackbufferColor != NULL );
	_ASSERT( mpBackbufferDepth != NULL );

    // get the description for the texture we want to filter
	D3DSURFACE_DESC ddsd;
    m_pInitialStateTexture->GetLevelDesc(0, &ddsd);

	int i;

	for( i=0; i < kMaxNumTargets; i++ )
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
    for( i = 0; i < kMaxNumTargets; i++ )
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


HRESULT		CA_Fire::SetPixelShader( DWORD dwIndex )
{
	HRESULT hr = S_OK;

	hr = m_pD3DDev->SetPixelShader( m_pShaderManager->GetShaderHandle( dwIndex ) );
	ASSERT_IF_FAILED(hr);

	return hr;
}


HRESULT		CA_Fire::SetVertexShader( DWORD dwIndex )
{
	HRESULT hr = S_OK;

	hr = m_pD3DDev->SetVertexShader( m_pShaderManager->GetShaderHandle( dwIndex ) );
	ASSERT_IF_FAILED(hr);

	return hr;
}



bool CA_Fire::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	// Returns true if keyboard command was processed


	char charcode;

	if( bDown )
	{
		// Use only capital letters here - these are VK_ codes!

		switch( dwKey )
		{
		case VK_LEFT:
			if( m_nSlowDelay > 0 )
				m_nSlowDelay--;
			m_bSlow = true;
			FDebug("frame rate slow delay: %d\n", m_nSlowDelay );
			return( true );			// keyboard command was processed
			break;
		case VK_RIGHT:
			if( m_nSlowDelay < 500 )
				m_nSlowDelay++;
			m_bSlow = true;
			FDebug("frame rate slow delay: %d\n", m_nSlowDelay );
			return( true );
			break;

		case 'C':
			m_bMulticolor = !m_bMulticolor;
			return( true );
			break;

		case 'B':
			m_bWrap = !m_bWrap;
			FDebug("Wrap mode:  %d\n", m_bWrap );
			return( true );
			break;

		case 'M':
			m_nSkipInterval++;
			FDebug("Skip Interval:  %d\n", m_nSkipInterval );
			return( true );
			break;
		case 'N':
			m_nSkipInterval--;
			if( m_nSkipInterval < 0 )
				m_nSkipInterval = 0;
			FDebug("Skip Interval:  %d\n", m_nSkipInterval );
			return( true );
			break;

		case '1' :
			m_eRenderMode = FULLSCREEN_FINALOUT;
			return( true );
			break;

		case '2':
			m_eRenderMode = FULLSCREEN_EMBER_TEXTURE;
			return( true );
			break;

		case '3' :
			m_eRenderMode = ALL_TOGETHER;
			return( true );
			break;

		case 'F':
			m_bFarther = !m_bFarther;
			return( true );
			break;

		case 'R':
		case '`':
		case '~':
			m_bReset = true;

			return( true );
			break;
		case 'G':
			// start or stop the animation
			m_bAnimate = !m_bAnimate;
			return( true );
			break;
		case VK_SPACE:
			m_bSingleStep = !m_bSingleStep;
			return( true );
			break;
		case 'S':
			m_bSlow = !m_bSlow;
			return( true );
			break;

		default:

			charcode = MapVirtualKey( dwKey, 2 ); // VK to char code

			switch( charcode )
			{
			case '[':
			case '{':
				m_fColorAtten *= 0.999f;
				FDebug("blend:  %f\n", m_fColorAtten );
				return( true );
				break;

			case ']':
			case '}':
				m_fColorAtten *= 1.001f;
				FDebug("blend:  %f\n", m_fColorAtten );
				return( true );
				break;

			case '-':
				m_fTranslateAmount *= 0.99f;
				FDebug("m_fTranslateAmount:  %f\n", m_fTranslateAmount );
				return( true );
				break;

			case '=':
				m_fTranslateAmount *= 1.01f;
				FDebug("m_fTranslateAmount:  %f\n", m_fTranslateAmount );
				return( true );
				break;

			};
		};
	}

	return( false );		// keyboard wasn't processed

}








void CA_Fire::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pUI->OnLButtonDown(x, y);
		}
		else
		{
			m_pUI->OnLButtonUp(x, y);
		}
	}
}


void CA_Fire::MouseMove(HWND hWnd, int x, int y)
{
	if( m_pUI != NULL )
		m_pUI->OnMouseMove(x, y);

	return;
}


IDirect3DTexture8 *	CA_Fire::GetOutputTexture()
{
	assert( m_pOutputSrc != NULL );
	return( m_pOutputSrc );
}

void	CA_Fire::SetInputTexture( IDirect3DTexture8 * pTex )
{
	m_pInputTexture = pTex;
}


HRESULT CA_Fire::Tick()
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
	SetVertexShader(m_dwVertexShader);
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





	if( m_bReset == true )
	{
		m_bReset = false;
		m_nFlipState = 0;

		m_bSingleStep = true;	// if animation stopped, render a single step
								//  after reseting
	}


	int tmp;

	if( m_nFlipState == 0 )
	{

		m_nTarget1		= 0;
		m_nTarget2		= 1;

		// Need to set textures to null so that when SetRenderTarget is
		//  called, none of the render targets are bound as textures.
		hr = m_pD3DDev->SetTexture(0, NULL );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTexture(1, NULL );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTexture(2, NULL );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTexture(3, NULL );
		ASSERT_IF_FAILED(hr);

		// Clear the textures
		hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTarget1], NULL);
		ASSERT_IF_FAILED( hr );
		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x00, 0x00 ), 1.0, 0);

		hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTarget2], NULL);
		ASSERT_IF_FAILED( hr );
		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x00, 0x00 ), 1.0, 0);


	}
	else if(  m_bAnimate == true || m_bSingleStep == true )
	{
		switch( m_nFlipState )
		{
		case 1:
			// flip-flop between source and dest
			//  textures

			tmp = m_nTarget2;
			m_nTarget2		= m_nTarget1;
			m_nTarget1		= tmp;
			
			break;
		}
	}





	if( m_bAnimate == true  )
	{
		// Update the textures for one step of the 
		//   game or cellular automata 

		DoSingleTimeStep();

	}
	else if( m_bSingleStep == true )
	{
		DoSingleTimeStep();

		m_bSingleStep = false;

	}
	else
	{
		// slow down the rendering and give any other apps a chance
		Sleep(70);
	}

	if( m_bSlow && (m_nSlowDelay > 0) )
	{
		Sleep(m_nSlowDelay);
	}



    IDirect3DTexture8       * pSrcTex;

	static int nSkip = 0;

	if( nSkip >= m_nSkipInterval && (m_eRenderMode != DO_NOT_RENDER))
	{
		nSkip = 0;

		// Switch back to normal rendering to display the results of 
		//   the rendering to texture

		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


		hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
		ASSERT_IF_FAILED( hr );


		// turn off pixel shading
		SetPixelShader(0);	

		

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

		case FULLSCREEN_EMBER_TEXTURE:

			// Draw quad over full display
			D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);

			D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, m_pInputTexture );
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

			m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x00, 0x00 ), 1.0, 0);

			// draw quads in the other corners, use generated textures
			for (int j = 0; j < 3; ++j)
			{
				D3DXMatrixTranslation(&matWorld, (j == 2) ? -1.0f :  1.0f, 
												 (j == 0) ?  1.0f : -1.0,
												 0.0f);
				D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
				D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
				m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

				pSrcTex = m_pInputTexture;			// upper right corner image

				if( m_bAnimate )
				{
					if( j==1 )
						pSrcTex = mpTextureFiltered[m_nTarget1];	// low right corner
					else if( j==2 )
						pSrcTex = mpTextureFiltered[m_nTarget2];	// low left corner
				}
				else
				{
					assert( 1 < kMaxNumTargets );

					if( j==1 )
						pSrcTex = mpTextureFiltered[0];	// low right corner
					else if( j==2 )
						pSrcTex = mpTextureFiltered[1];	// low left corner
				}


				if( pSrcTex != 0 )
				{
					m_pD3DDev->SetTexture(0, pSrcTex );
					hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
				}
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




HRESULT CA_Fire::DoSingleTimeStep()
{

	HRESULT hr;
	int i;

	float pix_mult[4];


    // variable for writing to constant memory which uv-offsets to use
    D3DXVECTOR4     offset(0.0f, 1.0f, 0.0f, 0.0f);

	m_pD3DDev->SetVertexShaderConstant( CV_ONOFF_1, &offset,    1);

	DWORD	wrapval;





	//////////////////////////////////////////////////////////////////
	// Need to set textures to null so that when SetRenderTarget is
	//  called, none of the render targets are bound as textures.
	hr = m_pD3DDev->SetTexture(0, NULL );
	ASSERT_IF_FAILED(hr);
	hr = m_pD3DDev->SetTexture(1, NULL );
	ASSERT_IF_FAILED(hr);
	hr = m_pD3DDev->SetTexture(2, NULL );
	ASSERT_IF_FAILED(hr);
	hr = m_pD3DDev->SetTexture(3, NULL );
	ASSERT_IF_FAILED(hr);
	

	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTarget1], NULL );
	ASSERT_IF_FAILED(hr);


	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matTemp;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;


	// Render embers into newly cleared texture, blending old texture
	//   on top of the embers

	if( m_pInputTexture != NULL )
	{
		//////////////////////////////////////////////////////
		//  Set matrix for rendering embers into texture
		//    on a quad viewed in perspective from various
		//    orientations controlled by mouse


		float height = 0.45f;

		D3DXVECTOR3 vNEWEyePt    = D3DXVECTOR3( 0.0f, height,	-2.5f );
		D3DXVECTOR3 vNEWLookatPt = D3DXVECTOR3( 0.0f, height,	0.0f );
		D3DXVECTOR3 vNEWUp       = D3DXVECTOR3( 0.0f, 1.0f,		0.0f );


		// Set World, View, Projection, and combination matrices.
		D3DXMatrixLookAtLH(&matView, &vNEWEyePt, &vNEWLookatPt, &vNEWUp);


		D3DXMatrixPerspectiveFovLH( &matProj, 3.1415f/5.0f, 1.0f, 0.5f, 300.0f );


		D3DXMatrixRotationX( &matTemp, 3.14159f * 0.5f );


		D3DXMatrixMultiply(&matViewProj, &matView, &matProj);


		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixMultiply( &matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );

		// Apply mouse UI rotation after initial X axis rotation in matTemp

		D3DXMatrixMultiply( &matWorld, &matTemp, &matWorld );


		D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
		D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);


		m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

		// Render 1st using offset set 1 - for nearest neighbor pixels
		offset.x = 1.0f;
		m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

		m_pD3DDev->SetTexture(0, m_pInputTexture );
		m_pD3DDev->SetTexture(1, m_pInputTexture );
		m_pD3DDev->SetTexture(2, m_pInputTexture );
		m_pD3DDev->SetTexture(3, m_pInputTexture );


		pix_mult[0] = 0.45f;		// red 
		pix_mult[1] = 0.15f;		// green
		pix_mult[2] = 0.05f;		// blue
		pix_mult[3] = 0.0f;			// alpha

		hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_1, &pix_mult, 1 );
		ASSERT_IF_FAILED(hr);


		pix_mult[0] = 0.45f;		// red 
		pix_mult[1] = 0.15f;		// green
		pix_mult[2] = 0.05f;		// blue
		pix_mult[3] = 0.0f;			// alpha

		hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_2, &pix_mult, 1 );
		ASSERT_IF_FAILED(hr);



		hr = SetPixelShader( m_dwTwoWeight_PostMultShader );	
		ASSERT_IF_FAILED( hr );

		offset.x = 1.0f;
		m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


		if( m_bWireframe )
		{
			// special consideration for showing wireframe object being
			//   rendered as the source embers

			hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
			ASSERT_IF_FAILED( hr );


			hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0x0, 0x00, 0x00 ), 1.0, 0);
			ASSERT_IF_FAILED(hr);

			SetPixelShader( 0 );
				
			m_pD3DDev->SetRenderState(D3DRS_FILLMODE,	D3DFILL_WIREFRAME );

			// chances are the texture will be all dark, so render in solid color
			m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR,  0xFFFFFFFF );

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR	);

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
			m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );


			// Draw object on which embers would appear
			// This version for wireframe mode

			bool drawtorus = true;

			if( drawtorus )
			{
				SetVertexShader( m_SHI_SimpleObjVShader );
	
				m_pObj->Draw();
			}
			else		// draw flat plane
			{

				hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
				ASSERT_IF_FAILED(hr);

			}

			return( S_OK );
		}
		else
		{
			hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x00, 0x00 ), 1.0, 0);
			ASSERT_IF_FAILED(hr);


			// Add result into the dest texture:
			m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
			m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );


			for( i=0; i < 4 ; i++ )
			{
				hr = m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP);
				hr = m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP);
			}

			// Draws "embers" into the texture surface as though they
			//   are viewed in perspective
			// draw the fan with displaced texture coordinates

			bool drawtorus = true;

			if( drawtorus )
			{
				SetVertexShader( m_SHI_SimpleObjVShader );
	
				m_pObj->Draw();
			}
			else		// draw flat plane
			{

				hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
				ASSERT_IF_FAILED(hr);

			}
		}
	}

	/////////////////////////////////////////////////////////////

	wrapval = m_bWrap ?  D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;

	for( i=0; i < 4 ; i++ )
	{
		hr = m_pD3DDev->SetTextureStageState(i, D3DTSS_BORDERCOLOR, 0x00000000);
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSU,  wrapval);
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_BORDER);
		ASSERT_IF_FAILED(hr);
	}
	
	/////////////////////////////////////////////////////////////
	// Set to draw full coverage quad for the render-to-texture
	//  passes:

	SetVertexShader(m_dwVertexShader);
	m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));

	/////////////////////////////////////////////////////////////

	// Set the TC warping on/off
	offset.x = 1.0f;
	offset.y = 0.0f;

	m_pD3DDev->SetVertexShaderConstant( CV_ONOFF_1, &offset,    1);


	float hoff, woff;

	woff =  m_fPerTexelWidth/2.0f;
	hoff =  m_fPerTexelHeight/2.0f;

	hoff += m_fTranslateAmount * m_fPerTexelHeight;

	static float xp = 0;
	static float yp = 0;

	float xlim = m_fPerTexelWidth  * 0.5f;
	float ylim = m_fPerTexelHeight * 1.0f;


	float xdmag;
	float ydmag;

	xdmag = 0.0001f;
	ydmag = 0.005f;

	xdmag = 0.005f;
	xdmag = 0.0001f;
	ydmag = 0.005f;

	// alter degree of jittering by the translate amount
	//  so when less translation, you also get less rapid jittering

	xdmag *= m_fTranslateAmount / 2.5f;
	ydmag *= m_fTranslateAmount / 2.5f;


	// Brownian noise motion for texel sampling offset vector

	xp += xdmag * ( 1.0f - 2.0f * rand()/((float)RAND_MAX));
	yp += ydmag * ( 1.0f - 2.0f * rand()/((float)RAND_MAX));

	if( xp > xlim ) xp = xlim;
	if( yp > ylim ) yp = ylim;
	if( xp < -xlim ) xp = -xlim;
	if( yp < -ylim ) yp = -ylim;

	hoff += yp;
	woff += xp;

	float		type4OffsetX[4] = { -  m_fPerTexelWidth	+ woff, 
                                       m_fPerTexelWidth	+ woff,  
                                       0.0f	+ woff,
									   0.0f	+ woff };

    float		type4OffsetY[4] = {    0.0f		+ hoff, 
                                       0.0f		+ hoff,
									   m_fPerTexelHeight		+ hoff, 
                                    -  m_fPerTexelHeight		+ hoff  };


    // write all these offsets to constant memory
    for ( i = 0; i < 4; ++i)
    {
        D3DXVECTOR4  type4Offset(type4OffsetX[i], type4OffsetY[i], 0.0f, 0.0f);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE4     + 5*i, &type4Offset, 1);
    }


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




	// add blurred previous output texture
	
	// Add result of pixel operations into the dest texture:
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );


	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR		);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR		);
	}


	pix_mult[0] = m_fColorAtten * 1.0;		// red 
	pix_mult[1] = m_fColorAtten * 1.0;		// green
	pix_mult[2] = m_fColorAtten * 1.0;		// green
	pix_mult[3] = 0.0f;						// alpha

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_2, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);

	if( m_bMulticolor )
	{
		pix_mult[0] = m_fColorAtten * 0.95f;		// red 
		pix_mult[1] = m_fColorAtten * 1.05f;		// green
		pix_mult[2] = m_fColorAtten * 1.09f;		// blue
		pix_mult[3] = 0.0f;							// alpha
	}

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_1, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);


	// Render 1st using offset set 1 - for nearest neighbor pixels
	offset.x = 1.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTarget2] );
    m_pD3DDev->SetTexture(1, mpTextureFiltered[m_nTarget2] );
    m_pD3DDev->SetTexture(2, mpTextureFiltered[m_nTarget2] );
    m_pD3DDev->SetTexture(3, mpTextureFiltered[m_nTarget2] );



	m_pOutputSrc = mpTextureFiltered[m_nTarget1];


	offset.x = 4.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);


	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_POINT		);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_POINT		);
	}




	/////////////////////////////////////////////////////////////////
	// If further processing is requested, do 1 more dependent 
	//   lookup into a colormap for display to the user only.
	//   This remapping doesn't affect the logic at all, it's only
	//   a prettier version for the user to see.

	if( m_bFarther )
	{

		hr = SetPixelShader( m_dwDependentAR );
		ASSERT_IF_FAILED( hr );


		hr = m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTarget1] );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTexture(1, m_pOutputMapTexture );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTexture(2, NULL );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetTexture(3, NULL );
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTarget2], NULL);
		ASSERT_IF_FAILED(hr);

		m_pOutputSrc = mpTextureFiltered[m_nTarget2];


		// Render using offsets of zero
		offset.x = 0.0f;
		m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);



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
		m_nFlipState = 1;
		break;
	}


	return hr;



}



void CA_Fire::CreateAndWriteUVOffsets(int width, int height)
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

    
    m_fPerTexelWidth  = 1.0f/static_cast<float>(width);
    m_fPerTexelHeight = 1.0f/static_cast<float>(height);
	

	HRESULT hr;

	// Change the multiplier to 2 or 3 to spread the texel sampling.  This
	//  has the effect of running several independent "games" in neighboring
	//  texels, which do not affect eachother.

	m_fPerTexelWidth	*= 1;	// 1 = no change - run standard game of life
    m_fPerTexelHeight *= 1;


	float woff, hoff;
	woff =  m_fPerTexelWidth/2.0f;
	hoff =  m_fPerTexelHeight/2.0f;

	// Ofset set 0 : center texel sampling
    float      noOffsetX[4] = { woff,woff,woff,woff };
    float      noOffsetY[4] = { hoff,hoff,hoff,hoff };


	// Offset set 1:  Nearest neighbors -  b,d,f,h texels
	// Use index offset 1.0 to access these

    float		type1OffsetX[4] = { -  m_fPerTexelWidth + woff, 
                                       m_fPerTexelWidth + woff,  
                                       0.0f + woff,
									   0.0f + woff };

    float		type1OffsetY[4] = {    0.0f + hoff, 
                                       0.0f + hoff,
									   m_fPerTexelHeight + hoff, 
                                    -  m_fPerTexelHeight + hoff  };


	// These are a,c,g,i texels == diagonal neightbors
	// Use index offset 2.0 to use these

    float		type2OffsetX[4] = { -  m_fPerTexelWidth, 
                                    -  m_fPerTexelWidth,  
                                       m_fPerTexelWidth,   
                                       m_fPerTexelWidth  };
    float		type2OffsetY[4] = { -  m_fPerTexelHeight, 
                                       m_fPerTexelHeight, 
                                       m_fPerTexelHeight, 
                                    -  m_fPerTexelHeight };
	
	// These offsets are for use with bilinear filtering
	//   of the neighbors, to sample all 8 neighbors in
	//   one pass instead of two.  Bilinear averages the
	//   two bordering texels, but the coordinate must be
	//   exactly on the texel border to make this work.
	//		[0] = on the border of the ab texels
	//      [1] = between cf texels
	//		[2] = between ih texels
	//		[3] = between gd texels



    float      type3OffsetX[4] = {	- m_fPerTexelWidth/2.0f	+ woff,
									  m_fPerTexelWidth		+ woff,
									  m_fPerTexelWidth/2.0f	+ woff,
									- m_fPerTexelWidth		+ woff	};
	
    float      type3OffsetY[4] = {	- m_fPerTexelHeight		+ hoff,
									- m_fPerTexelHeight/2.0f	+ hoff,
									  m_fPerTexelHeight		+ hoff,
									  m_fPerTexelHeight/2.0f	+ hoff	};


	// Offset that has the effect of scrolling the texture as
	//   it is sampled

	hoff += -3.5f * m_fPerTexelHeight;

	float		type4OffsetX[4] = { -  m_fPerTexelWidth	+ woff, 
                                       m_fPerTexelWidth	+ woff,  
                                       0.0f	+ woff,
									   0.0f	+ woff };

    float		type4OffsetY[4] = {    0.0f		+ hoff, 
                                       0.0f		+ hoff,
									   m_fPerTexelHeight		+ hoff, 
                                    -  m_fPerTexelHeight		+ hoff  };


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


	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_1, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_2, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);

}






