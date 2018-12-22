/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_RGB_Plasmas
File:  CA_Water.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
3/17/01 - Greg James - This is fun stuff!

The class is getting a bit bloated & could use a good parent class.

******************************************************************************/



#include <crtdbg.h>


#include "eb_file.h"		// for GetFilePath() and Load..Shader()

#include "CA_Water.h"

#include "nvtex.h"
#include "..\..\..\CommonSrc\NV_Error.h"			// for FDebug

#include "Constants.h"
#include "PixelConstants.h"


using namespace nv_objects;
using namespace std;


// Resolution of the calculations is that of the initial file
//  loaded.  This can be any power of 2 in width and height.

#define INITIAL_FILENAME  "start.tga"



////////////////////////////////////////////////////////////////////

#ifndef ASSERT_IF_FAILED
	#define ASSERT_IF_FAILED( hres )	\
	{									\
		if( FAILED(hres) )				\
		   assert( false );				\
	}
#endif


////////////////////////////////////////////////////////////////////

CA_Water::CA_Water()
{
	m_bWireframe			= false;
	m_pInitialStateTexture	= NULL;

	m_pDropletTexture		= NULL;

	m_dwVertexShader		= 0;
	m_dwEqualWeight_PostMultShader = 0;

	m_dwNeighborForceCalc_1 = 0;
	m_dwNeighborForceCalc_2 = 0;
	m_dwApplyForceShader	= 0;
	m_dwApplyVelocityShader = 0;
	m_dwPSH_NormalMapCreate2_Scale = 0;

	m_pVertexBuffer			= NULL;
	mpBackbufferColor		= 0 ;
	mpBackbufferDepth		= 0 ;

	m_bDrawOutput			= true;

	for ( int i = 0; i < kMaxNumTargets; ++i )
	{
        mpTextureFiltered[i] = 0;
		mpFilterTarget   [i] = 0;
	}
}


CA_Water::~CA_Water()
{
	Free();
}

/////////////////////////////////////////////////////////


HRESULT CA_Water::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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



void CA_Water::ReleaseNVTextures( nv_objects::NVTexture *** pTex, int num )
{
	for( int i=0; i < num; i++ )
	{
		if( *pTex[i] != NULL )
		{
			SAFE_DELETE( *pTex[i] );
			*pTex[i] = NULL;
		}
	}
}


void CA_Water::ReleaseNVTextures()
{
	const int NUM = 2;

	nv_objects::NVTexture ** pTex[NUM];

	pTex[0] = &m_pInitialStateTexture;
	pTex[1] = &m_pDropletTexture;


	ReleaseNVTextures( pTex, NUM );
}


HRESULT CA_Water::LoadNVTexture( LPDIRECT3DDEVICE8 pD3DDev, nv_objects::NVTexture * pTex, EBString filename )
{
	HRESULT hr = S_OK;
				   
    hr = pTex->CreateFromFile( pD3DDev, GetFilePath( filename.c_str() ) );
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't load texture: ";
		m_strLastError += filename;
		assert( false );
	}

	return( hr );
}


void CA_Water::LoadRulesAndOtherMaps()
{
	// This one does not re-create the render targets
	//  if new maps of different resolution are loaded
	//  while running.  Only Initialize() creates
	//  the render targets.


	ReleaseNVTextures();


	////////////////////////////////////////////////
	m_pInitialStateTexture = new nv_objects::NVTexture();
	assert( m_pInitialStateTexture != NULL );

	LoadNVTexture( m_pD3DDev, m_pInitialStateTexture, INITIAL_FILENAME );

	////////////////////////////////////////////////
	m_pDropletTexture = new nv_objects::NVTexture();
	assert( m_pInitialStateTexture != NULL );

	LoadNVTexture( m_pD3DDev, m_pDropletTexture, "Droplet1.bmp" );

}



std::string CA_Water::GetFilePath( const std::string& strFileName )
{
	return(	effect_api::GetFilePath( strFileName ) );
}


HRESULT CA_Water::LoadAndCreateShader(const std::string& strFilePath,
										const DWORD* pDeclaration,
		                                DWORD Usage,
										EBSHADERTYPE ShaderType,
										DWORD* pHandle)
{
	return( effect_api::LoadAndCreateShader( m_pD3DDev, strFilePath, pDeclaration, Usage, ShaderType, pHandle ));
}


