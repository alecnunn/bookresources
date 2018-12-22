/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\TwoSided
File:  shader_TwoSided.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include "eb_effect.h"
#include "nvdevice.h"
#include "shader_TwoSided.h" 
#include "TwoSided.h"

#include "NV_Error.h"

#include "SimpleObject8.h"

using namespace std;


#define DTOR (D3DX_PI / 180.0f)



DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderTwoSided();
}

}

CShaderTwoSided::CShaderTwoSided()
:	m_eDisplayOption(DISPLAY_TwoSidedRESULT),
	m_pVertexBuffer(NULL),
	m_pIndexBuffer(NULL),
	m_fAngle(0.0f),
	m_pNVDevice(NULL),
	m_dwTwoSidedPixelShader(0),
	m_dwTwoSidedNoLightShader(0),
	m_dwTwoSidedVShader(0),
	m_pBumpMapTexture(NULL),
	m_pEnvMapTexture(NULL)
{
	m_strEffectName = "Two-Sided Texturing";
	m_strEffectLocation = "Pixel Shaders\\Lighting";
	m_strEffectPixelShader = GetFilePath("TwoSided.nvp");
	m_strEffectVertexShader = GetFilePath("TwoSided.nvv");

	m_pDecalTexture = NULL;
	m_pLightObjTexture = NULL;
	m_pDecalBackTexture = NULL;

	m_bWireframe = false;

	m_pObjLight = NULL;
	m_pObj		= NULL;
}

void CShaderTwoSided::UpdateProperties()
{
	EBEffect::UpdateProperties();

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "TwoSided", GetFilePath("TwoSided.nvv"), EBTYPE_STRING_PROP));

	// Pixel Shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "TwoSided", GetFilePath("TwoSided.nvp"), EBTYPE_STRING_PROP));

}

CShaderTwoSided::~CShaderTwoSided()
{
	Free();	
}




HRESULT CShaderTwoSided::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_DELETE(m_pNVDevice);

	if( m_pObj != NULL )
	{
		delete m_pObj;
		m_pObj= NULL;
	}

	if( m_pObjLight != NULL )
	{
		delete m_pObjLight;
		m_pObjLight= NULL;
	}

	SAFE_RELEASE(m_pBumpMapTexture);
	SAFE_RELEASE(m_pDecalTexture);
	SAFE_RELEASE(m_pEnvMapTexture);
	SAFE_RELEASE(m_pLightObjTexture);
	SAFE_RELEASE(m_pDecalBackTexture);


	if (m_pD3DDev)
	{
		m_pD3DDev->DeletePixelShader(m_dwTwoSidedPixelShader);
		m_dwTwoSidedPixelShader = NULL;

		m_pD3DDev->DeletePixelShader(m_dwTwoSidedNoLightShader);
		m_dwTwoSidedNoLightShader = NULL;

		m_pD3DDev->DeleteVertexShader(m_dwTwoSidedVShader);
		m_dwTwoSidedVShader = NULL;

		SAFE_RELEASE(m_pD3DDev);
	}

	
	return S_OK;
}



