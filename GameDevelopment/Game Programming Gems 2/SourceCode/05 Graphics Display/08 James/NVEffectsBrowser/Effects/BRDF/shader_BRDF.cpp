/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\BRDF
File:  shader_BRDF.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include "eb_effect.h"
#include "nvinc.h"
#include "shader_BRDF.h"

// constant memory definitions for the vertex/pixel shader
#include "Constants.h"

#define MINNAERT_CONSTANT .22456
class TLVertex
{
public:
	D3DXVECTOR4 Position;
	DWORD Diffuse;
	DWORD Specular;
	D3DXVECTOR2 Texture;
};

#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

#define DRESS_INDEX 1
//#define TESTQUAD 1

using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CShaderBRDF();
			break;
		default:
			return NULL;
		}
	}
	
}

HRESULT CreateNormalizationCubeMap(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, WORD dwMipmaps, LPDIRECT3DCUBETEXTURE8* pTextureReturn)
{
	HRESULT hr;
	LPDIRECT3DCUBETEXTURE8 pCubeTexture;

	assert(pTextureReturn);

	*pTextureReturn = NULL;

	hr = D3DXCreateCubeTexture(pD3DDev, dwWidth, dwMipmaps, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pCubeTexture);
	if(FAILED(hr))
	{
		return hr;
	}

	for (int i = 0; i < 6; i++)
	{
		D3DLOCKED_RECT Locked;
		D3DXVECTOR3 Normal;
		float w,h;
		D3DSURFACE_DESC ddsdDesc;
		
		pCubeTexture->GetLevelDesc(0, &ddsdDesc);

		pCubeTexture->LockRect((D3DCUBEMAP_FACES)i, 0, &Locked, NULL, 0);

		for (int y = 0; y < ddsdDesc.Height; y++)
		{
			h = (float)y / ((float)(ddsdDesc.Height - 1));
			h *= 2.0f;
			h -= 1.0f;

			for (int x = 0; x < ddsdDesc.Width; x++)
			{
				w = (float)x / ((float)(ddsdDesc.Width - 1));
				w *= 2.0f;
				w -= 1.0f;

				DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits += x;

				switch((D3DCUBEMAP_FACES)i)
				{
					case D3DCUBEMAP_FACE_POSITIVE_X:
						Normal = D3DXVECTOR3(1.0f, -h, -w);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_X:
						Normal = D3DXVECTOR3(-1.0f, -h, w);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Y:
						Normal = D3DXVECTOR3(w, 1.0f, h);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Y:
						Normal = D3DXVECTOR3(w, -1.0f, -h);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Z:
						Normal = D3DXVECTOR3(w, -h, 1.0f);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Z:
						Normal = D3DXVECTOR3(-w, -h, -1.0f);
						break;
					default:
						assert(0);
						break;
				}

				D3DXVec3Normalize(&Normal, &Normal);

				// Scale to be a color from 0 to 255 (127 is 0)
				Normal += D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				Normal *= 127.0f;

				// Store the color
				*pBits = (DWORD)(((DWORD)Normal.x << 16) | ((DWORD)Normal.y << 8) | ((DWORD)Normal.z << 0));

			}
		}
		pCubeTexture->UnlockRect((D3DCUBEMAP_FACES)i, 0);
	}

	if ((dwMipmaps == 0) || (dwMipmaps > 1))
		D3DXFilterCubeTexture(pCubeTexture, NULL, 0, D3DX_FILTER_LINEAR);

	*pTextureReturn = pCubeTexture;

	return S_OK;
}