HRESULT CA_Water::Initialize( LPDIRECT3DDEVICE8 pDev )
{
	// Called when effect is selected from list in browser
	// Free() is called when effect is de-selected

    int     i;
	HRESULT hr;


	// get the device
	assert( pDev != NULL );

	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();			// released on Free()



	m_Droplets.clear();


	// See class header for what each variable represents!
		
	m_bReset = true;
	m_bAnimate = true;
	m_bSingleStep = false;

	m_bSlow = true;

	m_bWrap		= true;

	m_bCreateNormalMap = true;

	m_bDrawOutput = true;

	m_bMulticolor = false;

	m_bUseNormalMapMethod2 = true;
	m_bApplyInteriorBoundaries = true;


	m_eRenderMode = FULLSCREEN_FINALOUT;

	m_nSkipInterval = 0;
	m_nFlipState = 0;


	m_fNrmlSTScale = 0.8f;

	m_dwSleepDelay = 20;

	m_fEqRestore_factor = 0.5f;


	/////////////////////

	m_fBlend = 0.25f;
	m_fVelFactor = 0.50f;

	m_fBlurDist = 0.5f;
	m_fEqRestore_factor = 0.5f;
	m_fDropletFreq = 0.175f;


	/////////////////////////////////

	m_dwCurrentVertexShader = NULL;

	m_pCurrentRulesTex = NULL;

	/////////////////////////////////


	// create the vertex and pixel shaders
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwVertexShader = 0;
	hr = LoadAndCreateShader(GetFilePath("TexCoord_4_Offset.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwVertexShader);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	
    hr = LoadAndCreateShader(GetFilePath("EqualWeightCombine_PostMult.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwEqualWeight_PostMultShader));
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	// Load pixel shaders for neighbor force calc
	// 1st pass = 3 samples & center
    hr = LoadAndCreateShader(GetFilePath("NeighborForceCalc.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwNeighborForceCalc_1 ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	// 2nd pass of force calc = 1 sample & center
    hr = LoadAndCreateShader(GetFilePath("NeighborForceCalc2.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwNeighborForceCalc_2 ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	// pixel shader for applying force to height field
    hr = LoadAndCreateShader(GetFilePath("ApplyForceShader.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwApplyForceShader ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	// pixel shader for applying force to height field
    hr = LoadAndCreateShader(GetFilePath("ApplyVelocityShader.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwApplyVelocityShader ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	// pixel shader for creating normal maps with scalable red & green 
	//  components for the s and t axis
    hr = LoadAndCreateShader(GetFilePath("NormalMapCreate2_Scale.pso"), NULL, 0, SHADERTYPE_PIXEL,
								&(m_dwPSH_NormalMapCreate2_Scale ) );
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	///////////////////////////////////////////////////////////////////////////////////////


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


	SetInitialRenderStates();

    return hr;
}



HRESULT CA_Water::Free()
{
	SAFE_RELEASE( m_pVertexBuffer );			// sets pointers to null after delete


	ReleaseNVTextures();
	m_Droplets.clear();


	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwVertexShader);
		m_pD3DDev->DeletePixelShader(m_dwEqualWeight_PostMultShader);

		m_pD3DDev->DeletePixelShader(m_dwNeighborForceCalc_1);
		m_pD3DDev->DeletePixelShader(m_dwNeighborForceCalc_2);
		m_pD3DDev->DeletePixelShader(m_dwApplyForceShader);
		m_pD3DDev->DeletePixelShader(m_dwApplyVelocityShader);	
		m_pD3DDev->DeletePixelShader(m_dwPSH_NormalMapCreate2_Scale);


		m_dwVertexShader = 0;
		m_dwEqualWeight_PostMultShader = 0;
		m_dwNeighborForceCalc_1 = 0;
		m_dwNeighborForceCalc_2 = 0;
		m_dwApplyForceShader = 0;
		m_dwApplyVelocityShader = 0;
		m_dwPSH_NormalMapCreate2_Scale = 0;
	

        for (int i = 0; i < kMaxNumTargets; ++i)
        {
            SAFE_RELEASE(mpFilterTarget[i]);
            SAFE_RELEASE(mpTextureFiltered[i]);
        }

        SAFE_RELEASE(mpBackbufferColor);
		SAFE_RELEASE(mpBackbufferDepth);

        SAFE_RELEASE(m_pD3DDev);		// we AddRef()'d in Initialize
	}
	
	m_pD3DDev = NULL;

	return S_OK;
}


HRESULT CA_Water::Start()
{
	return S_OK;
}




HRESULT CA_Water::CreateTextureRenderTarget()
{
    HRESULT         hr;

	_ASSERT( mpBackbufferColor != NULL );
	_ASSERT( mpBackbufferDepth != NULL );

    // get the description for the texture we want to filter
	D3DSURFACE_DESC ddsd;
    m_pInitialStateTexture->GetTexture()->GetLevelDesc(0, &ddsd);

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



bool CA_Water::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	// Return true if keyboard key handled.

	char charcode;


	float eqinc = 0.05f;
	float blurinc = 0.01f;
	float normalinc = 0.01f;

	if( bDown )
	{
		// Use only capital letters here - these are VK_ codes!

		switch( dwKey )
		{
		case VK_RIGHT:
			if( m_dwSleepDelay < 500 )
				m_dwSleepDelay += 2;
			m_bSlow = true;
			FDebug("m_dwSleepDelay: %d\n", m_dwSleepDelay );
			return(true);
			break;

		case VK_LEFT:
			if( m_dwSleepDelay > 1 )
				m_dwSleepDelay -= 2;
			m_bSlow = true;
			FDebug("m_dwSleepDelay: %d\n", m_dwSleepDelay );
			return(true);
			break;

		case VK_UP:
			m_fDropletFreq += 0.05f;
			if( m_fDropletFreq > 1.0f )
				m_fDropletFreq = 1.0f;
			FDebug("Droplet Frequency: %f\n", m_fDropletFreq );
			return(true);
			break;
		case VK_DOWN:
			m_fDropletFreq -= 0.05f;
			if( m_fDropletFreq < 0.0f )
				m_fDropletFreq = 0.0f;
			FDebug("Droplet Frequency: %f\n", m_fDropletFreq );
			return(true);
			break;

		case VK_RETURN:
			// single step
			m_bAnimate = false;
			m_bSingleStep = !m_bSingleStep;
			return(true);
			break;

		case VK_SPACE:
			// start or stop the animation
			m_bAnimate = !m_bAnimate;
			return(true);
			break;

		case 'I':
		case 'L':
			m_bApplyInteriorBoundaries = !m_bApplyInteriorBoundaries;
			FDebug("Apply interior boundaries: %s\n", m_bApplyInteriorBoundaries ? "TRUE" : "FALSE");
			return(true);
			break;

		case 'M':
			break;


		case 'Z':
			if( m_fEqRestore_factor >= eqinc )
				m_fEqRestore_factor -= eqinc;
			else
				m_fEqRestore_factor = 0.0f;
			FDebug("m_fEqRestore_factor: %f\n", m_fEqRestore_factor );
			return(true);
			break;
		case 'X':
			if( m_fEqRestore_factor <= 1.0f - eqinc )
				m_fEqRestore_factor += eqinc;
			else
				m_fEqRestore_factor = 1.0f;
			FDebug("m_fEqRestore_factor: %f\n", m_fEqRestore_factor );
			return(true);
			break;


		case 'C':
			if( m_fBlurDist >= blurinc )
				m_fBlurDist -= blurinc;
			FDebug("m_fBlurDist: %f\n", m_fBlurDist );
			UpdateBlurVertOffset();
			return(true);
			break;

		case 'V':
			if( m_fBlurDist <= 1.0f - blurinc )
				m_fBlurDist += blurinc;
			FDebug("m_fBlurDist: %f\n", m_fBlurDist );
			UpdateBlurVertOffset();
			return(true);
			break;


		case 'B':
			m_bWrap = !m_bWrap;
			FDebug("Wrap mode:  %d\n", m_bWrap );
			return(true);
			break;

		case 'Y':
			if( m_fNrmlSTScale >= normalinc )
				m_fNrmlSTScale -= normalinc;
			FDebug("Normal ST scale for red & green: %f\n", m_fNrmlSTScale );
			return(true);
			break;

		case 'U':
			if( m_fNrmlSTScale <= 1.0f - normalinc )
				m_fNrmlSTScale += normalinc;
			FDebug("Normal ST scale for red & green: %f\n", m_fNrmlSTScale );
			return(true);
			break;

		case '1' :
			m_eRenderMode = FULLSCREEN_NEIGHBOR_CALC;
			FDebug("displaying neighbor calc = force texture\n");
			return(true);
			break;

		case '2':
			m_eRenderMode = FULLSCREEN_RESULT;
			FDebug("displaying result = velocity\n");
			return(true);
			break;

		case '3' :
			m_eRenderMode = ALL_TOGETHER;
			FDebug("displaying all surfs\n");
			return(true);
			break;
		case '4':
			m_eRenderMode = FULLSCREEN_FINALOUT;
			FDebug("displaying finalout = height field\n");
			return(true);
			break;


		case 'R':
		case '`':
		case '~':
			FDebug("Loading new droplet and initial maps\n");
			LoadRulesAndOtherMaps();
			return(true);
			break;

		case 'S':
			m_bSlow = !m_bSlow;
			return(true);
			break;
		case 'W':
			m_bWireframe = !m_bWireframe;
			return(false);
			break;


		default:

			charcode = MapVirtualKey( dwKey, 2 ); // VK to char code

			switch( charcode )
			{
			case '<':
			case ',':
				m_fVelFactor *= 0.98f;
				FDebug("velocity factor (mass):  %f\n", m_fVelFactor );
				return(true);
				break;

			case '>':
			case '.':
				m_fVelFactor *= 1.02f;
				FDebug("velocity factor (mass):  %f\n", m_fVelFactor );
				return(true);
				break;

			case '[':
			case '{':
				m_fBlend *= 0.98f;
				FDebug("blend:  %f\n", m_fBlend );
				return(true);
				break;

			case ']':
			case '}':
				m_fBlend *= 1.02f;
				FDebug("blend:  %f\n", m_fBlend );
				return(true);
				break;

			};
		};
	}

	return( false );
}





void CA_Water::SetInitialRenderStates()
{
	int i;

    // set up render state: disable all except texture stage 0 (see below)
    for(i = 0; i < 4; i++ )
    {
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
	m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE,		FALSE );


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

}



HRESULT CA_Water::Tick()
{	
	// Render a new frame

	HRESULT hr;
	int i;

	SetInitialRenderStates();



	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;

    // write to constant memory which uv-offsets to use to accumulate neighbor information
    D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	// Disable culling
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // set render state 
	hr = m_pD3DDev->SetVertexShader( m_dwVertexShader );
	ASSERT_IF_FAILED(hr);

	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));
	ASSERT_IF_FAILED(hr);


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
	D3DXMatrixMultiply(&m_mWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&m_mWorldViewProj, &m_mWorldViewProj);

	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);





	if( m_bReset == true )
	{
		m_bReset = false;
		m_nFlipState = 0;

		FDebug("Reset water sim to initial conditions\n");

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

	if( m_bSlow )
	{
		// slow down the rendering and give any other apps a chance
		Sleep( m_dwSleepDelay );
	}



    IDirect3DTexture8       * pSrcTex;

	static int nSkip = 0;

	if( nSkip >= m_nSkipInterval && m_bDrawOutput == true )
	{
		nSkip = 0;

		// Switch back to normal rendering to display the results of 
		//   the rendering to texture

		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


		hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
		ASSERT_IF_FAILED( hr );

		// No need to clear - We overdraw the whole backbuffer
		// No z test

		hr = m_pD3DDev->SetRenderState( D3DRS_ZENABLE, false ); 
		ASSERT_IF_FAILED(hr);
		m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,	false );

		// turn off pixel shading
		m_pD3DDev->SetPixelShader( 0 );


		//////////////////////////////////////////////////////////		
		// Render the result of the calculations to the device
		// These steps are not needed to run the simulation - for final
		//  display of the results only

		if( m_bWireframe )
		{
			hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0,0,0 ), 1.0, 0);
			ASSERT_IF_FAILED(hr);

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

			D3DXMatrixMultiply(&m_mWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&m_mWorldViewProj, &m_mWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


			hr = m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTex_HeightTarg] );
			ASSERT_IF_FAILED(hr);

			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
			ASSERT_IF_FAILED(hr);

			break;

		case FULLSCREEN_RESULT :
			// Draw quad over full display
			D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);

			D3DXMatrixMultiply(&m_mWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&m_mWorldViewProj, &m_mWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTex_VelTarg] );

			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			break;
		case FULLSCREEN_NEIGHBOR_CALC:

			// Draw quad over full display
			D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);

			D3DXMatrixMultiply(&m_mWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&m_mWorldViewProj, &m_mWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

			m_pD3DDev->SetTexture(0, mpTextureFiltered[3] );

			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			break;
		case ALL_TOGETHER :
		   // draw quad in upper left corner: original texture
			D3DXMatrixTranslation(&matWorld, -1.0f, 1.0f, 0.0f);

			D3DXMatrixMultiply(&m_mWorldViewProj, &matWorld, &matViewProj);
			D3DXMatrixTranspose(&m_mWorldViewProj, &m_mWorldViewProj);
			m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);

			// reset offsets to 0 (ie no offsets)
			offset.x = 0.0f;
			m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


			m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTex_VelTarg] );	// upper left corner


			if( m_SimulationMode == SM_RGB_GLOW ||
				m_SimulationMode == SM_HUE_DROPLETS )
			{
				m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTex_HeightTarg] );
			}


			hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);


			// draw quads in the other corners, use generated textures
			for (int j = 0; j < 3; ++j)
			{
				D3DXMatrixTranslation(&matWorld, (j == 2) ? -1.0f :  1.0f, 
												 (j == 0) ?  1.0f : -1.0,
												 0.0f);
				D3DXMatrixMultiply(&m_mWorldViewProj, &matWorld, &matViewProj);
				D3DXMatrixTranspose(&m_mWorldViewProj, &m_mWorldViewProj);
				m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);


				// case for upper right display
				pSrcTex = mpTextureFiltered[m_nTex_HeightTarg];		// upper right corner;


				if( j==1 )
				{
					pSrcTex = mpTextureFiltered[3];

				}
				else if( j==2 )
				{
					pSrcTex = mpTextureFiltered[m_nTex_VelTarg];
				}



				// if one of these modes, want to display same texture in all 4
				//  panes, to show texture wrapping.
				if( m_SimulationMode == SM_RGB_GLOW ||
					m_SimulationMode == SM_HUE_DROPLETS )
				{
					pSrcTex = mpTextureFiltered[m_nTex_HeightTarg];
				}


				if( pSrcTex != 0 )
				{
					m_pD3DDev->SetTexture(0, pSrcTex );
					hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
				}
				else
				{
					FDebug("bad texture pointer in result display!!\n");
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


	// Reset bilinear filtering, as that is most common
	for( i=0; i < 4; i++ )
	{
		m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE  );
	}


	return( hr );
}