HRESULT CShaderTwoSided::Initialize(IDirect3DDevice8* pDev)
{
	FDebug("\n\n");
	FDebug("Begin CShaderTwoSided::Initialize...\n");

	HRESULT hr;

 	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVTwoSidedDevice(pDev, this);

	m_nHDiv = 6;
	m_nVDiv = 35;
	m_fTwist = 0.3f;
	m_fWidth = 4.0f;

	m_pObj = new SimpleObject8;
	assert( m_pObj != NULL );

	m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
	m_pObj->GenerateNormals();


	// Make light stand-in object:
	m_pObjLight = new SimpleObject8;
	assert( m_pObj != NULL );
	m_pObjLight->InitSphere( 0.3f, 8, 8 );


	////////////////////////////////////////////////////////
	// Load pixel shaders for objects

	hr = LoadAndCreateShader(GetFilePath("TwoSided.pso"), NULL, 0,
							 SHADERTYPE_PIXEL, &m_dwTwoSidedPixelShader);
	if (FAILED(hr))
	{
		FDebug("Can't create TwoSided.pso\n");
		return hr;
	}


	hr = LoadAndCreateShader(GetFilePath("TwoSidedNoLight.pso"), NULL, 0,
							 SHADERTYPE_PIXEL, &m_dwTwoSidedNoLightShader);
	if (FAILED(hr))
	{
		FDebug("Can't create TwoSidedNoLight.pso\n");
		return hr;
	}


	////////////////////////////////////////////////////////
	// Load vertex shader for two-sided lighting

	vector<DWORD> Declaration;

	// Microsoft's D3DVSD_REG is not working properly, so use
	//  this STL vector method instead

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3  )); // Pos
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3  )); // Normal
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_D3DCOLOR)); // Diffuse
	Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT2  )); // Texture
	Declaration.push_back(D3DVSD_END());


	m_dwTwoSidedVShader = 0;
	hr = LoadAndCreateShader(GetFilePath("TwoSided.vso"), &Declaration[0], 0,
							 SHADERTYPE_VERTEX, &m_dwTwoSidedVShader);
	if (FAILED(hr))
	{
		assert( false );
		return hr;
	}

	////////////////////////////////////////////////////////

	m_pD3DDev->SetRenderState(D3DRS_LIGHTING,TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESS);
	m_pD3DDev->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
	m_pD3DDev->SetRenderState(D3DRS_LOCALVIEWER,TRUE);

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if( m_bWireframe )
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
	}

	
	// Set light color in vertex shader constant
	DWORD lcol = 0x00FFFFFF;			// white
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_COLOR, &lcol, 1);


	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("blank.tga").c_str(), &m_pLightObjTexture);
	if (FAILED(hr))
	{
		FDebug("Could not create m_pLightObjTexture texture!");
		return E_FAIL;
	}

	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("leaf_fr_02.jpg").c_str(), &m_pDecalTexture);
	if (FAILED(hr))
	{
		FDebug("Could not create m_pDecalTexture texture!");
		return E_FAIL;
	}

	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("leaf_trans_01.jpg").c_str(), &m_pDecalBackTexture);
	if (FAILED(hr))
	{
		FDebug("Could not create m_pDecalTexture texture!");
		return E_FAIL;
	}


	m_pD3DDev->SetTexture(0,(LPDIRECT3DBASETEXTURE8)m_pDecalTexture);

	m_pD3DDev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU | 0 );

	m_pD3DDev->SetTexture(1,(LPDIRECT3DBASETEXTURE8)m_pDecalBackTexture);

	m_pD3DDev->SetTextureStageState(1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
	m_pD3DDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU | 1 );


	m_pD3DDev->SetTexture(2,(LPDIRECT3DBASETEXTURE8)m_pBumpMapTexture);

	m_pD3DDev->SetTextureStageState(2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(2,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
	m_pD3DDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU | 2);


 


	// Camera stuff
	SetDefaultView();

	
	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);


	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);

	// CV_LIGHT_CONST -> x = ambient
	//   y = ambient for back facing illumination - transparency
    //   z = back facing const attenuation

	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_CONST, D3DXVECTOR4(0.45f, 0.1f, 0.7f, 0.0f), 1);	
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_CONST, D3DXVECTOR4(0.15f, 0.1f, 0.7f, 0.0f), 1);

	FDebug("End CShaderTwoSided::Initialize...\n");

	return S_OK;
}


void CShaderTwoSided::SetView()
{	

	// Create view matrix
	D3DXMatrixLookAtLH( &m_matView, &m_EyePos, &m_vLookAt, &m_vUp);
	
	D3DXMatrixInverse( &m_matViewInv, NULL, &m_matView );

}