HRESULT CreateMinnaertMap(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, DWORD dwHeight, WORD dwMipmaps, 
						  LPDIRECT3DTEXTURE8* pTextureReturn, double MinnaertPower)
{
	HRESULT hr;
	LPDIRECT3DTEXTURE8 pTexture;
	D3DLOCKED_RECT Locked;
	D3DSURFACE_DESC ddsdDesc;
	int x,y;

	assert(pTextureReturn);

	*pTextureReturn = NULL;

	hr = D3DXCreateTexture(pD3DDev, dwWidth, dwHeight, dwMipmaps, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTexture);
	if(FAILED(hr))
	{
		return hr;
	}
	
	pTexture->GetLevelDesc(0, &ddsdDesc);

	pTexture->LockRect(0, &Locked, NULL, 0);

	for (y = 0; y < dwHeight; y++)
	{
		for (x = 0; x < dwWidth; x++)
		{
			DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
			pBits += x;

			double lPower;
			double ePower;
			double lePower;
			BYTE power;
			double w, h;

			w = ((double)(x+1)/(double)dwWidth);
			h = ((double)(dwHeight-y)/(double)dwHeight);

			lPower = pow(w, MinnaertPower);
			ePower = pow(h, 1-MinnaertPower);

			lePower = lPower * ePower;
			lePower = max(min(lePower, 1.0), 0.35);

			power = (BYTE)(lePower * 255.0f);

			*pBits = (DWORD)(((DWORD)power << 16) | ((DWORD)power << 8) | ((DWORD)power << 0));

		}

	}

	pTexture->UnlockRect(0);

	if ((dwMipmaps == 0) || (dwMipmaps > 1))
		D3DXFilterTexture(pTexture, NULL, 0, D3DX_FILTER_LINEAR);

	*pTextureReturn = pTexture;

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderBRDF Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderBRDF::CShaderBRDF()
:	m_pWomanTexture(NULL),
	m_bWireframe(false),
	m_mUI(NULL),
	m_bUseTrilinear(true),
	m_eDisplayOption(BRDFDISPLAY_MINNAERT),
	m_MinnaertPower(MINNAERT_CONSTANT),
	m_pMinnaertTexture(NULL),
	m_pVelvetBumpMapTexture(NULL),
	m_pNormalizingTexture(NULL),
	m_bAutoRotate(false),
	m_curYaw(0.0f),
	m_curPitch(0.0f),
	m_pTestQuadVB(NULL),
	m_pTexture2DVB(NULL)

{
	DWORD i;

	m_strEffectLocation = "Pixel Shaders\\Minnaert Reflection Lighting";
	m_strEffectName = "BRDF Shading";
	m_strEffectVertexShader = GetFilePath("BRDF.nvv");
	m_strEffectPixelShader = "";
	
#ifdef TESTQUAD
	m_LightDir.x = 0.0f;
	m_LightDir.y = 0.0f;
	m_LightDir.z = 1.0f;
#else
	m_LightDir.x = -1.0f;
	m_LightDir.y = -1.0f;
	m_LightDir.z = 1.0f;
//	m_LightDir.x = 0.5f;
//	m_LightDir.y = -0.5f;
//	m_LightDir.z = 0.5f;
#endif
	D3DXVec3Normalize(&m_LightDir, &m_LightDir);

	m_VRotation.x = 0.5f;
	m_VRotation.y = -0.5f;
	m_VRotation.z = 0.5f,
	D3DXVec3Normalize(&m_VRotation, &m_VRotation);

	for (i=0; i< NUM_OBJECTS; i++)
	{
		m_pVertexBuffer[i] = NULL;
		m_pIndexBuffer[i] = NULL;
		m_pAttributes[i] = NULL;
		m_pMaterials[i] = NULL;
		m_dwCurrentVShader[i] = NULL;
		m_dwCurrentPShader[i] = NULL;
		m_SaveMouse_X[i] = 0;
		m_SaveMouse_Y[i] = 0;
		m_SaveMouse_bDown[i] = FALSE;
	}

	D3DXMatrixIdentity(&m_MatWorld);
	D3DXMatrixIdentity(&m_MatView);
	D3DXMatrixIdentity(&m_MatProj);
}

// ------------------------------------------------------------------------------
// CShaderBRDF Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderBRDF::~CShaderBRDF()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderBRDF::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CShaderBRDF::UpdateProperties()
{
	EBEffect::UpdateProperties();
	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Minnaert Reflections", (DWORD)BRDFDISPLAY_MINNAERT, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Shading only", (DWORD)BRDFDISPLAY_SHADING, EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	pEnumProp = new EBEnumProperty("Move/Rotate Options", OBJECT_MEMBER(m_Rotating), EBTYPE_DWORD_PROP);
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Woman", (DWORD)WOMAN_OBJECT, EBTYPE_ENUM_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Light", (DWORD)LIGHT_OBJECT, EBTYPE_ENUM_PROP));
	AddProperty(pEnumProp);

	AddProperty(new EBProperty("Auto Rotate", OBJECT_MEMBER(m_bAutoRotate), EBTYPE_BOOL_PROP));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Minnaert Lighting", GetFilePath("BRDF.nvv"), EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Minnaert Lighting", GetFilePath("BRDF.nvp"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Directional Lighting", GetFilePath("dirLight.nvv"), EBTYPE_STRING_PROP));


//ddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	


}


