/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_GameOfLife
File:  CA_GameOfLife.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
4/27/01
No longer requires an EBEffect * to be passed in.  Uses global effect_api::
	GetFilePath() and load shader functions.  Joy!
If all .vso, .pso files are in directory of the executable, then the GetFilePath()
	call can be removed --- just use the straight file names.

This class is now more portable to any application wanting to do procedural
    texture animation.
I'd still like to separate the UI into another class, but this will have to wait.


******************************************************************************/


#include <crtdbg.h>


#include "eb_file.h"

#include <vector>
#include "CA_GameOfLife.h"
#include "..\..\..\CommonSrc\NV_Error.h"			// for FDebug

#include "Constants.h"
#include "PixelConstants.h"

#include "ShaderManager.h"


using namespace std;


#define RULE_TABLE_NAME		"RuleTable.bmp"
#define INITIAL_FILENAME	"start.bmp"
#define OUTPUT_MAP_FILENAME	"OutputColors.bmp"


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


CA_GameOfLife::CA_GameOfLife()
{

	m_bWireframe			= false;
	m_pInitialStateTexture	= NULL;
	m_pRulesTexture			= NULL ;

	m_SHI_4Offset_VertexShader		= 0;
	m_SHI_EqualWeightCombineShader	= 0;
	m_SHI_DependentGB				= 0;
	m_SHI_TexelsToBlueWithBias		= 0;

	m_pVertexBuffer			= NULL;
	mpBackbufferColor		= 0 ;
	mpBackbufferDepth		= 0 ;

	m_pD3DDev				= NULL;

	for ( int i = 0; i < kMaxNumTargets; ++i )
	{
        mpTextureFiltered[i] = 0;
		mpFilterTarget   [i] = 0;
	}

}

CA_GameOfLife::~CA_GameOfLife()
{
	Free();	
}

/////////////////////////////////////////////////////////


HRESULT CA_GameOfLife::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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


void CA_GameOfLife::UpdateProperties()
{

}


void CA_GameOfLife::LoadRulesAndOtherMaps()
{
	// Load "rules" texture which determines how the neighbor accumulation 
	//  maps to new pixels
	// Also reload output color map 
	// Also reload initial conditions map

	EBString filename;
	HRESULT hr;

	if( m_pRulesTexture != NULL )
	{
		SAFE_RELEASE( m_pRulesTexture );
	}

	if( m_pInitialStateTexture != NULL )
	{
		SAFE_RELEASE( m_pInitialStateTexture );
	}

	////////////////////////////////////////////////

	filename = RULE_TABLE_NAME;

	hr = D3DXCreateTextureFromFile( m_pD3DDev, GetFilePath(filename.c_str()).c_str(),
									&m_pRulesTexture );

	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't load texture: ";
		m_strLastError += filename;
		assert( false );
		return;
	}

	////////////////////////////////////////////////

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


	//  Not re-creating render targets.  This could be problem if 
	//   you reload an initial texture with different resolution while
	//   the simulation is running.  The new resolution will not be 
	//   reflected in the render targets

}



