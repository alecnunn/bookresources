/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "nvdevice.h"
#include "nvmesh.h"
#include "nvfile.h"
#include "shader_Morph.h"
#include "morph.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderMorph();
}

}

CShaderMorph::CShaderMorph()
	: m_pDolphinMap(NULL),
		m_pFloorMesh(NULL),
		m_fWeight(0.0f),
		m_pDolphinIB(NULL),
		m_pNVDevice(NULL),
		m_dwNumVertices(0),
		m_dwNumIndices(0),
		m_bWireframe(false),
		m_bPause(false)
{
	m_pDolphinVB[0] = NULL;
	m_pDolphinVB[1] = NULL;
	m_pDolphinVB[2] = NULL;

	m_strEffectLocation = "Vertex Shaders\\Deformation";
	m_strEffectName = "Morphing Dolphin";
	m_strEffectVertexShader = GetFilePath("Morph.nvv");
	m_strEffectPixelShader = "";

}

void CShaderMorph::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Vertex Tweening", GetFilePath("Morph.nvv"), EBTYPE_STRING_PROP));
}

CShaderMorph::~CShaderMorph()
{
	Free();	
}

HRESULT CShaderMorph::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	DWORD i;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVMorphDevice(pDev, this);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwSeaFloorShader = 0;
	hr = LoadAndCreateShader(GetFilePath("Morph_SeaFloor.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwSeaFloorShader);
	if (FAILED(hr))
		return hr;
	
	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_STREAM(1));
	Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(4, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(5, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwDolphinShader = 0;
	hr = LoadAndCreateShader(GetFilePath("Morph.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwDolphinShader);
	if (FAILED(hr))
		return hr;


    NVFile* pDolphinGroupObject = new NVFile();
    hr  = pDolphinGroupObject->Create(m_pNVDevice, GetFilePath("dolphin_group.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load dolphin group";
		return hr;
	}
	
	NVMesh* pDolphinMesh[3];
    pDolphinMesh[0] = (NVMesh*)pDolphinGroupObject->FindMesh( "Dolph01" );
    pDolphinMesh[1] = (NVMesh*)pDolphinGroupObject->FindMesh( "Dolph02" );
    pDolphinMesh[2] = (NVMesh*)pDolphinGroupObject->FindMesh( "Dolph03" );
	pDolphinMesh[0]->SetFVF(m_pNVDevice, DOLPHINVERTEX_FVF);
	pDolphinMesh[1]->SetFVF(m_pNVDevice, DOLPHINVERTEX_FVF);
	pDolphinMesh[2]->SetFVF(m_pNVDevice, DOLPHINVERTEX_FVF);

	D3DVERTEXBUFFER_DESC ddsdDescDolphinVB;
	D3DVERTEXBUFFER_DESC ddsdDescSrcVB;
	D3DINDEXBUFFER_DESC ddsdDescDolphinIB;

	for (i = 0; i < 3; i++)
	{
		LPDIRECT3DVERTEXBUFFER8 pSrcVB;
		pDolphinMesh[i]->GetSysMemMesh()->GetVertexBuffer(&pSrcVB);

		pSrcVB->GetDesc(&ddsdDescSrcVB);

		pDev->CreateVertexBuffer(ddsdDescSrcVB.Size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pDolphinVB[i]);

		m_pDolphinVB[i]->GetDesc(&ddsdDescDolphinVB);

		BYTE* pSrc;
		BYTE* pDest;
		m_pDolphinVB[i]->Lock(0, ddsdDescDolphinVB.Size, &pDest, D3DLOCK_NOSYSLOCK);
		pSrcVB->Lock(0, ddsdDescSrcVB.Size, &pSrc, D3DLOCK_NOSYSLOCK);

		memcpy(pDest, pSrc, ddsdDescSrcVB.Size);

		pSrcVB->Unlock();
		m_pDolphinVB[i]->Unlock();

		SAFE_RELEASE(pSrcVB);
	}

	pDolphinMesh[0]->GetSysMemMesh()->GetIndexBuffer(&m_pDolphinIB);

	m_pDolphinIB->GetDesc(&ddsdDescDolphinIB);
	m_pDolphinVB[0]->GetDesc(&ddsdDescDolphinVB);

	m_dwNumVertices = ddsdDescDolphinVB.Size / sizeof(DolphinVertex);
	switch(ddsdDescDolphinIB.Format)
	{
		case D3DFMT_INDEX16:
			m_dwNumIndices = ddsdDescDolphinIB.Size / 2;
			break;
		case D3DFMT_INDEX32:
			m_dwNumIndices = ddsdDescDolphinIB.Size / 2;
			break;
	}

	SAFE_DELETE(pDolphinGroupObject);

	
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("dolphin.dds").c_str(),
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
		&m_pDolphinMap);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create dolphin.dds";
		return hr;
	}

    for( DWORD t=0; t<32; t++ )
    {
        TCHAR strTextureName[80];
		LPDIRECT3DTEXTURE8 pTexture;
        sprintf( strTextureName, "Caust%02ld.dds", t );
        
		hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
			GetFilePath(strTextureName).c_str(),
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
			&pTexture);
		if (FAILED(hr))
		{
			m_strLastError = "Could not create ";
			m_strLastError += strTextureName;
			return E_FAIL;
		}
		m_vecCausticTextures.push_back(pTexture);
    }

	
	// Load the seafloor x file and extract its mesh
    m_pFloorMesh = new NVMesh();
    m_pFloorMesh->Create(m_pNVDevice, GetFilePath("seafloor.x"));
	m_pFloorMesh->SetFVF(m_pNVDevice, SEAFLOORVERTEX_FVF);
	m_pFloorMesh->SetVertexShader(m_dwSeaFloorShader);

    // Add some "hilliness" to the terrain
    LPDIRECT3DVERTEXBUFFER8 pVB;
    if( SUCCEEDED( m_pFloorMesh->GetSysMemMesh()->GetVertexBuffer( &pVB ) ) )
    {
        SeaFloorVertex* pVertices;
        DWORD   dwNumVertices = m_pFloorMesh->GetSysMemMesh()->GetNumVertices();

        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
		{
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Texture.x *= 10;
			pVertices[i].Texture.y *= 10;
		}

        pVB->Unlock();
        pVB->Release();
    }


	m_pFloorMesh->RestoreDeviceObjects(m_pNVDevice);

	m_pD3DDev->SetRenderState( D3DRS_FOGCOLOR, WATER_COLOR );
	m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,      TRUE );
    m_pD3DDev->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
    m_pD3DDev->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
    m_pD3DDev->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_HALF, D3DXVECTOR4(0.5f,0.5f,0.5f,0.5f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_TEXTURESCALE, D3DXVECTOR4(0.5f, 0.05f, 0.0f, 1.0f), 1);
	
	D3DXVECTOR4 vFogData;
	vFogData.x = 1.0f;	// Fog Start
	vFogData.y = 50.0f; // Fog End
	vFogData.z = 1.0f / (vFogData.y - vFogData.x); // Fog range
	vFogData.w = 255.0f;
	m_pD3DDev->SetVertexShaderConstant(CV_FOGDATA, &vFogData.x, 1.0f);

	// Camera setup
	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 1.0f, 1000.0f);

	m_pNVDevice->SetViewTransform(&matView);
	m_pNVDevice->SetProjectionTransform(&matProj);
	return S_OK;

}