// ------------------------------------------------------------------------------
// CShaderBRDF::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderBRDF::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_MIPMAP))
	{
		m_strLastError = "Device does not support mipmaps!";
		return E_FAIL;
	}

	if (!(pCaps->TextureCaps & D3DPTFILTERCAPS_MIPFLINEAR))
	{
		m_bUseTrilinear = false;
	}

	if(pCaps->MaxSimultaneousTextures < 2)
	{
		m_strLastError = "Device does not support two simultaneous textuers!";
		return E_FAIL;
	}

	if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderBRDF::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderBRDF::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	DWORD i;
	
	// get the D3D device 
	m_pD3DDev = pDev;
	pDev->AddRef();
	
	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	m_pD3DDev->GetViewport(&m_D3DViewPort);
	rect.bottom = m_D3DViewPort.Height;
	rect.right  = m_D3DViewPort.Width;
	m_mUI = new MouseUI((const RECT)rect);
	
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());
	
	// Load the vertex shader object file for regular transformed and lit 
	// and create it.
	m_dwCurrentVShader[WOMAN_OBJECT] = NULL;
	hr = LoadAndCreateShader(GetFilePath("BRDF.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, 
					&m_dwCurrentVShader[WOMAN_OBJECT]);
	if (FAILED(hr))
		return hr;

	m_dwCurrentPShader[WOMAN_OBJECT] = NULL;
	hr = LoadAndCreateShader(GetFilePath("BRDF.pso"), NULL, 0, SHADERTYPE_PIXEL, 
				&m_dwCurrentPShader[WOMAN_OBJECT]);
	if (FAILED(hr))
		return hr;

	m_dwCurrentVShader[LIGHT_OBJECT] = NULL;
	hr = LoadAndCreateShader(GetFilePath("dirLight.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, 
					&m_dwCurrentVShader[LIGHT_OBJECT]);
	

	// no pixel shader for this one.
	m_dwCurrentPShader[LIGHT_OBJECT] = NULL;
//	hr = LoadAndCreateShader(GetFilePath("dirLight.pso"), NULL, 0, SHADERTYPE_PIXEL, 
//				&m_dwCurrentPShader[LIGHT_OBJECT]);
	if (FAILED(hr))
		return hr;


	for (i=0; i< NUM_OBJECTS; i++)
	{
		hr = LoadXFile(GetFilePath(LoadObjects[i]).c_str(), TSVERTEX_FVF, i);
		if(FAILED(hr))
			return hr;
	}

#ifdef TESTQUAD
	m_Rotating = LIGHT_OBJECT;
#else
	m_Rotating = WOMAN_OBJECT;
#endif

	m_LastRotating = m_Rotating;

    m_pD3DDev->SetRenderState( D3DRS_CULLMODE,       D3DCULL_CCW ); //D3DCULL_none ); 
    m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,      FALSE);

    m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE,        TRUE );
    
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	//load textures
	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("woman.dds").c_str(), &m_pWomanTexture);
	if (FAILED(hr))
		return hr;
	
	// create a normalization cubemap
	hr = CreateNormalizationCubeMap(m_pD3DDev, 256, 9, &m_pNormalizingTexture);
	if (FAILED(hr))
		return hr;

	hr = CreateMinnaertMap(m_pD3DDev, 256, 256, 0, &m_pMinnaertTexture, m_MinnaertPower);
	if (FAILED(hr))
		return hr;

	//load textures
	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("velvetBump.dds").c_str(), &m_pVelvetBumpMapTexture);
	if (FAILED(hr))
		return hr;

	Create2DBitmap(0, 0, 100, 100);

	CreateTestQuad();

	m_pD3DDev->SetTexture(1, m_pWomanTexture);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	m_pD3DDev->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	if(m_bUseTrilinear)
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	// setup our object pitched about 90 degrees, because of 3ds max exporter
	// doesn't
#ifdef TESTQUAD
	D3DXMatrixRotationYawPitchRoll(&m_MatObject[WOMAN_OBJECT], D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
#else
	D3DXMatrixIdentity(&m_MatObject[WOMAN_OBJECT]);
#endif
	D3DXMatrixIdentity(&m_MatObject[LIGHT_OBJECT]);

	SetLightPosition();

	// make enough room for camera to rotate around radius of object,
	// with a gap of approximately 1/4 of the size of the light object.

	SetupFixedCamera(m_ObjectRadius[WOMAN_OBJECT]+(m_ObjectRadius[LIGHT_OBJECT]*1.5f));

	return S_OK;
}

void CShaderBRDF::SetupFixedCamera(float radius)
{
	m_vEyePt = D3DXVECTOR3( 0.0f, 0.0f, -2.0f*radius);
	m_vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);
	m_vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);

    D3DXMatrixLookAtLH(&m_MatView, &m_vEyePt, &m_vLookatPt, &m_vUp);
    FLOAT fAspect = m_D3DViewPort.Width / (FLOAT)m_D3DViewPort.Height;
    D3DXMatrixPerspectiveFovLH( &m_MatProj, D3DX_PI/4, fAspect, 100.0f,
                                radius*4);
}

