/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "nvmesh.h"
#include "nvdevice.h"
#include "shader_Reflect_and_Refract.h"
#include "Reflect_and_Refract.h"

using namespace nv_objects;
using namespace std;


DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderReflectRefract();
}

}


CShaderReflectRefract::CShaderReflectRefract()
	: m_pCubeTexture(NULL),
		m_pSphere(NULL),
		m_bMipMap(true),
		m_bWireframe(false),
		m_pUI(NULL),
		m_pNVDevice(NULL),
		m_pWorldBoxVertices(NULL),
		m_pWorldBoxIndices(NULL),
		m_fRefraction(0.4f)
{
	for (int i = 0; i < 6; i++)
	{
		m_pWorldTextures[i] = NULL;
	}

	m_strEffectName = "Reflect and Refract";
	m_strEffectLocation = "Vertex Shaders\\Texture Coordinate Generation";
	m_strEffectPixelShader = "";
	m_strEffectVertexShader = GetFilePath("Reflect_and_Refract.nvv");
}

CShaderReflectRefract::~CShaderReflectRefract()
{
	Free();	
}

void CShaderReflectRefract::UpdateProperties()
{
	EBEffect::UpdateProperties();
	AddProperty(new EBProperty("MipMap the CubeMap", OBJECT_MEMBER(m_bMipMap), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Reflection and Refraction Calculation", GetFilePath("Reflect_and_Refract.nvv"), EBTYPE_STRING_PROP));

}

HRESULT CShaderReflectRefract::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		m_strLastError = "Device does not support cubemaps!";
		return E_FAIL;
	}

	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}

	return S_OK;
}

#define NUM_CUBE_VERTICES (4*6)
#define NUM_CUBE_INDICES  (6*6)
HRESULT CShaderReflectRefract::CreateCube(WorldBoxVertex* pVertices, WORD* pIndices)
{
    // Set up the vertices for the cube.

    // -Z face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR2(1.0f, 0.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR2(0.0f, 0.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR2(0.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR2(1.0f, 1.0f));

    // +Z face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f ));

    // -Y face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR2(0.0f, 0.0f ));

    // +Y face
	*pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f ));

    // -X face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR2(1.0f, 1.0f ));

    // +X face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR2(0.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f ));


    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;

    return S_OK;
}
HRESULT CShaderReflectRefract::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;



	m_pD3DDev = pDev;
	pDev->AddRef();
	m_pNVDevice = new NVDevice(pDev);

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("Reflect_and_REFRACT.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

	m_pSphere = new NVMesh();
	hr = m_pSphere->Create(m_pNVDevice, GetFilePath("Sphere4.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load Sphere4.x";
		return hr;
	}

	m_pSphere->SetFVF(m_pNVDevice, D3DFVF_XYZ | D3DFVF_NORMAL);
	m_pSphere->SetVertexShader(m_dwCurrentShader);


    hr = m_pD3DDev->CreateVertexBuffer(NUM_CUBE_VERTICES * sizeof(WorldBoxVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pWorldBoxVertices);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create vertex buffer!";
		return hr;
	}

	hr = m_pD3DDev->CreateIndexBuffer(NUM_CUBE_INDICES * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pWorldBoxIndices);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create index buffer!";
		return hr;
	}
	WorldBoxVertex* pVertices = NULL;
	WORD* pIndices = NULL;
	hr = m_pWorldBoxVertices->Lock(0, sizeof(WorldBoxVertex) * NUM_CUBE_VERTICES,(BYTE**)&pVertices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not lock vertex buffer!";
		return hr;
	}

	hr = m_pWorldBoxIndices->Lock(0, sizeof(WORD) * NUM_CUBE_INDICES,(BYTE**)&pIndices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not lock vertex buffer!";
		return hr;
	}
	CreateCube(pVertices, pIndices);

	
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_negz.bmp").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Z]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create ";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_posz.bmp").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Z]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create ";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_negy.bmp").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Y]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create ";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_posy.bmp").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Y]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create ";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_posx.bmp").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_X]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create ";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_negx.bmp").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_X]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create ";
		return hr;
	}

	m_pWorldBoxVertices->Unlock();
	m_pWorldBoxIndices->Unlock();

	    // allocate our cube texture
	hr = D3DXCreateCubeTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlobby_cube_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pCubeTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sky_cube_mipmap.dds";
		return hr;
	}

	m_pSphere->SetTexture(m_pCubeTexture);
	m_pD3DDev->SetTexture(1,m_pCubeTexture );
	
	// tex 1 used for environment cube map 
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	// tex 0 used for rendering environment object (cube)
	//  That clamp mode is set elsewhere when box is drawn	

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_ADD);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f), 1);
//	m_pD3DDev->SetVertexShaderConstant(CV_HALF, D3DXVECTOR4(0.5f,0.5f,0.5f,0.5f), 1);

	m_pSphere->RestoreDeviceObjects(m_pNVDevice);

	return S_OK;

}