HRESULT CA_GameOfLife::Initialize( LPDIRECT3DDEVICE8 pDev, ShaderManager * pManager )
{
	// Called when effect is selected from list in browser
	// Free() is called when effect is de-selected

	assert( pDev != NULL );
	assert( pManager != NULL );

	// Free everything in case we're already initialized
	Free();


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


	///////////////////////////////////////////////


	m_bReset		= true;
	m_bAnimate		= true;
	m_bSingleStep	= false;

	m_bSlow			= false;

	m_bWrap			= false;

	m_bOccasionalExcitation	= false;


	m_nSlowDelay = 25;

	m_eRenderMode = FULLSCREEN_NEIGHBOR_CALC;

	m_nSkipInterval = 0;
	m_nFlipState = 0;


	/////////////////////////////////


	// create the vertex and pixel shaders
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());


	hr = m_pShaderManager->LoadAndCreateShader( GetFilePath("TexCoord_4_Offset.vso"),
							GetFilePath("TexCoord_4_Offset.nvv"), &Declaration[0], 0,
							SM_SHADERTYPE_VERTEX, &m_SHI_4Offset_VertexShader);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;
	

	// Shader for 1st step in accumulating neighbor information
    hr = m_pShaderManager->LoadAndCreateShader(GetFilePath("TexelsToBlueWithBias.pso"),
							GetFilePath("TexelsToBlueWithBias.nvp"), NULL, 0,
							SM_SHADERTYPE_PIXEL, &( m_SHI_TexelsToBlueWithBias ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


    hr = m_pShaderManager->LoadAndCreateShader(GetFilePath("EqualWeightCombine.pso"),
							GetFilePath("EqualWeightCombine.nvp"), NULL, 0,
							SM_SHADERTYPE_PIXEL, &(m_SHI_EqualWeightCombineShader));
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	// Dependent 2D lookup pixel shader for logic rules:
    hr = m_pShaderManager->LoadAndCreateShader(GetFilePath("DependentGB.pso"),
							GetFilePath("DependentGB.nvp"), NULL, 0,
							SM_SHADERTYPE_PIXEL, &(m_SHI_DependentGB ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;





    // get a pointer to the current back-buffer (so we can restore it later)
	m_pD3DDev->GetRenderTarget(        &mpBackbufferColor );
	m_pD3DDev->GetDepthStencilSurface( &mpBackbufferDepth );



	D3DXVECTOR4  commonconst( 0.0f, 0.5f, 1.0f, 2.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_CONSTS_1, &commonconst,    1);


	/////////////////////////////////


	// load the initial conditions texture map, rules map, and 
	//  optional final color re-mapping map

	// The "rules" texture determines how the neighbor accumulation 
	//  maps to new pixels in the next generation

	LoadRulesAndOtherMaps();

	hr = CreateTextureRenderTarget();
	ASSERT_IF_FAILED(hr);


	////////////////////////////////////////////////////////////////////////////


	D3DSURFACE_DESC ddsd;
    m_pInitialStateTexture->GetLevelDesc(0, &ddsd);

    CreateAndWriteUVOffsets(ddsd.Width, ddsd.Height);


	////////////////////////////////////////////////////////////////////////////
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


HRESULT CA_GameOfLife::Free()
{
	SAFE_RELEASE( m_pVertexBuffer );			// sets pointers to null after delete
	SAFE_RELEASE( m_pInitialStateTexture );
	SAFE_RELEASE( m_pRulesTexture );


	if (m_pD3DDev)
	{
		m_SHI_4Offset_VertexShader = 0;
		m_SHI_EqualWeightCombineShader = 0;
		m_SHI_DependentGB = 0;
		m_SHI_TexelsToBlueWithBias = 0;

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


HRESULT CA_GameOfLife::Start()
{
	return S_OK;
}



std::string CA_GameOfLife::GetFilePath( const std::string& strFileName )
{
	return(	effect_api::GetFilePath( strFileName ) );
}



HRESULT CA_GameOfLife::CreateTextureRenderTarget()
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

        hr = m_pD3DDev->SetRenderTarget(mpFilterTarget[i], NULL);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't SetRenderTarget to new surface!\n";
            assert(false);
            return E_FAIL;
        }
	
		m_pTexFinalSrc = mpTextureFiltered[i];
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


HRESULT		CA_GameOfLife::SetPixelShader( SHADER_INDEX dwIndex )
{
	HRESULT hr = S_OK;

	hr = m_pD3DDev->SetPixelShader( m_pShaderManager->GetShaderHandle( dwIndex ) );
	ASSERT_IF_FAILED(hr);

	return hr;
}


HRESULT		CA_GameOfLife::SetVertexShader( SHADER_INDEX dwIndex )
{
	HRESULT hr = S_OK;

	hr = m_pD3DDev->SetVertexShader( m_pShaderManager->GetShaderHandle( dwIndex ) );
	ASSERT_IF_FAILED(hr);

	return hr;
}



bool CA_GameOfLife::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
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
			m_eRenderMode = FULLSCREEN_NEIGHBOR_CALC;
			return( true );
			break;

		case '2':
			m_eRenderMode = FULLSCREEN_RESULT;
			return( true );
			break;

		case '3' :
			m_eRenderMode = ALL_TOGETHER;
			return( true );
			break;
		case '4':
			m_eRenderMode = FULLSCREEN_FINALOUT;
			return( true );
			break;


		case 'R':
		case '`':
		case '~':
			FDebug("Loading new rules texture map\n");
			LoadRulesAndOtherMaps();
			return( true );
			break;

		case VK_SPACE:
			// start or stop the animation
			m_bAnimate = !m_bAnimate;
			return( true );
			break;

		case VK_RETURN:
			m_bAnimate = false;
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
				break;

			case ']':
			case '}':
				break;

			case '-':
				break;

			case '=':
				break;

			};
		};
	}

	return( false );		// keyboard wasn't processed

}




IDirect3DTexture8 *	CA_GameOfLife::GetOutputTexture()
{
	assert( m_pTexFinalSrc != NULL );
	return( m_pTexFinalSrc );
}


HRESULT CA_GameOfLife::Tick()
{	
	// Render a new frame

	HRESULT hr;

    m_pD3DDev->SetRenderState(D3DRS_ZENABLE,			FALSE );


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
	SetVertexShader(m_SHI_4Offset_VertexShader);
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


	if( m_nFlipState == 0 )
	{

		m_pTexSrc			= m_pInitialStateTexture;

		m_nTargetIntermediate = 0;
		m_nTargetCellField    = 1;

		m_pTexFinalSrc		= mpTextureFiltered[1];

	}
	else if(  m_bAnimate == true || m_bSingleStep == true )
	{
		switch( m_nFlipState )
		{
		case 1:
			// Here, over a single time step, rendering goes
			//   from source field to intermediate texture,
			//   then back to source field (new generation)
			//   So there is only one flipstate besides the
			//   initial one of "0" which uses the initial
			//   conditions texture.


			m_nTargetIntermediate = 0;
			m_nTargetCellField    = 1;

			m_pTexSrc			= mpTextureFiltered[m_nTargetCellField];

			m_pTexFinalSrc		= mpTextureFiltered[1];

			break;

		}
	}





	if( m_bAnimate == true  )
	{
		// Update the textures for one step of the 
		//   game or cellular automata 

		DoSingleTimeStep_3Pass();

	}
	else if( m_bSingleStep == true )
	{
		DoSingleTimeStep_3Pass();

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

	if( nSkip >= m_nSkipInterval && (m_eRenderMode != DO_NOT_RENDER) )
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

			hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x0, 0x00, 0x00 ), 1.0, 0);
			ASSERT_IF_FAILED(hr);

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

			m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTargetCellField] );
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

			m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTargetCellField] );
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

			m_pD3DDev->SetTexture(0, mpTextureFiltered[ m_nTargetIntermediate ] );
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

			m_pD3DDev->SetTexture(0, m_pInitialStateTexture );
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


				pSrcTex = mpTextureFiltered[m_nTargetIntermediate];	// upper right corner


				if( j==1 )
					pSrcTex = mpTextureFiltered[m_nTargetCellField]; // lower right
				else if( j==2 )
					pSrcTex = mpTextureFiltered[m_nTargetCellField]; // lower left


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


	return( S_OK );
}