void	CA_Water::SetNormalMapSTScale( float STScale )
{
	m_fNrmlSTScale = STScale;
}


void	CA_Water::SetBlurDistance( float fac )
{
	m_fBlurDist  = fac;
	UpdateBlurVertOffset();
}

void	CA_Water::SetEqRestoreFac( float fac )
{
	m_fEqRestore_factor = fac;
}

void	CA_Water::SetVelocityApplyFac( float fac )
{
	m_fVelFactor = fac;
}

void	CA_Water::SetBlendFac( float fac )
{
	m_fBlend = fac;
}




void CA_Water::UpdateBlurVertOffset()
{
	float woff, hoff;

	woff = m_fPerTexelWidth  / 2.0f;
	hoff = m_fPerTexelHeight / 2.0f;

    float      type3OffsetX[4] = {	- m_fPerTexelWidth/2.0f	+ woff,
									  m_fPerTexelWidth		+ woff,
									  m_fPerTexelWidth/2.0f	+ woff,
									- m_fPerTexelWidth		+ woff	};
	
    float      type3OffsetY[4] = {	- m_fPerTexelHeight			+ hoff,
									- m_fPerTexelHeight/2.0f	+ hoff,
									  m_fPerTexelHeight			+ hoff,
									  m_fPerTexelHeight/2.0f	+ hoff	};

	 

	float interp_x, interp_y;

    for (int i = 0; i < 4; ++i)
    {
		interp_x = m_fBlurDist * ( type3OffsetX[i] - woff ) + woff;
		interp_y = m_fBlurDist * ( type3OffsetY[i] - hoff ) + hoff;

        D3DXVECTOR4  vec_type3Offset( interp_x, interp_y, 0.0f, 0.0f);

    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE3     + 5*i, &vec_type3Offset, 1);
    }
}