HRESULT CShaderMorph::Free()
{
	SAFE_RELEASE(m_pDolphinMap);

	SAFE_RELEASE(m_pDolphinIB);
	SAFE_RELEASE(m_pDolphinVB[0]);
	SAFE_RELEASE(m_pDolphinVB[1]);
	SAFE_RELEASE(m_pDolphinVB[2]);

	while(!m_vecCausticTextures.empty())
	{
		LPDIRECT3DTEXTURE8 pTexture = m_vecCausticTextures.back();
		SAFE_RELEASE(pTexture);
		m_vecCausticTextures.pop_back();
	}

	SAFE_DELETE(m_pFloorMesh);

	SAFE_DELETE(m_pNVDevice);

	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwDolphinShader);
		m_pD3DDev->DeleteVertexShader(m_dwSeaFloorShader);
		SAFE_RELEASE(m_pD3DDev);
	}

	return S_OK;
}

HRESULT CShaderMorph::Start()
{
	return S_OK;
}

HRESULT CShaderMorph::SetTransform()
{
	D3DXMATRIX matWorld, matView, matProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;
	D3DXMATRIX matWorldViewProj;

	matWorld = m_pNVDevice->GetWorldTransform();
	matView = m_pNVDevice->GetViewTransform();
	matProj = m_pNVDevice->GetProjectionTransform();

	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
	
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	// Worldview transform
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_1, &matWorldViewIT(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_2, &matWorldViewIT(2, 0), 1);

	return S_OK;
}