HRESULT CShaderReflectRefract::Free()
{
	for (int i = 0; i < 6; i++)
	{
		SAFE_RELEASE(m_pWorldTextures[i]);
	}

	SAFE_DELETE(m_pSphere);
	SAFE_RELEASE(m_pCubeTexture);
	SAFE_DELETE(m_pNVDevice);
	SAFE_RELEASE(m_pWorldBoxVertices);
	SAFE_RELEASE(m_pWorldBoxIndices);
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
		delete m_pUI, m_pUI = 0;
	return S_OK;
}

HRESULT CShaderReflectRefract::Start()
{
	return S_OK;
}

HRESULT CShaderReflectRefract::DrawWorldBox()
{

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    
	m_pD3DDev->SetStreamSource(0, m_pWorldBoxVertices, sizeof(WorldBoxVertex));
	m_pD3DDev->SetIndices(m_pWorldBoxIndices, 0);
	m_pD3DDev->SetVertexShader(D3DFVF_XYZ  | D3DFVF_TEX1);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Z]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 0, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Z]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 6, 2);
	
	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Y]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 12, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Y]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 18, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_X]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 24, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_X]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 30, 2);
		
	
	return S_OK;
}

HRESULT CShaderReflectRefract::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matViewInv;
	D3DXMATRIX matProj;
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;
	D3DXMATRIX matWorldViewProj;

	// Set refraction value
	m_pD3DDev->SetVertexShaderConstant(CV_REFRACT, D3DXVECTOR4(m_fRefraction, m_fRefraction, m_fRefraction, m_fRefraction ), 1);

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0x00, 0x00, 0x00 ), 1.0, 0);

	if (m_bWireframe)
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);
	}
	else
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// Set View transform
	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 10.0f, 0.0f, -0.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR4  vRotatedEyePt, newViewVec;

	D3DXMATRIX compoundMouseMatrix;
	D3DXVECTOR4 vTransEye, vTransLookat;
	D3DXVec3Transform(&vTransEye, &vEyePt, &m_pUI->GetTranslationMatrix());
	D3DXVec4Transform(&vRotatedEyePt, &vTransEye, &m_pUI->GetRotationMatrix()); 

	m_pD3DDev->SetVertexShaderConstant(CV_WORLDEYEPOS, &vRotatedEyePt, 1);

	D3DXMatrixLookAtLH(&matView, &((D3DXVECTOR3)vRotatedEyePt), &vLookatPt, &vUp);
	
    m_pD3DDev->SetTransform(D3DTS_VIEW, &matView);

	// Set Projection transform
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1.0f, 0.2f, 200.0f);

	m_pD3DDev->SetTransform(D3DTS_PROJECTION, &matProj);

	// Set World transform for sky box
	D3DXVECTOR3 vecScale;

	// Scale the sky box 
	vecScale.x = 100.0; vecScale.y = 100.0; vecScale.z = 100.0;
	
	D3DXMatrixScaling(&matWorld, vecScale.x, vecScale.y, vecScale.z);

	// Draw sky box
	m_pD3DDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	DrawWorldBox();

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);


	// Set states for sphere render
	// Using TFACTOR for blend between reflection and refraction
	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0xd0ffffff );

	// Set sphere render texture (stage 0), and stage 1 to cubemap
	m_pSphere->SetTexture(m_pCubeTexture);
	m_pD3DDev->SetTexture(1, m_pCubeTexture);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_BLENDFACTORALPHA);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	D3DXMatrixScaling(&matWorld, 1.0f,1.0f,1.0f);

	// Generate matrices for vertex shader 
	// Projection to clip space
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);	
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

	// World transform
	D3DXMatrixTranspose(&matWorld, &matWorld);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &matWorld(0, 0), 4);

	if (m_bMipMap)
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	}

	m_pSphere->Render(m_pNVDevice);

	return hr;
}


void CShaderReflectRefract::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if( (button == MOUSE_LEFTBUTTON) )
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
	return;
}

void CShaderReflectRefract::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderReflectRefract::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	if(bDown)
	{
		switch(dwKey)
		{
			case 'W':
			case 'w':
				m_bWireframe = !m_bWireframe;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
				break;

            case 'H' :
            case VK_F1 :
            {
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n + increase index of refraction\n\n - decrease index of refraction",
					   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
                break;
            }
			
			case VK_ADD:
			case 0xBB:
			case '+':
			case '=':
			{
				m_fRefraction += 0.005f;
				break;
			}

			case VK_SUBTRACT:
			case 0xBD:
			case '-':
			case '_':
			{
				m_fRefraction -= 0.005f;
				if(m_fRefraction < 0)
					m_fRefraction = 0;
				break;
			}

            case VK_HOME :
            case VK_END :
			case VK_NUMPAD7 :
			case VK_NUMPAD1 :
            case '7' :
            case '1' :
                m_pUI->Reset();
                m_bWireframe = false;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            
            case VK_UP :
			case VK_NUMPAD8 :
            case '8' :
                m_pUI->Translate(  -0.5f, 0.0f, 0.0f );
                break;
            
            case VK_DOWN :
			case VK_NUMPAD2 :
            case '2' :
                m_pUI->Translate( 0.5f, 0.0f, 0.0f );
                break;
		}
	}
	return;
}