void	CA_Water::DrawInteriorBoundaryObjects()
{
	bool bInlineDroplet = true;


	HRESULT hr;
	assert( m_pD3DDev != NULL );
	int i;

	IDirect3DSurface8       *pBack_Color = NULL; 
	IDirect3DSurface8       *pBack_Depth = NULL; 

	/////////////////////////////

	const int numobjects = 1;
	int ind = 0;

	float xmin[numobjects];
	float xmax[numobjects];
	float ymin[numobjects];
	float ymax[numobjects];

	DWORD color = 0x00000000;		// ARGB

	xmin[ind] = 0.0f;
	xmax[ind] = 1.0f;
	ymin[ind] = 0.0f;
	ymax[ind] = 1.0f;
	ind++;


	// Could also draw a few fixed objects with
	//  preset positions, etc..
	// Select a different texture or constant color
	//  as the source for these objects
	/*
	const int numobjects = 2;
	int ind = 0;

	float xmin[numobjects];
	float xmax[numobjects];
	float ymin[numobjects];
	float ymax[numobjects];

	DWORD color = 0x00000000;		// ARGB

	xmin[ind] = 0.485f;
	xmax[ind] = 0.50f;
	ymin[ind] = 0.15f;
	ymax[ind] = 0.40f;
	ind++; assert( ind < numobjects );

	xmin[ind] = 0.485f;
	xmax[ind] = 0.50f;
	ymin[ind] = 0.45f;
	ymax[ind] = 0.55f;
	ind++;
	*/

	float x_center[ numobjects ];
	float y_center[ numobjects ];
	float x_size[ numobjects ];
	float y_size[ numobjects ];

	for( i=0; i < numobjects; i++ )
	{
		x_center[i] = (xmin[i] + xmax[i])/2.0f;
		y_center[i] = (ymin[i] + ymax[i])/2.0f;
	
		x_size[i] = (xmax[i] - xmin[i]);
		y_size[i] = (ymax[i] - ymin[i]);
	}


	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	// Draw into the velocity texture:
	if( bInlineDroplet )
	{
		// Rendering is happening in middle of physics calc.
		// Render to appropriate texture - this doesn't require 
		//   any special work

	}
	else
	{
		// rendering happening outside physics calc
		// Render to appropriate velocity texture for next frame,
		//  also save and restore current render targets

		m_pD3DDev->GetRenderTarget(        &pBack_Color );
		m_pD3DDev->GetDepthStencilSurface( &pBack_Depth );


		SetInitialRenderStates();

		hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));		
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTex_VelTarg], NULL);
		ASSERT_IF_FAILED(hr);
	}


	hr = m_pD3DDev->SetVertexShader( m_dwVertexShader );
	ASSERT_IF_FAILED(hr);


	///////////////////////////////////////
	// Set render mode to use source texture
	// Alpha determines fixed vs. free areas

	hr = m_pD3DDev->SetPixelShader( 0 );
	ASSERT_IF_FAILED(hr);

	m_pD3DDev->SetTexture(0, m_pInitialStateTexture->GetTexture() );

	//  Additive blend
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA		);
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);

	// m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, color );	// ARGB

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR	);	// blue only

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1	);
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );



	float x,y;

	for( i=0; i < numobjects; i++ )
	{
		// shift x,y into -1 to 1 range
		x = (x_center[i] - 0.5f)*2.0f;
		y = (y_center[i] - 0.5f)*2.0f;

		// alter matrix to place the droplet
		D3DXMATRIX matWorld;
		D3DXMATRIX matView;
		D3DXMATRIX matProj;
		D3DXMATRIX matViewProj;
		D3DXMATRIX matWorldViewProj;

		D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
		D3DXVECTOR3 vLookatPt = D3DXVECTOR3( x,    y,     0.0f );
		D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );

		// Set World, View, Projection, and combination matrices.
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

		// 2.0f cause translation extends from -1 to 1
		D3DXMatrixOrthoLH(&matProj, 2.0f, 2.0f, 0.2f, 20.0f);	// x,y, zmin zmax

		D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

		// draw a single quad to texture:
		// the quad covers the whole "screen" exactly

		D3DXMatrixScaling(&matWorld, x_size[i], y_size[i], 1.0f);

		D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
		D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

		m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

		// Draw the square object
		hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		ASSERT_IF_FAILED(hr);

	}

	// reset world view proj matrix
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);


	// Reset to original render target & depth
	if( !bInlineDroplet )
	{
		hr = m_pD3DDev->SetRenderTarget( pBack_Color, pBack_Depth );
		ASSERT_IF_FAILED(hr);
	}

	SAFE_RELEASE(pBack_Color);
	SAFE_RELEASE(pBack_Depth);
}