HRESULT CA_GameOfLife::DoSingleTimeStep_3Pass()
{

	HRESULT hr;
	int i;

	float pix_mult[4];


    // variable for writing to constant memory which uv-offsets to use
    D3DXVECTOR4     offset(0.0f, 1.0f, 0.0f, 0.0f);

	m_pD3DDev->SetVertexShaderConstant( CV_ONOFF_1, &offset,    1);


	// even if wireframe mode, render to texture as solid
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE,			D3DFILL_SOLID );

	/////////////////////////////////////////////////////////////////

	DWORD wrapval = m_bWrap ?  D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
	for( i=0; i < 4 ; i++ )
	{
		m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSU,  wrapval);
		m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSV,  wrapval);
	}


	/////////////////////////////////////////////////////////
	// First, write the source texture into the blue channel
	// I do this in preparation for a 2D dependent green-blue lookup
	//   into a "rules" texture which governs how old pixels spawn
	//   or die into new pixels.  
	// The logic for the game of life depends on 9 pixels:  the source
	//   pixel and it's 8 neightbors.  These are accumulated in three
	//   passes.


	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTargetIntermediate], NULL);


	//  Set simple pixel shader to multiply input texture by RGB = 0,0,1
	//   to select only blue, and add a small component of green + blue to 
	//   that in order to properly offset the dependent green-blue lookup
	//   to be done in a later stage

	SetPixelShader( m_SHI_TexelsToBlueWithBias );


	m_pD3DDev->SetTexture(0, m_pTexSrc );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


	// Render using offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	// draw the fan with normal texture coords
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);



	////////////////////////////////////////////////////
	//	Excitation of Game Of Life simulation
	//	Every 400 steps draw the cell field into itself
	//   at offset coordinates to spawn new activity.

	static int count = 0;

	if( m_bOccasionalExcitation )
	{
		count++;

		if( count > 400 )
		{
			// Additive blending
			m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
			m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

			// Render using scrolling offsets of a few texels
			offset.x = 4.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			// draw the fan with normal texture coords
			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			count = 0;
		}
	}


	
	/////////////////////////////////////////////////////////////////
	// Render 1st set of neighbors
	// Add in nearest neighbors


	hr = SetPixelShader( m_SHI_EqualWeightCombineShader );	

	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTargetIntermediate], NULL);
	ASSERT_IF_FAILED(hr);

	// Add result of pixel operations into the dest texture:
	// This adds the new green component from neighbor sampling
	//   into the previous blue on/off result from above

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Render set of neighbors using bilinear filtering to sample
	//  equal weight of all 8 neighbors in just 4 texture samples

	offset.x = 3.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	pix_mult[0] = 0.0f;		// red 
	pix_mult[1] = 0.25f;	// green
	pix_mult[2] = 0.0f;		// blue
	pix_mult[3] = 0.0f;		// alpha

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_1, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_2, &pix_mult, 1 );
	ASSERT_IF_FAILED(hr);
	


	m_pD3DDev->SetTexture(0, m_pTexSrc );
    m_pD3DDev->SetTexture(1, m_pTexSrc );
    m_pD3DDev->SetTexture(2, m_pTexSrc );
    m_pD3DDev->SetTexture(3, m_pTexSrc );

	// Enable bilinear to count 8 neighbors with 4 samples
	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR		);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR		);
	}


	// draw the fan with displaced texture coordinates
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// Now do dependent 2D lookup into rules texture to set new
	//   source pixels;

	// Write white colors to final target

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	
	hr = SetPixelShader( m_SHI_DependentGB );
	ASSERT_IF_FAILED( hr );

	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTargetCellField], NULL);
	ASSERT_IF_FAILED(hr);

	// Render using offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	m_pD3DDev->SetTexture(0, mpTextureFiltered[ m_nTargetIntermediate ] );
	m_pD3DDev->SetTexture(1, m_pRulesTexture );


	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT	);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT	);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT	);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT	);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);


	// Draw the quad to render in all new cells 
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);


	////////////////////////////////////////////////////////////
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



void CA_GameOfLife::CreateAndWriteUVOffsets(int width, int height)
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


	// drifing uppder
//	hoff += -m_fPerTexelHeight;
/*
    float      type4OffsetX[4] = {	- m_fPerTexelWidth	+ woff,
									  m_fPerTexelWidth	+ woff,
									  0.0f				+ woff,
									  0.0f				+ woff	};
	
    float      type4OffsetY[4] = {	  0.0f				+ hoff,
									  0.0f				+ hoff,
									  m_fPerTexelHeight	+ hoff,
									- m_fPerTexelHeight	+ hoff	};
*/

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