void CShaderTwoSided::SetDefaultView()
{
	// also set default object parameters:
	m_nHDiv = 6;
	m_nVDiv = 35;
	m_fTwist = 0.3f;
	m_fWidth = 4.0f;

	m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
	m_pObj->GenerateNormals();


	// Set default view:
	m_EyePos.x  =	  0.0f;
	m_EyePos.y	=   -10.0f;
	m_EyePos.z	=     0.0f;

	m_vLookAt.x = 0.0f;
	m_vLookAt.y = 0.0f;
	m_vLookAt.z = 0.0f;

	m_vUp.x =  0.0f;
	m_vUp.y =  0.0f;
	m_vUp.z = -1.0f;


	// Set world matrix to view from an angle
	D3DXMatrixIdentity(&m_matWorld);


	// Set field of view

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(55.0f),
								1.0f,
								0.1f, 500.0f);		// znear,far


	SetView();
}





HRESULT CShaderTwoSided::Start()
{
	m_fAngle = 0.0f;
	return S_OK;
}


void CShaderTwoSided::Keyboard( DWORD dwKey, UINT nFlags, bool bDown)
{

	float strafe_factor = 0.1f;
	float strafe_min    = 0.1f;

	UINT charcode;

	if( bDown )
	{

		switch( dwKey )
		{
		case 'T':
			// increase tesselation
			m_nVDiv ++;
			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case 'G':
			// decrease tesselation
			m_nVDiv --;
			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case 'H':
			// increase tesselation
			m_nHDiv ++;
			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case 'F':
			// decrease tesselation
			m_nHDiv --;
			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case '.':
		case '>':
			m_fTwist += 0.05f;

			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case ',':
		case '<':
			m_fTwist -= 0.05f;

			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case 'R':
			m_nHDiv = 6;
			m_nVDiv = 35;
			m_fTwist = 0.3f;
			m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
			m_pObj->GenerateNormals();
			break;

		case 'W':
			m_bWireframe = !m_bWireframe;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			break;

		case VK_UP :
		case VK_NUMPAD8 :
			m_EyePos.y +=max( strafe_min, fabs( m_EyePos.y * strafe_factor ));
			m_vLookAt.y += max( strafe_min, fabs( m_vLookAt.y * strafe_factor ));
			SetView();
			break;

		case VK_DOWN :
		case VK_NUMPAD2 :
			m_EyePos.y -=max( strafe_min, fabs( m_EyePos.y * strafe_factor ));
			m_vLookAt.y -= max( strafe_min, fabs( m_vLookAt.y * strafe_factor ));
			SetView();
			break;

		case VK_LEFT :
		case VK_NUMPAD4 :
			m_EyePos.x -=max( strafe_min, fabs( m_EyePos.x * strafe_factor ));
			m_vLookAt.x -= max( strafe_min, fabs( m_vLookAt.x * strafe_factor ));
			SetView();
			break;

		case VK_RIGHT :
		case VK_NUMPAD6 :
			m_EyePos.x += max( strafe_min, fabs( m_EyePos.x * strafe_factor ));
			m_vLookAt.x += max( strafe_min, fabs( m_vLookAt.x * strafe_factor ));
			SetView();
			break;

		case VK_NUMPAD9 :
		case VK_PRIOR :		// pageup
			m_EyePos.z += max( strafe_min, fabs( m_EyePos.z * strafe_factor ));
			m_vLookAt.z += max( strafe_min, fabs( m_vLookAt.z * strafe_factor ));
			SetView();
			break;

		case VK_NUMPAD3 :
		case VK_NEXT    :   // pagedown
			m_EyePos.z -= max( strafe_min, fabs( m_EyePos.z * strafe_factor ));
			m_vLookAt.z -= max( strafe_min, fabs( m_vLookAt.z * strafe_factor ));
			SetView();
			break;

		case VK_HOME :
		case VK_NUMPAD7 :
			SetDefaultView();
			break;

		default:

			charcode = MapVirtualKey( dwKey, 2 ); // VK to char code
			switch( charcode )
			{

			case '.':
			case '>':
				m_fTwist += 0.05f;

				m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
				m_pObj->GenerateNormals();
				break;

			case ',':
			case '<':
				m_fTwist -= 0.05f;

				m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
				m_pObj->GenerateNormals();
				break;

			case '[':
			case '{':
				m_fWidth -= 0.05f;
				m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
				m_pObj->GenerateNormals();
				break;

			case ']':
			case '}':
				m_fWidth += 0.05f;
				m_pObj->InitSpiral( m_fWidth, 10.0f, m_nHDiv, m_nVDiv, m_fTwist );
				m_pObj->GenerateNormals();
				break;
			};

			break;
		}
	}
}

  
HRESULT CShaderTwoSided::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld, matInvWorld;
	D3DXMATRIX matTemp;

	D3DXVECTOR4  eyeObjSpc, lightObjSpc;	   // eye and light positions in object space

	// Clear to grey
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);


	if( m_bWireframe )
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
	}


	// Increase rotation
	m_fAngle = 9.0f * ((float)timeGetTime())/1000.0f;

	// Set light position:
	float lightr = 2.5f;
	float lang  =  m_fAngle * 8.0f;

	m_LightPos.x  = lightr * (float)sin( D3DXToRadian( lang ) );
	m_LightPos.y  = lightr * (float)cos( D3DXToRadian( lang ) );
	m_LightPos.z  = 4.0f   * (float)sin( D3DXToRadian( lang * 0.5f ) );



	// Uncomment for no object rotation
	//	m_fAngle = 90.0f;


	// World matrix for rendering twisted sheet object
	D3DXVECTOR3 vRotation( 0.0f, 0.0f, 1.0f);
	D3DXMatrixRotationAxis( &matWorld, &vRotation, D3DXToRadian(m_fAngle));

	// compute inverse of rotation matrix
	D3DXMatrixTranspose( &matInvWorld, &matWorld );

	// Transform light and cam pos to object space and set vshader constants

	D3DXVec3Transform( &lightObjSpc, &m_LightPos, &matInvWorld );
	D3DXVec3Transform( &eyeObjSpc, &m_EyePos, &matInvWorld );

	// Set vshader constants
	m_pD3DDev->SetVertexShaderConstant( CV_LIGHT_POS_OSPACE, (void*)&lightObjSpc.x, 1 );
	m_pD3DDev->SetVertexShaderConstant( CV_EYE_POS_OSPACE,   (void*)&eyeObjSpc.x,   1 );



	// Set vshader constant to world * view * proj matrix:

    D3DXMatrixMultiply( &matTemp, &matWorld, &m_matView );
	D3DXMatrixMultiply( &matTemp, &matTemp, &m_matProj );

	// Set 4 constants starting with CV_WORLDVIEWPROJ_0
	D3DXMatrixTranspose( &matTemp, &matTemp );
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matTemp(0, 0), 4);



	m_pD3DDev->SetVertexShader(m_dwTwoSidedVShader);
	m_pD3DDev->SetPixelShader(m_dwTwoSidedPixelShader);

	m_pD3DDev->SetTexture(0,(LPDIRECT3DBASETEXTURE8)m_pDecalTexture);


	// Draw the twisted strip object
	if( m_pObj != NULL )
	{
		m_pObj->Draw( m_pD3DDev );
	}



	// Set world matrix to light position & render light
	//  stand-in object:

	D3DXMatrixTranslation( &matWorld, m_LightPos.x, m_LightPos.y, m_LightPos.z );

	// Setup the transforms
	m_pD3DDev->SetTransform(D3DTS_PROJECTION,	&m_matProj  );
	m_pD3DDev->SetTransform(D3DTS_VIEW,			&m_matView  );
	m_pD3DDev->SetTransform(D3DTS_WORLD,		&matWorld );

	m_pD3DDev->SetVertexShader( SOBJVERT_FVF			    );
	m_pD3DDev->SetPixelShader(  m_dwTwoSidedNoLightShader	);


	m_pD3DDev->SetTexture(0,(LPDIRECT3DBASETEXTURE8) m_pLightObjTexture );

	// Draw the light object
	if( m_pObjLight != NULL )
	{
		m_pObjLight->Draw( m_pD3DDev );
	}


	return hr;
}


HRESULT CShaderTwoSided::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}

	if (!(pCaps->MaxTextureBlendStages >= 2))
	{
		m_strLastError = "Not enough texture blend stages!";
		return E_FAIL;
	}

	if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}

	return S_OK;
}