void CShaderBRDF::UpdateObject(DWORD index)
{
    D3DXMATRIX matRotationInverse;
    D3DXMATRIX matRotation;
	D3DXMATRIX matTemp;

	D3DXMatrixTranslation( &m_MatWorld, -m_ObjectCenter[index].x,
			                              -m_ObjectCenter[index].y,
					                      -m_ObjectCenter[index].z );
	switch (index)
	{
	case WOMAN_OBJECT:

		if (m_Rotating == WOMAN_OBJECT)
		{
			if (m_bAutoRotate)
			{
				// rotate about y axis
				m_curPitch += D3DXToRadian(1.0f);
				m_curYaw += D3DXToRadian(5.0f);
				D3DXMatrixRotationAxis(&m_MatObject[WOMAN_OBJECT], &m_VRotation, m_curYaw);
			}
			else 
			{
				D3DXMatrixMultiply( &m_MatObject[WOMAN_OBJECT], &m_mUI->GetRotationMatrix(), &m_mUI->GetTranslationMatrix() );
			}
		}

		D3DXMatrixMultiply( &m_MatWorld, &m_MatWorld, &m_MatObject[WOMAN_OBJECT] );

		break;

	case LIGHT_OBJECT:
		if (m_Rotating == LIGHT_OBJECT)
		{
			if (m_bAutoRotate)
			{
				m_curPitch += D3DXToRadian(1.0f);
				m_curYaw += D3DXToRadian(5.0f);
				D3DXMatrixRotationAxis(&matTemp, &m_VRotation, m_curYaw);
			}
			else
			{
				matTemp = m_mUI->GetRotationMatrix();
			}

			m_LightDir.x = 0.0f;
			m_LightDir.y = 0.0f;
			m_LightDir.z = 1.0f;
			D3DXVec3Normalize(&m_LightDir, &m_LightDir);

			D3DXVec3TransformNormal(&m_LightDir, &m_LightDir, &matTemp);
			SetLightPosition();
		}

		D3DXMatrixMultiply( &m_MatWorld, &m_MatWorld, &m_MatObject[LIGHT_OBJECT]);

		break;
	}

}

void CShaderBRDF::SetLightPosition()
{
	D3DXMATRIX matTemp;
	D3DXVECTOR3 lightPos(0, 0, -m_ObjectRadius[WOMAN_OBJECT]+(m_ObjectRadius[LIGHT_OBJECT]*.25f));
	D3DXVECTOR3 vLightObjectDir(0.0f, 0.0f, 1.0f);

	m_mUI->GetTranslationMatrix(&matTemp);
	D3DXVec3TransformCoord(&lightPos, &lightPos, &matTemp);

	// Create the rotation axis
	D3DXVECTOR3 vOrthoNormal;
	D3DXVec3Cross(&vOrthoNormal, &m_LightDir, &vLightObjectDir);
	D3DXVec3Normalize(&vOrthoNormal, &vOrthoNormal);

	// Calculate the angle between the two vectors.
	float fAngle = acos(D3DXVec3Dot(&m_LightDir, &vLightObjectDir));

	// Rotate the object about our rotation axis to map one vector onto another
	D3DXMatrixRotationAxis(&m_MatObject[LIGHT_OBJECT], &vOrthoNormal, -fAngle);

	D3DXMatrixTranslation( &matTemp, lightPos.x, lightPos.y, lightPos.z);
	D3DXMatrixMultiply(&m_MatObject[LIGHT_OBJECT], &matTemp, &m_MatObject[LIGHT_OBJECT]);

}

HRESULT CShaderBRDF::SetVertexShaderMatrices(DWORD index)
{
    // set up vertex shading constants to contain proper
    // transformation matrices etc.
    D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldI;

	UpdateObject(index);

	D3DXMatrixInverse(&matWorldI, NULL, &m_MatWorld);

	D3DXMatrixMultiply(&matWorldView,     &m_MatWorld,     &m_MatView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &m_MatProj);
	
	// Write Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

	// made this a vector 4 so, we wouldn't have to set the w part in the shader
	D3DXVECTOR4 vecLight;
	// Transform from world space to model space and point it away from the vertex towards the light
	// Light is pointing into the world
	D3DXVec3TransformNormal((D3DXVECTOR3*)&vecLight, &m_LightDir, &matWorldI);
	D3DXVec3Normalize((D3DXVECTOR3*)&vecLight, (D3DXVECTOR3*)&vecLight);
	// Shader math requires that the vector is to the light
	vecLight = -vecLight;
	vecLight.w = 1.0f;

	m_pD3DDev->SetVertexShaderConstant(CVP_LIGHT_DIRECTION, &vecLight, 1);

	// bias and scale, because pixel shader will clamp these values
	vecLight += D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 0.0f );
	vecLight *= .5f;
	vecLight.w = 1.0f;
	m_pD3DDev->SetPixelShaderConstant(CVP_LIGHT_DIRECTION, &vecLight, 1);

	// made this a vector 4 so, we wouldn't have to set the w part in the shader
	D3DXVECTOR4 eyePos;
	// put eye into object space.
	D3DXVec3TransformCoord((D3DXVECTOR3*)&eyePos, &m_vEyePt, &matWorldI);
	eyePos.w = 1.0f;
	m_pD3DDev->SetVertexShaderConstant(CV_EYE_POSITION, &eyePos, 1);

	D3DXVECTOR4 vecOne(1.0f, 1.0f, 1.0f, 1.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, &vecOne, 1);

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderBRDF::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderBRDF::Free()
{
	DWORD i;

	for (i=0; i<NUM_OBJECTS; i++)
	{
		SAFE_RELEASE(m_pVertexBuffer[i]);
		SAFE_RELEASE(m_pIndexBuffer[i]);
	
		SAFE_DELETE_ARRAY(m_pAttributes[i]);
		SAFE_DELETE_ARRAY(m_pMaterials[i]);

		if (m_pD3DDev)
		{
			if (m_dwCurrentVShader[i] != 0)
			{
				m_pD3DDev->DeleteVertexShader(m_dwCurrentVShader[i]);
				m_dwCurrentVShader[i] = 0;
			}

			if (m_dwCurrentPShader[i] != 0)
			{
				m_pD3DDev->DeletePixelShader(m_dwCurrentPShader[i]);
				m_dwCurrentPShader[i] = 0;
			}
		}

	}
	
	SAFE_RELEASE(m_pD3DDev);
	SAFE_RELEASE(m_pWomanTexture);
	SAFE_RELEASE(m_pMinnaertTexture);
	SAFE_RELEASE(m_pNormalizingTexture);
	SAFE_RELEASE(m_pVelvetBumpMapTexture);
	SAFE_RELEASE(m_pTestQuadVB);	
	SAFE_RELEASE(m_pTexture2DVB);
	SAFE_DELETE(m_mUI);
	
	return S_OK;
}