void	CA_Water::AddDroplet( float x, float y, float scale )
{
	// Adds a droplet to the rendering queue.
	// These are then rendered when DrawDroplets() is called
	// Coords are from 0 to 1.0 across texture surface
	// Size of droplet is determined at draw time - Could add 
	//   another variable to override this
	
	D3DXVECTOR3	pos( x,y,scale );

	m_Droplets.push_back( pos );

}


void	CA_Water::DrawDroplets()
{
	//////////////////////////////////////////////////
	// input x,y in range 0.0 to 1.0 to cover whole target

	HRESULT hr;
	int		i;
	float	x,y;
	float	scale;

	assert( m_pD3DDev != NULL );


	/////////////////////////////
	// Draw into the velocity texture:
	// Rendering is happening in middle of physics calc.
	// Render to appropriate velocity texture

	IDirect3DSurface8       *pBack_Color = NULL; 
	IDirect3DSurface8       *pBack_Depth = NULL; 
	m_pD3DDev->GetRenderTarget(        &pBack_Color );
	m_pD3DDev->GetDepthStencilSurface( &pBack_Depth );

	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTex_VelTarg], NULL);
	ASSERT_IF_FAILED(hr);


	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;


	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );

	// 2.0f cause translation extends from -1 to 1
	D3DXMatrixOrthoLH(&matProj, 2.0f, 2.0f, 0.2f, 20.0f);	// x,y, zmin zmax



	///////////////////////////////////////
	// Set render mode - additive
	hr = m_pD3DDev->SetPixelShader( 0 );
	ASSERT_IF_FAILED(hr);

	hr = m_pD3DDev->SetVertexShader( m_dwVertexShader );
	ASSERT_IF_FAILED(hr);

	m_pD3DDev->SetTexture(0, m_pDropletTexture->GetTexture() );

	//  Additive blend
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );


	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR	);	// blue only

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );


	for( i=0; i < m_Droplets.size(); i++)
	{
		// shift [0,1.0] coords into [-1,1] range

		x =		( m_Droplets[i].x - 0.5f ) * 2.0f;
		y =		( m_Droplets[i].y - 0.5f ) * 2.0f;
		scale =   m_Droplets[i].z;

		// alter matrix to place the droplet appropriately
		D3DXVECTOR3 vLookatPt = D3DXVECTOR3( x,    y,     0.0f );

		// Set World, View, Projection, and combination matrices.
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

		D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

		D3DXMatrixScaling(&matWorld, 1.0f/scale, 1.0f/scale, 1.0f);

		D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
		D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

		m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

		// Draw a single quad to the texture render target
		// The quad is textured with the initial droplet texture, and
		//   covers some small portion of the render target
		// Draw the droplet

		hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		ASSERT_IF_FAILED(hr);

	}

	// Clear the array of droplets that we've just drawn
	m_Droplets.clear();

	// reset world view proj matrix
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &m_mWorldViewProj(0, 0), 4);

	// restore orig render targets
	hr = m_pD3DDev->SetRenderTarget( pBack_Color, pBack_Depth );
	ASSERT_IF_FAILED(hr);

	SAFE_RELEASE(pBack_Color);
	SAFE_RELEASE(pBack_Depth);
}



void	CA_Water::Do_CreateNormalMapSteps()
{
	HRESULT hr = S_OK;

	D3DXVECTOR4 offset( 0.0f, 0.0f, 0.0f, 0.0f);


	hr = m_pD3DDev->SetTexture( 0,	mpTextureFiltered[m_nTex_HeightTarg] );
	hr = m_pD3DDev->SetTexture( 1,	mpTextureFiltered[m_nTex_HeightTarg] );
	hr = m_pD3DDev->SetTexture( 2,	mpTextureFiltered[m_nTex_HeightTarg] );
	hr = m_pD3DDev->SetTexture( 3,	mpTextureFiltered[m_nTex_HeightTarg] );
	ASSERT_IF_FAILED(hr);


	m_pD3DDev->SetRenderTarget( mpFilterTarget[ m_nTex_NrmlTarg ], NULL );
	ASSERT_IF_FAILED( hr );


	// if( m_bUseNormalMapMethod2 )
	{
		// Set constants for red & green scale factors (also essential color masks)
		// Red mask first
		float pix_masks[4] = { m_fNrmlSTScale, 0.0f, 0.0f, 0.0f };

		hr = m_pD3DDev->SetPixelShaderConstant( PCN_RED_MASK, &pix_masks, 1 );
		ASSERT_IF_FAILED(hr);

		// Now green mask & scale:
		pix_masks[0] = 0.0f;
		pix_masks[1] = m_fNrmlSTScale;

		hr = m_pD3DDev->SetPixelShaderConstant( PCN_GREEN_MASK, &pix_masks, 1 );
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->SetPixelShader( m_dwPSH_NormalMapCreate2_Scale );
		ASSERT_IF_FAILED(hr);
	}



	// use nearest neighbor offsets
	offset.x = 4.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);


	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);

}