HRESULT CShaderMorph::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matTemp;

	if (m_bPause)
		return S_OK;

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, WATER_COLOR, 1.0, 0);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	float fKickFreq = pTimer->GetDuration() * 2.0f;
	float fPhase = pTimer->GetDuration() / 3.0f;

	// Select the second frame to lerp with, based on the time.
	// We have 3 meshes - the middle and the top for the up phase, the 
	// middle and the bottom for the down phase
	float fSinTheta = sin(fKickFreq);
	if (fSinTheta < 0.0f)
	{
		fSinTheta = -fSinTheta;

		hr = m_pD3DDev->SetStreamSource(1, m_pDolphinVB[0], sizeof(DolphinVertex));
		if (FAILED(hr))
			return hr;
	}
	else
	{
		hr = m_pD3DDev->SetStreamSource(1, m_pDolphinVB[2], sizeof(DolphinVertex));
		if (FAILED(hr))
			return hr;
	}
	m_pD3DDev->SetStreamSource(0, m_pDolphinVB[1], sizeof(DolphinVertex));
	m_pD3DDev->SetIndices(m_pDolphinIB, 0);
	m_pD3DDev->SetVertexShader(m_dwDolphinShader);

	// Put the weights in the vertex shader
	m_pD3DDev->SetVertexShaderConstant(CV_WEIGHT, D3DXVECTOR4(1.0f - fSinTheta, fSinTheta, 0.0f, 0.0f), 1);

    // Move the dolphin in a circle
    D3DXMATRIX matScale, matTrans1, matRotate1, matRotate2;
    D3DXMatrixRotationZ( &matRotate1, -cosf(fKickFreq)/6 );
    D3DXMatrixRotationY( &matRotate2, fPhase );
    D3DXMatrixScaling(  &matScale, 0.01f, 0.01f, 0.01f );
    D3DXMatrixTranslation( &matTrans1, -5*sinf(fPhase), sinf(fKickFreq)/2, 10-10*cosf(fPhase) );

	D3DXMatrixIdentity(&matWorld);
    D3DXMatrixMultiply( &matWorld, &matTrans1, &matWorld);
	D3DXMatrixMultiply( &matWorld, &matScale, &matWorld);
	D3DXMatrixMultiply( &matWorld, &matRotate2, &matWorld);
	D3DXMatrixMultiply( &matWorld, &matRotate1, &matWorld);	
    
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	// Create a directional light
	D3DXVECTOR3 vLightToEye;
	D3DXVECTOR3 vLight(0.0f, -1.0f, 0.0f);
	D3DXVECTOR4 vLightEye;

	// Transform direction vector into eye space
	matView = m_pNVDevice->GetViewTransform();
	D3DXVec3Normalize(&vLightToEye, &vLight);
	D3DXVec3TransformNormal(&vLightToEye, &vLightToEye, &matView);
	D3DXVec3Normalize(&vLightToEye, &vLightToEye);

	// Shader math requires that the vector is to the light
	vLightEye.x = -vLightToEye.x;
	vLightEye.y = -vLightToEye.y;
	vLightEye.z = -vLightToEye.z;
	vLightEye.w = 1.0f;
	
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_DIRECTION, &vLightEye.x, 1);

	D3DXVECTOR4 vLightAmbient(0.2f, 0.2f, 0.2f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_AMBIENT, &vLightAmbient.x, 1);
		
	if (m_pDolphinMap)
		m_pD3DDev->SetTexture(0, m_pDolphinMap);
		
	DWORD tex = ((DWORD)(pTimer->GetDuration()*32))%32;

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	// Copy the alpha through from the vertex shader
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

	// Note that this premodulate is possible because the vertex shader has cunningly setup
	// the alpha channel to contain the intensity of the light.
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	if ((m_vecCausticTextures.size() > tex) && m_vecCausticTextures[tex])
		m_pD3DDev->SetTexture(1, m_vecCausticTextures[tex]);
	
	hr = m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumIndices/3);

	// Sea roof
	D3DXMatrixRotationZ(&matWorld, D3DX_PI);
	matWorld._42 += 10.0f;

	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	
	m_pD3DDev->SetTexture(1, m_vecCausticTextures[tex]);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	m_pFloorMesh->Render(m_pNVDevice);

	// Sea floor
	D3DXMatrixScaling( &matWorld, 1.0f, 1.0f, 1.0f);
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	m_pFloorMesh->Render(m_pNVDevice);
	
	return hr;
}

HRESULT CShaderMorph::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->MaxSimultaneousTextures >= 2))
	{
		m_strLastError = "Device cannot dual texture!";
		return E_FAIL;
	}

	if (!(pCaps->TextureOpCaps & D3DTEXOPCAPS_BLENDCURRENTALPHA))
	{
		m_strLastError = "Device cannot handle BLENDCURRENTALPHA operation!";
		return E_FAIL;
	}

	return S_OK;

}

void CShaderMorph::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n W - Wireframe Toggle \n\n P\\Pause - Pause animation\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_PAUSE:
		{
			m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		}
		break;

		default:
		break;
    }

}