HRESULT CShaderBRDF::LoadXFile(const char* fileName, const DWORD dwFVF, DWORD index)
{
	ID3DXMesh *tempMesh, *tempMeshFVF, *tempMeshOpt;
    LPBYTE pSrc;
	
	HRESULT hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_SYSTEMMEM, m_pD3DDev, NULL, 
		(ID3DXBuffer**)&m_pMaterials[index], &m_dwNumSections[index], &tempMesh);
	
	//convert to our format
	hr = tempMesh->CloneMeshFVF(D3DXMESH_DYNAMIC, dwFVF, m_pD3DDev, &tempMeshFVF);
	
	tempMeshFVF->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);

	DWORD attribSize = m_dwNumSections[index];
	hr = tempMeshOpt->GetAttributeTable(NULL, &attribSize);
	
	m_pAttributes[index] = new D3DXATTRIBUTERANGE[attribSize];
	
	hr = tempMeshOpt->GetAttributeTable(m_pAttributes[index], &attribSize);
	
	tempMeshOpt->GetVertexBuffer(&m_pVertexBuffer[index]);
	tempMeshOpt->GetIndexBuffer(&m_pIndexBuffer[index]);

	m_pVertexBuffer[index]->Lock( 0, 0, (BYTE**)&pSrc, 0 );
	// grab bound sphere, so we may initially scale our view.
	D3DXComputeBoundingSphere(pSrc, tempMeshOpt->GetNumVertices(), dwFVF, &m_ObjectCenter[index], &m_ObjectRadius[index]);

	m_pVertexBuffer[index]->Unlock();


	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshFVF);
	SAFE_RELEASE(tempMeshOpt);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderBRDF::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderBRDF::Start()
{
	return S_OK;
}

// this really should be a general drawing function inside the effects browser
// code.
HRESULT CShaderBRDF::Draw2DBitmap()
{
	DWORD dwVertexShader;
	DWORD dwPixelShader;
	UINT VBStride;
	LPDIRECT3DVERTEXBUFFER8 pVB;
	LPDIRECT3DBASETEXTURE8 pTexture;
	DWORD i;

	typedef struct tagRenderStates
	{
		D3DRENDERSTATETYPE Type;
		DWORD Value;
	} tRenderStates;

	typedef struct tagTextureStageStates
	{
		D3DTEXTURESTAGESTATETYPE Type;
		DWORD Value;
	} tTextureStates;

	tRenderStates SavedRender[] = 
	{
		{ D3DRS_ZENABLE, 0 }, 
		{ D3DRS_ALPHATESTENABLE, 0 },
		{ D3DRS_ALPHAREF, 0 },
		{ D3DRS_ALPHAFUNC, 0 },
		{ D3DRS_ALPHABLENDENABLE, 0 },
		{ D3DRS_SRCBLEND, 0 },
		{ D3DRS_DESTBLEND, 0 },
		{ D3DRS_FOGENABLE, 0 },
		{ D3DRS_CULLMODE, 0 },
		{ D3DRS_FILLMODE, 0 },
		{ D3DRS_WRAP0, 0}
	};

	tTextureStates SavedTextureStage0[] = 
	{
		{ D3DTSS_COLOROP, 0 }, 
		{ D3DTSS_COLORARG1, 0 },
		{ D3DTSS_ALPHAOP, 0 },
		{ D3DTSS_ALPHAARG1, 0 },
		{ D3DTSS_TEXCOORDINDEX, 0 },
		{ D3DTSS_MINFILTER, 0 },
		{ D3DTSS_MAGFILTER, 0 },
	};

	tTextureStates SavedTextureStage1[] = 
	{
		{ D3DTSS_COLOROP, 0 }, 
		{ D3DTSS_ALPHAOP, 0 },
		{ D3DTSS_TEXCOORDINDEX, 0 }
	};

	// Save state
	m_pD3DDev->GetVertexShader(&dwVertexShader);
	m_pD3DDev->GetPixelShader(&dwPixelShader);
	m_pD3DDev->GetStreamSource(0, &pVB, &VBStride);
	m_pD3DDev->GetTexture(0, &pTexture);
	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		m_pD3DDev->GetRenderState(SavedRender[i].Type, &SavedRender[i].Value);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->GetTextureStageState(0, SavedTextureStage0[i].Type, &SavedTextureStage0[i].Value);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->GetTextureStageState(1, SavedTextureStage1[i].Type, &SavedTextureStage1[i].Value);
	}

	// draw logo
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	m_pD3DDev->SetStreamSource(0, m_pTexture2DVB, sizeof(TLVertex));
	m_pD3DDev->SetVertexShader(D3DFVF_TLVERTEX);
	m_pD3DDev->SetPixelShader(0);

	// Enable alpha testing to remove the outer pixels
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHAREF, 0x80 );
	m_pD3DDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_pD3DDev->SetRenderState( D3DRS_WRAP0, 0);

	m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );

	// Restore state
	m_pD3DDev->SetVertexShader(dwVertexShader);
	m_pD3DDev->SetStreamSource(0, pVB, VBStride);
	m_pD3DDev->SetTexture(0, pTexture);
	m_pD3DDev->SetPixelShader(dwPixelShader);

	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pVB);

	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		m_pD3DDev->SetRenderState(SavedRender[i].Type, SavedRender[i].Value);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->SetTextureStageState(0, SavedTextureStage0[i].Type, SavedTextureStage0[i].Value);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->SetTextureStageState(1, SavedTextureStage1[i].Type, SavedTextureStage1[i].Value);
	}

	return S_OK;
}