HRESULT CA_Water::DoSingleTimeStep()
{

	HRESULT hr;
	int i;

	int tmp;


	// Swap texture source & target indices & pointers
	//  0 = start from initial loaded texture
	//  1/2 = flip flop back and forth between targets & sources

	switch( m_nFlipState )
	{
	case 0:

		m_pTex_HeightSrc	= m_pInitialStateTexture->GetTexture();

		m_nTex_HeightTarg = 0;
		m_nTex_HeightSrc =  4;

		m_nTex_VelSrc	= 1;
		m_nTex_VelTarg	= 2;

		m_nForceStepOne		= 3;
		m_nForceTexture		= 5;

		// use old ht src as temp place to render normals
		m_nTex_NrmlAccum	= m_nTex_HeightSrc;
		m_nTex_NrmlTarg		= 3;				// use force tex as place to keep em


		// Clear initial velocity texture to 0x80 == gray
		hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTex_VelSrc], NULL);
		ASSERT_IF_FAILED( hr );
		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0x80, 0x80, 0x80 ), 1.0, 0);


		break;
	case 1:
		// Both to 2 as we never read & write height simultaneously so this is ok
		tmp = m_nTex_HeightTarg;
		m_nTex_HeightTarg = m_nTex_HeightSrc;
		m_nTex_HeightSrc = tmp;

		m_pTex_HeightSrc	= mpTextureFiltered[m_nTex_HeightSrc];


		m_nTex_VelSrc	= 2;
		m_nTex_VelTarg	= 1;

		m_nForceStepOne		= 3;
		m_nForceTexture		= 5;

		// use old ht src as temp place to render normals
		m_nTex_NrmlAccum	= m_nTex_HeightSrc;
		m_nTex_NrmlTarg		= 3;				// use force tex as place to keep em


		break;
	case 2:

		tmp = m_nTex_HeightTarg;
		m_nTex_HeightTarg = m_nTex_HeightSrc;
		m_nTex_HeightSrc = tmp;

		m_pTex_HeightSrc	= mpTextureFiltered[m_nTex_HeightSrc];

		m_nTex_VelSrc	= 1;
		m_nTex_VelTarg	= 2;

		m_nForceStepOne		= 3;
		m_nForceTexture		= 5;

		// use old ht src as temp place to render normals
		m_nTex_NrmlAccum	= m_nTex_HeightSrc;
		m_nTex_NrmlTarg		= 3;				// use force tex as place to keep em
		
		break;
	}










    // variable for writing to constant memory which uv-offsets to use
    D3DXVECTOR4     offset(0.0f, 1.0f, 0.0f, 0.0f);

	m_pD3DDev->SetVertexShaderConstant( CV_ONOFF_1, &offset,    1);


	// Set force restoring values to equilibrium (0.5 = gray)
	float eq_rest[4] = { m_fEqRestore_factor,m_fEqRestore_factor,m_fEqRestore_factor, 1.0f };

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_EQ_REST_FAC, &eq_rest,    1);
	ASSERT_IF_FAILED(hr);


	//////////////////////////////////////////////////////////
	// Set constant for offseting values to 0.5 for gray = 0



	m_fhalfoff = 0.500f;

	float pix_offset[4] = { m_fhalfoff,m_fhalfoff,m_fhalfoff, 1.0f };


	hr = m_pD3DDev->SetPixelShaderConstant( PCN_ADDHALF, &(pix_offset[0]) , 1);
	ASSERT_IF_FAILED(hr);


	// no need to clear temporary target


	// even if wireframe mode, render to texture as solid
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE,			D3DFILL_SOLID );



	DWORD wrapval = m_bWrap ?  D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV,  wrapval);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU,  wrapval);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV,  wrapval);


    for(i = 0; i < 4; i++ )
    {
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    }


	/////////////////////////////////////////////////////////////
	//  Render first 3 components of force from three neighbors
	//  Offsets selected are 1 center texel for center height
	//    and 3 of the 4 nearest neighbors.  Texture selected
	//    is same for all stages as we're turning height difference
	//    of nearest neightbor texels into a force value.


	hr = m_pD3DDev->SetPixelShader( m_dwNeighborForceCalc_1 );
	ASSERT_IF_FAILED( hr );
	
	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nForceStepOne], NULL);
	ASSERT_IF_FAILED( hr );


	// write first part of force calc
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


	hr = m_pD3DDev->SetTexture(0, m_pTex_HeightSrc );
	ASSERT_IF_FAILED(hr);
	hr = m_pD3DDev->SetTexture(1, m_pTex_HeightSrc );
	ASSERT_IF_FAILED(hr);
	hr = m_pD3DDev->SetTexture(2, m_pTex_HeightSrc );
	ASSERT_IF_FAILED(hr);
	hr = m_pD3DDev->SetTexture(3, m_pTex_HeightSrc );
	ASSERT_IF_FAILED(hr);


	// Render using offset set 1
	offset.x = 1.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	// draw the quad with texture coords doing difference of 
	//  nearby texels for force calc.
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);



	////////////////////////////////////////////////////////////////
	// Now add in last component of force for the 4th neighbor
	//  that we didn't have enough texture lookups to do in the 
	//  first pass

	hr = m_pD3DDev->SetPixelShader( m_dwNeighborForceCalc_2 );
	ASSERT_IF_FAILED( hr );

	// Cannot use additive blending as the force contribution might
	//   be negative and would have to subtract from the dest.
	// We must instead use an additional texture as target and read
	//   the previous partial 3-neighbor result into the pixel shader
	//   for possible subtraction

	// Alphablend must be false

	//; t0 = center
	//; t1 = 2nd axis final point
	//; t2 = previous partial result texture sampled at center
	//; t3 = not used

	m_pD3DDev->SetTexture(0, m_pTex_HeightSrc );
	m_pD3DDev->SetTexture(1, m_pTex_HeightSrc );
	m_pD3DDev->SetTexture(2, mpTextureFiltered[m_nForceStepOne] );


	// Render to the final force texture 
	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nForceTexture], NULL);
	ASSERT_IF_FAILED( hr );



	offset.x = 2.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);




	/////////////////////////////////////////////////////////////////
	// Apply the force with a scale factor to reduce it's magnitude.
	// Add this to the current texture representing the water height.

	hr = m_pD3DDev->SetPixelShader( m_dwApplyForceShader );
	ASSERT_IF_FAILED( hr );


	// use offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	
	m_pD3DDev->SetTexture(0, mpTextureFiltered[m_nTex_VelSrc]	);// 1st is offset to 0 with offsets 1.0
	m_pD3DDev->SetTexture(1, mpTextureFiltered[m_nForceTexture]	);				// stage 2,3 not used


	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTex_VelTarg], NULL);
	ASSERT_IF_FAILED( hr );


	// no more additive blending - math for signed values has to be
	//   done in the pixel shader	
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	// Set constant at which to apply force to velocity
	float pix_force_mult[4] = { m_fBlend, m_fBlend, m_fBlend, 1.0f };	// RGBA

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_1, &pix_force_mult, 1 );
	ASSERT_IF_FAILED(hr);



	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);





	///////////////////////////////////////////////////////////////////
	// With velocity texture selected, render new excitation droplets
	//   at random freq.


	float rnd_freq;
	rnd_freq = (float)rand()/((float)RAND_MAX);

	if( m_fDropletFreq > rnd_freq )
	{
		// a drop falls - decide where

		float x = (float)rand()/((float)RAND_MAX);
		float y = (float)rand()/((float)RAND_MAX);

		// pick a droplet size at random:
		float scale = 10.0f;
		scale += 25.0f * ((float)rand()/((float)RAND_MAX));

		AddDroplet( x, y, scale );
	}

	//  Now draw the droplets:
	DrawDroplets();


	//////////////////////////////////////////////////////////////////////
	// Apply velocity to position


	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTex_HeightTarg], NULL);
	ASSERT_IF_FAILED( hr );


	hr = m_pD3DDev->SetPixelShader( m_dwApplyVelocityShader );
	ASSERT_IF_FAILED( hr );
	
	m_pD3DDev->SetTexture(0, m_pTex_HeightSrc );
	m_pD3DDev->SetTexture(1, mpTextureFiltered[m_nTex_VelTarg] );
	m_pD3DDev->SetTexture(2, 0);
	m_pD3DDev->SetTexture(3, 0);


	// no more additive blending - math for signed values has to be
	//   done in the pixel shader	
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	// Set constant at which to apply velocity to height
	float pix_vel_mult[4] = { m_fVelFactor, m_fVelFactor, m_fVelFactor, 1.0f };	// RGBA

	hr = m_pD3DDev->SetPixelShaderConstant( PCN_MULTFACTOR_1, &pix_vel_mult, 1 );
	ASSERT_IF_FAILED(hr);


	// use offsets of zero
	offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);


	///////////////////////////////////////////////////////////////////
	//  blur positions to smooth noise & generaly dampen things
	//  degree of blur is controlled by magnitude of 4 neighbor texel
	//   offsets with bilinear on


	// Swap height targ & src

	tmp = m_nTex_HeightTarg;
	m_nTex_HeightTarg = m_nTex_HeightSrc;
	m_nTex_HeightSrc = tmp;

	m_pTex_HeightSrc	= mpTextureFiltered[m_nTex_HeightSrc];

	// Now blur height

	m_pD3DDev->SetTexture(0, m_pTex_HeightSrc );
	m_pD3DDev->SetTexture(1, m_pTex_HeightSrc );
	m_pD3DDev->SetTexture(2, m_pTex_HeightSrc );
	m_pD3DDev->SetTexture(3, m_pTex_HeightSrc );


	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	hr = m_pD3DDev->SetRenderTarget( mpFilterTarget[m_nTex_HeightTarg], NULL);
	ASSERT_IF_FAILED( hr );

	hr = m_pD3DDev->SetPixelShader( m_dwEqualWeight_PostMultShader );
	ASSERT_IF_FAILED( hr );

	
	offset.x = 3.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED(hr);


	////////////////////////////
	// Draw fixed objects within the texture

	if( m_bApplyInteriorBoundaries )
	{
		DrawInteriorBoundaryObjects();
	}

	///////////////////////////////////////////////////////////////////
	// If selected, create a normal map from the height
	
	if( m_bCreateNormalMap )
	{
		Do_CreateNormalMapSteps();
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


IDirect3DTexture8 *		CA_Water::GetOutputTexture()
{
	int texture_index;

	switch( m_SimulationMode )
	{
	case SM_NORMAL_GRAY:
		texture_index = m_nTex_NrmlTarg;
		break;

	case SM_RGB_GLOW:
		texture_index = m_nTex_HeightTarg;
		break;

	case SM_HUE_DROPLETS:
		texture_index = m_nTex_HeightTarg;
		break;
	}


	assert( mpTextureFiltered[texture_index] != NULL );

	return( mpTextureFiltered[texture_index] );
}



void CA_Water::CreateAndWriteUVOffsets(int width, int height)
{

	// This sets vertex shader constants used to displace the
	//  source texture over several additive samples.  This is 
	//  used to accumulate neighboring texel information that we
	//  need to run the simulation.
	// Label the texels as follows, with the source texel being
	//   rendered as "e"
	//		(0.0)
    //            abc
    //            def
    //            ghi
	//				   (1,1)


    
    m_fPerTexelWidth  = 1.0f/static_cast<float>(width);
    m_fPerTexelHeight = 1.0f/static_cast<float>(height);
	

	// A multiplier for sampling beyond the 8 nearest neighbors
	// For this, we don't use the multiplier, so it is just 1.0

	m_fPerTexelWidth	*= 1.0f;	  // 1 = no change - run standard game of life
    m_fPerTexelHeight   *= 1.0f;


	// Half-texel width and height offsets for sampling from the
	//  exact center of a texel when rendering a full-coverage quad
	//  to a texture target.  Without these, a full-coverage quad with
	//  texture coords from 0 to 1.0 would sample from the lower corner
	//  of each source texel in rendering to a texture target of the 
	//  same resolution as the source texture.

	float woff, hoff;
	woff =  m_fPerTexelWidth/2.0f;
	hoff =  m_fPerTexelHeight/2.0f;


	// Offsets set 0 - all zeros for center of texel sampling.

    // first the easy one: no offsets for sampling the center
	//  'e' texel
	// Use index offset value 0.0 to access these in the 
	//  vertex shader.

    float      noOffsetX[4] = { 0.0f + woff, 0.0f + woff, 0.0f + woff, 0.0f + woff}; 
    float      noOffsetY[4] = { 0.0f + hoff, 0.0f + hoff, 0.0f + hoff, 0.0f + hoff};


	// Offset set 1:  For use with neighbor force pixel shader 1
	//  samples center with 0, +u, -u, and +v,
	//	ie the 'e','f','d', and 'h' texels

    float		type1OffsetX[4] = {	   0.0f + woff,
									-  m_fPerTexelWidth  + woff, 
                                       m_fPerTexelWidth  + woff,
									   0.0f + woff					};

    float		type1OffsetY[4] = {	   0.0f + hoff,
									   0.0f + hoff, 
                                       0.0f + hoff,
									   m_fPerTexelHeight + hoff		};



 

	// Offset set 2:  for use with neighbor force pixel shader 2
	//  samples center with 0, and -v texels 
	//  ie the 'e' and 'b' texels
	// This completes a pattern of sampling center texel and it's
	//   4 nearest neighbors to run the height-based water simulation

	// 3rd must be 0 0 to sample texel center from partial result
	//   texture.

    float		type2OffsetX[4] = {		0.0f + woff,
										0.0f + woff,
										0.0f + woff,
										0.0f + woff					};

    float		type2OffsetY[4] = {		0.0f + hoff,
									-  m_fPerTexelHeight + hoff, 
										0.0f + hoff,
										0.0f + hoff					};


	// Now we change all that & instead of sampling 1 texel from center, we
	//  sample 1.5 texels from center and use bilinear sampling.  This reduces
	//  some of the noise and high-frequency oscillation from sampling the 
	//  exact texel neighbor only, giving smoother results in the height field
	//  and smoother water ripples

	// Displacements 1.5 texels from center texel
	//
	// 0 = 0 0
	// 1 = -s -t
	// 2 = +s -t
	// 3 = +s +t
// /*
	type1OffsetX[0] =	0.0f + woff;
	type1OffsetX[1] = - m_fPerTexelWidth  ;
	type1OffsetX[2] =	m_fPerTexelWidth  + 2.0f * woff;
	type1OffsetX[3] =	m_fPerTexelWidth  + 2.0f * woff;

	type1OffsetY[0] =	0.0f + hoff;
	type1OffsetY[1] = -	m_fPerTexelHeight;
	type1OffsetY[2] = -	m_fPerTexelHeight;
	type1OffsetY[3] =	m_fPerTexelHeight + 2.0f * woff;


	// Displacements 1.5 texels from center texel
	// 0 = 0 0
	// 1 = -s +t
	// 2 = 0 0

	type2OffsetX[0] =	0.0f + woff;
	type2OffsetX[1] = - m_fPerTexelWidth;
	type2OffsetX[2] =	0.0f + woff;
	type2OffsetX[3] =	0.0f + woff;

	type2OffsetY[0] =	0.0f + hoff;
	type2OffsetY[1] =   m_fPerTexelHeight + 2.0f * hoff;
	type2OffsetY[2] =	0.0f + hoff;
	type2OffsetY[3] =	0.0f + hoff;
// */
	

	// Offsets 3 are set in separate function


	//////////////////////////////////////////////////////////////
	// Nearest neighbor offsets:

	float		type4OffsetX[4] = { -  m_fPerTexelWidth	+ woff, 
                                       m_fPerTexelWidth	+ woff,  
                                       0.0f	+ woff,
									   0.0f	+ woff };

    float		type4OffsetY[4] = {    0.0f		+ hoff, 
                                       0.0f		+ hoff,
									   m_fPerTexelHeight		+ hoff, 
                                    -  m_fPerTexelHeight		+ hoff  };

	int i;

    // write all these offsets to constant memory
    for ( i = 0; i < 4; ++i)
    {
        D3DXVECTOR4  noOffset(      noOffsetX[i],    noOffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type1Offset(type1OffsetX[i], type1OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type2Offset(type2OffsetX[i], type2OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type4Offset(type4OffsetX[i], type4OffsetY[i], 0.0f, 0.0f);

    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_NO_OFFSET + 5*i, &noOffset,    1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE1     + 5*i, &type1Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE2     + 5*i, &type2Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE4     + 5*i, &type4Offset, 1);
    }


	// Set offsets set 3.0 used to further filter the high-frequency
	//  oscillations

	UpdateBlurVertOffset();

}



void CA_Water::SetSimMode( int simmode )
{


	m_SimulationMode = simmode;

	if( m_SimulationMode >= SM_LAST )
		m_SimulationMode = 0;


	switch( m_SimulationMode )
	{
	case SM_NORMAL_GRAY:
		m_fEqRestore_factor = 0.5f;
		m_fBlurDist = 0.5f;
		UpdateBlurVertOffset();

		m_eRenderMode = FULLSCREEN_RESULT;
		m_bDrawOutput = false;

		m_fBlend = 0.25f;
		m_fVelFactor = 0.5;
		m_fDropletFreq = 0.1750f;

		m_bCreateNormalMap = true;


		FDebug("Set sim mode to NORMAL_GRAY\n");
		break;

	case SM_RGB_GLOW:
		m_fEqRestore_factor = 0.0f;
		m_fBlurDist = 0.01f;
		UpdateBlurVertOffset();


		m_eRenderMode = FULLSCREEN_FINALOUT;
		m_bDrawOutput = true;

		m_fBlend = 0.25f;
		m_fVelFactor = 0.5;
		m_fDropletFreq = 0.0f;
		
		m_bCreateNormalMap = false;
		m_bApplyInteriorBoundaries = true;


		FDebug("Set sim mode to RGB_GLOW\n");
		break;

	case SM_HUE_DROPLETS:
		m_fEqRestore_factor = 0.0f;
		m_fBlurDist = 0.01f;
		UpdateBlurVertOffset();

		m_fBlend = 0.02f;
		m_fVelFactor = 0.082994f;
		m_fDropletFreq = 0.0f;

		m_eRenderMode = FULLSCREEN_FINALOUT;
		m_bDrawOutput = true;

		m_bCreateNormalMap = false;
		m_bApplyInteriorBoundaries = false;

		FDebug("Set sim mode to HUE_DROPLETS\n");

		m_fBlurDist = 0.020f;
		m_fVelFactor = 0.0637f;
		m_fBlend = 0.01158f;


		break;

	default:
		FDebug("weird sim mode!\n");


	}
}