HRESULT CShaderBRDF::Create2DBitmap(DWORD x, DWORD y, DWORD width, DWORD height)
{
    TLVertex* v;
	HRESULT hr;

		// Create a big square for rendering the Logo mask
    hr = m_pD3DDev->CreateVertexBuffer( 4*sizeof(TLVertex),
                                                 D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX, 
                                                  D3DPOOL_MANAGED, &m_pTexture2DVB );
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed to create logo vertex buffer!", "ERROR", MB_OK);
		return hr;
	}

	hr = m_pTexture2DVB->Lock( 0, 0, (BYTE**)&v, 0 );
	if (SUCCEEDED(hr))
	{
		v[0].Position = D3DXVECTOR4(x, y, 0, 1);
		v[0].Diffuse = 0xffffffff;
		v[0].Specular = 0;
		v[0].Texture = D3DXVECTOR2(0, 0);

		v[1].Position = D3DXVECTOR4(x, y+height, 0, 1);
		v[1].Diffuse = 0xffffffff;
		v[1].Specular = 0;
		v[1].Texture = D3DXVECTOR2(0, 1);
    
		v[2].Position = D3DXVECTOR4(x+width, y, 0, 1);
		v[2].Diffuse = 0xffffffff;
		v[2].Specular = 0;
		v[2].Texture = D3DXVECTOR2(1, 0);

		v[3].Position = D3DXVECTOR4(x+width, y+width, 0, 1);
		v[3].Diffuse = 0xffffffff;
		v[3].Specular = 0;
		v[3].Texture = D3DXVECTOR2(1, 1);

		m_pTexture2DVB->Unlock();
	}

	return hr;

}

HRESULT CShaderBRDF::CreateTestQuad()
{
    TSVertex* v;
	HRESULT hr;
	float x,y,width,height;

	width = height = m_ObjectRadius[WOMAN_OBJECT]/2;
	x = m_ObjectCenter[WOMAN_OBJECT].x;
	y = m_ObjectCenter[WOMAN_OBJECT].y;


		// Create a big square for rendering the Logo mask
    hr = m_pD3DDev->CreateVertexBuffer( 4*sizeof(TSVertex),
                                                 D3DUSAGE_WRITEONLY, TSVERTEX_FVF, 
                                                  D3DPOOL_MANAGED, &m_pTestQuadVB );
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed to create Test Quad vertex buffer!", "ERROR", MB_OK);
		return hr;
	}

	hr = m_pTestQuadVB->Lock( 0, 0, (BYTE**)&v, 0 );
	if (SUCCEEDED(hr))
	{
		v[0].Position = D3DXVECTOR3(x, y, m_ObjectCenter[WOMAN_OBJECT].z);
		v[0].Normal = D3DXVECTOR3(0, 0, -1);
		v[0].Texture0 = D3DXVECTOR2(0, 0);

		v[1].Position = D3DXVECTOR3(x+width, y, m_ObjectCenter[WOMAN_OBJECT].z);
		v[1].Normal = D3DXVECTOR3(0, 0, -1);
		v[1].Texture0 = D3DXVECTOR2(1, 0);
    
		v[2].Position = D3DXVECTOR3(x, y-height, m_ObjectCenter[WOMAN_OBJECT].z);
		v[2].Normal = D3DXVECTOR3(0, 0, -1);
		v[2].Texture0 = D3DXVECTOR2(0, 1);

		v[3].Position = D3DXVECTOR3(x+width, y-height, m_ObjectCenter[WOMAN_OBJECT].z);
		v[3].Normal = D3DXVECTOR3(0, 0, -1);
		v[3].Texture0 = D3DXVECTOR2(1, 1);

		m_pTestQuadVB->Unlock();
	}

	return hr;

}

HRESULT CShaderBRDF::SetTextureStageStates(DWORD materialNum, DWORD index)
{
	D3DXMATERIAL *tMaterials;

	tMaterials = (D3DXMATERIAL *)m_pMaterials[index]->GetBufferPointer();
    // Setup render states
	if (tMaterials[materialNum].pTextureFilename != NULL)
	{
		// setup to use textures
		m_pD3DDev->SetTexture(0, m_pWomanTexture);

	    m_pD3DDev->SetRenderState(D3DRS_LIGHTING,	     FALSE );
		m_pD3DDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	}
	else
	{
		// setup to use textures
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	}
	
	return S_OK;
}
// ------------------------------------------------------------------------------
// CShaderBRDF::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderBRDF::Tick(EBTimer* pTimer)
{
	D3DXMATERIAL *tMaterials;
	HRESULT hr = S_OK;
	DWORD i;

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));

	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	SetVertexShaderMatrices(LIGHT_OBJECT);
	// draw the spotlight
	m_pD3DDev->SetVertexShader(m_dwCurrentVShader[LIGHT_OBJECT]);
	m_pD3DDev->SetPixelShader(m_dwCurrentPShader[LIGHT_OBJECT]);

	// use this vertex buffer as our source for vertices
	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer[LIGHT_OBJECT], sizeof(TSVertex));
	if (FAILED(hr))
		return hr;	
	//set index buffer
	m_pD3DDev->SetIndices(m_pIndexBuffer[LIGHT_OBJECT], 0);

	for(i = 0; i < m_dwNumSections[LIGHT_OBJECT]; i++)
	{
		SetTextureStageStates(i, LIGHT_OBJECT);
		tMaterials = (D3DXMATERIAL *)m_pMaterials[LIGHT_OBJECT]->GetBufferPointer();
		m_pD3DDev->SetVertexShaderConstant(CV_MATERIAL_COLOR, &tMaterials[i].MatD3D.Diffuse, 1);

		m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_pAttributes[LIGHT_OBJECT][i].VertexStart,
			m_pAttributes[LIGHT_OBJECT][i].VertexCount, m_pAttributes[LIGHT_OBJECT][i].FaceStart*3, 
			m_pAttributes[LIGHT_OBJECT][i].FaceCount);
	}

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	// Now for the dress, Minnaert light it in second pass
	// setup to use textures, normalizing, Minnaert reflections texture
	m_pD3DDev->SetTexture(0, m_pNormalizingTexture);
	m_pD3DDev->SetTexture(1, m_pNormalizingTexture);
	m_pD3DDev->SetTexture(2, m_pMinnaertTexture);
	m_pD3DDev->SetTexture(3, m_pNormalizingTexture);
//	m_pD3DDev->SetTexture(3, m_pVelvetBumpMapTexture);

	// since Microsoft doesn't keep the hierarchy names around just use a hardcoded
	// number
	tMaterials = (D3DXMATERIAL *)m_pMaterials[WOMAN_OBJECT]->GetBufferPointer();
	m_pD3DDev->SetVertexShaderConstant(CV_MATERIAL_COLOR, &tMaterials[DRESS_INDEX].MatD3D.Diffuse, 1);

	SetVertexShaderMatrices(WOMAN_OBJECT);
	if (m_eDisplayOption == BRDFDISPLAY_SHADING)
	{
		// first pass just use fixed function pipeline
		m_pD3DDev->SetVertexShader(m_dwCurrentVShader[LIGHT_OBJECT]);
		m_pD3DDev->SetPixelShader(m_dwCurrentPShader[LIGHT_OBJECT]);
		SetTextureStageStates(i, WOMAN_OBJECT);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}
	else
	{
		// we are using BRDF mode here, so setup dress to use it
		m_pD3DDev->SetVertexShader(m_dwCurrentVShader[WOMAN_OBJECT]);
		m_pD3DDev->SetPixelShader(m_dwCurrentPShader[WOMAN_OBJECT]);
	}

#ifdef TESTQUAD
	m_pD3DDev->SetStreamSource(0, m_pTestQuadVB, sizeof(TSVertex));
	m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );
#else
	// use this vertex buffer as our source for vertices
	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer[WOMAN_OBJECT], sizeof(TSVertex));
	if (FAILED(hr))
		return hr;	
	//set index buffer
	m_pD3DDev->SetIndices(m_pIndexBuffer[WOMAN_OBJECT], 0);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_pAttributes[WOMAN_OBJECT][DRESS_INDEX].VertexStart,
					m_pAttributes[WOMAN_OBJECT][DRESS_INDEX].VertexCount, 
					m_pAttributes[WOMAN_OBJECT][DRESS_INDEX].FaceStart*3,
					m_pAttributes[WOMAN_OBJECT][DRESS_INDEX].FaceCount);


	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	// the light object uses normal lighting without minnaert
	m_pD3DDev->SetVertexShader(m_dwCurrentVShader[LIGHT_OBJECT]);
	m_pD3DDev->SetPixelShader(m_dwCurrentPShader[LIGHT_OBJECT]);

	for(i = 0; i < m_dwNumSections[WOMAN_OBJECT]; i++)
	{
		if (i != DRESS_INDEX)
		{
			SetTextureStageStates(i, WOMAN_OBJECT);
			tMaterials = (D3DXMATERIAL *)m_pMaterials[WOMAN_OBJECT]->GetBufferPointer();
			m_pD3DDev->SetVertexShaderConstant(CV_MATERIAL_COLOR, &tMaterials[i].MatD3D.Diffuse, 1);

			m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_pAttributes[WOMAN_OBJECT][i].VertexStart,
				m_pAttributes[WOMAN_OBJECT][i].VertexCount, m_pAttributes[WOMAN_OBJECT][i].FaceStart*3,
				m_pAttributes[WOMAN_OBJECT][i].FaceCount);
		}
	}

#endif

#ifdef TESTQUAD
	m_pD3DDev->SetTexture(0, m_pVelvetBumpMapTexture);
	Draw2DBitmap();
#endif

	return hr;
}


void CShaderBRDF::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if (button == MOUSE_LEFTBUTTON)
	{
		m_SaveMouse_bDown[m_Rotating] = bDown;
		if(bDown)
		{
			m_mUI->OnLButtonDown(x, y);
		}
		else
		{
			m_mUI->OnLButtonUp(x, y);
		}
	}
	return;
}

void CShaderBRDF::MouseMove(HWND hWnd, int x, int y)
{

	// this is a real hack, but the trackball code
	// won't let you set the matrix, so I have to save all the mouse
	// crap.
	if (m_Rotating != m_LastRotating)
	{
		m_LastRotating = m_Rotating;
		m_mUI->Reset();
		m_curYaw = 0.0f;
		m_curPitch = 0.0f;
		m_VRotation.z = 1.0f;
	}

	m_mUI->OnMouseMove(x, y);
	if (m_SaveMouse_bDown[m_Rotating])
	{
		m_SaveMouse_X[m_Rotating] += x;
		m_SaveMouse_Y[m_Rotating] += y;
	}
	return;
}

void CShaderBRDF::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
    if ( bDown )
    {
        switch ( dwKey )
        {

            case 'H' :
            case VK_F1 :
            {
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Ctrl Left Button & Mouse - Pan Camera \n\n Shift Left Button & Mouse - Move Camera In & Out\n\n",
					   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
                break;
            }

            case 'W' :
            {
                m_bWireframe = !m_bWireframe;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            case 'R' :
            {
                m_bWireframe = false;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

			case 'L':
			{
				m_Rotating = LIGHT_OBJECT;
                m_mUI->Reset();
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
				break;
			}

			case 'O':
			{
				m_Rotating = WOMAN_OBJECT;
                m_mUI->Reset();
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
				break;
			}

            case VK_HOME :
            case VK_END :
			case VK_NUMPAD7 :
			case VK_NUMPAD1 :
            case '7' :
            case '1' :
            {
                m_mUI->Reset();
                m_bWireframe = false;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            case VK_LEFT :
			case VK_NUMPAD4 :
            case '4' :
            {
                m_mUI->Translate( -1.0f, 0.0f, 0.0f );
                break;
            }

            case VK_RIGHT :
			case VK_NUMPAD6 :
            case '6' :
            {
                m_mUI->Translate(  1.0f, 0.0f, 0.0f );
                break;
            }

            case VK_UP :
			case VK_NUMPAD8 :
            case '8' :
            {
                m_mUI->Translate(  0.0f, 0.0f, -1.0f );
                break;
            }

            case VK_DOWN :
			case VK_NUMPAD2 :
            case '2' :
            {
                m_mUI->Translate( 0.0f, 0.0f, 1.0f );
                break;
            }

            case VK_PRIOR :
			case VK_NUMPAD9 :
            case '9' :
            {
                m_mUI->Translate(  0.0f, 1.0f, 0.0f );
                break;
            }
            case VK_NEXT :
			case VK_NUMPAD3 :
            case '3' :
            {
                m_mUI->Translate(  0.0f, -1.0f, 0.0f );
                break;
            }

            case VK_ADD :
            {
                m_mUI->Translate( 0.0f, 0.0f, 1.0f );
                break;
            }
            case VK_SUBTRACT :
            {
                m_mUI->Translate( 0.0f, 0.0f, -1.0f );
                break;
            }

            default :
            {
                break;
            }
        };
    }
	return;
}