/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\Dot3Bump_Dynamic
File:  shader_Dot3_Directional.cpp

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

#include "shader_Dot3_Directional.h"
#include "Constants.h"
#include "SimpleObject8.h"

#include "CA_WaterWaves.h"

using namespace nv_objects;
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
			return new CShaderDot3Directional();
		default:
			return NULL;
	}
}

}


//////////////////////////////////////////////////////////

void CShaderDot3Directional::UpdateProperties()
{
	EBEffect::UpdateProperties();
	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Bump Map", (DWORD)DISPLAY_BUMPMAP, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Texture Space Light Vector", (DWORD)DISPLAY_LIGHTVECTOR, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Per-pixel Bumpmapping", (DWORD)DISPLAY_RESULT, EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);


	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause lights", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));

	//////////////////////////////////////////////////////////////////////////
	// Add vertex & pixel shader display to the tabbed text boxes:
	// Vertex shaders
	// m_pVertexShaderEnum & PixelShaderEnum are part of base effect class =)
	//
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Texture Space Calculation",
										GetFilePath("dot3_directional.nvv"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Neighbor Sampling Setup",
										GetFilePath("TexCoord_4_Offset.nvv"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant( new EBEnumValue(m_pPixelShaderEnum, "Force: Step 1",
										GetFilePath("NeighborForceCalc.nvp"), EBTYPE_STRING_PROP ));
	m_pPixelShaderEnum->AddEnumerant( new EBEnumValue(m_pPixelShaderEnum, "Force: Step 2",
										GetFilePath("NeighborForceCalc2_X2.nvp"), EBTYPE_STRING_PROP ));

	m_pPixelShaderEnum->AddEnumerant( new EBEnumValue(m_pPixelShaderEnum, "Apply Force",
										GetFilePath("ApplyForceShader.nvp"), EBTYPE_STRING_PROP ));
	m_pPixelShaderEnum->AddEnumerant( new EBEnumValue(m_pPixelShaderEnum, "Apply Velocity",
										GetFilePath("ApplyVelocityShader.nvp"), EBTYPE_STRING_PROP ));
	m_pPixelShaderEnum->AddEnumerant( new EBEnumValue(m_pPixelShaderEnum, "Smooth",
										GetFilePath("EqualWeightCombine_PostMult.nvp"), EBTYPE_STRING_PROP ));

	m_pPixelShaderEnum->AddEnumerant( new EBEnumValue(m_pPixelShaderEnum, "Gray to NormalMap",
										GetFilePath("NormalMapCreate2_Scale.nvp"), EBTYPE_STRING_PROP ));

    string aboutText = "file://";
    aboutText += GetFilePath("NVEffectsExplained.htm");
    aboutText += "#DynamicDot3BumpMapping";
	SetAboutInfo( NULL, _T("Dynamic Dot3 Bump Directional"), _T(aboutText.c_str()));
	SetAboutInfo( NULL, _T("Developer Relations"), _T("http://www.nvidia.com/developer"));
	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));
	SetAboutInfo( _T("Date"), _T("May 2001"));
}


CShaderDot3Directional::CShaderDot3Directional()
:	m_eDisplayOption(DISPLAY_RESULT),
	m_pSphereVertexBuffer(NULL),
	m_pSphereIndexBuffer(NULL),
	m_fAngle(0.0f),
	m_pLightMesh(NULL),
	m_pNVDevice(NULL),
	m_dwLightShader(0),
	m_dwDot3Shader(0),
	m_pUI(NULL),
	m_bWireframe(false),
	m_bPause(false)
{
	m_strEffectLocation = "Effects\\Bump Mapping";
	m_strEffectName = "Dynamic Dot3 Bump Directional";		// A string holding the name of the effect
	m_strEffectPixelShader = "";
	m_strEffectVertexShader = GetFilePath("dot3_directional.nvv");
	m_strEffectVersion = "1.0";
	
	m_pCA_Water = NULL;

}

CShaderDot3Directional::~CShaderDot3Directional()
{
	
	Free();	
}

HRESULT CShaderDot3Directional::GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius,
											   WORD wNumRings, WORD wNumSections,
											   FLOAT scale_x, FLOAT scale_y, FLOAT scale_z)
{


	SimpleObject8	* pObj = new SimpleObject8();
	assert( pObj != NULL );

	pObj->InitSphere( fRadius, wNumRings, wNumSections );


	////////////////////////////////////////////////
	// Make vertex buffer & index buffer from it

	HRESULT hr;
	D3DXVECTOR3 vPoint;
	DWORD i;

	SAFE_RELEASE(m_pSphereVertexBuffer);
	SAFE_RELEASE(m_pSphereIndexBuffer);

	assert( pObj->m_PrimType == D3DPT_TRIANGLELIST );


	DWORD	wNumTriangles	= pObj->m_wNumInd / 3;
    DWORD   dwNumIndices	= pObj->m_wNumInd;
    DWORD   dwNumVertices	= pObj->m_wNumVerts;

	m_dwNumVertices = dwNumVertices;
	m_dwNumIndices = wNumTriangles * 3;
	m_dwNumFaces = wNumTriangles;	


	hr = m_pD3DDev->CreateVertexBuffer(dwNumVertices * sizeof(Dot3Vertex), 0, 0, D3DPOOL_DEFAULT, &m_pSphereVertexBuffer);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	hr = m_pD3DDev->CreateIndexBuffer(3 * wNumTriangles * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pSphereIndexBuffer);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	Dot3Vertex* pVertices;
	WORD* pIndices;
	
	m_pSphereVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	m_pSphereIndexBuffer->Lock(0, 0, (BYTE**)&pIndices, 0);

	
	float x,y,z, nx, ny, nz, u, v;

	// copy positions into VB
    for( i = 0; i < dwNumVertices; i++ )
    {
		x = scale_x * pObj->m_pVertices[i].pos.x	+ vCenter.x;
		y = scale_y * pObj->m_pVertices[i].pos.y	+ vCenter.y;
		z = scale_z * pObj->m_pVertices[i].pos.z	+ vCenter.z;

		nx = pObj->m_pVertices[i].nrm.x;
		ny = pObj->m_pVertices[i].nrm.y;
		nz = pObj->m_pVertices[i].nrm.z;

		u = pObj->m_pVertices[i].t0.x * 2.0f;
		v = pObj->m_pVertices[i].t0.y * 2.0f + 0.5f;

		pVertices[i] = Dot3Vertex(	D3DXVECTOR3( x, y, z ),
									D3DXVECTOR3( nx, ny, nz ),
									D3DXVECTOR2( u, v ) );
	}



    // Generate triangle index buffer from simple object's indices

    for( i = 0; i < dwNumIndices; i++ )
    {
        pIndices[i] = pObj->m_pIndices[i];
	}


	// All done - unlock the buffers:

	hr = m_pSphereVertexBuffer->Unlock();
	ASSERT_IF_FAILED(hr);

	hr = m_pSphereIndexBuffer->Unlock();
	ASSERT_IF_FAILED(hr);


	// Free the SimpleObject

	delete( pObj );
	pObj = NULL;


	// Compute the tangent space

	hr = CreateBasisVectors( m_pSphereVertexBuffer, m_pSphereIndexBuffer);
	ASSERT_IF_FAILED(hr);


    return S_OK;
}


HRESULT CShaderDot3Directional::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	vector<DWORD> Declaration;

	m_pD3DDev = pDev;
	pDev->AddRef();

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);

	m_pNVDevice = new NVDot3DirectionalDevice(pDev, this);


	///////////////


	m_pCA_Water = new CA_Water();
	assert( m_pCA_Water != NULL );

	m_pCA_Water->Initialize( pDev );

	m_pCA_Water->SetNormalMapSTScale( 0.3f );

	m_pCA_Water->SetBlurDistance( 0.50f );
	m_pCA_Water->SetEqRestoreFac( 0.05f );
	m_pCA_Water->SetVelocityApplyFac( 0.4998f );
	m_pCA_Water->SetBlendFac( 0.2499f );


	///////////////


	m_pLightMesh = new NVMesh();
	hr = m_pLightMesh->Create(m_pNVDevice, GetFilePath("arrow.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load arrow.x";
		return hr;
	}
	m_pLightMesh->SetFVF(m_pNVDevice, D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX1);


	hr = GenerateSphere(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1.2f, 20, 20, 1.0f, 1.0f, 1.0f);
//	hr = GenerateSphere(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1.3f, 2, 4, 1.0f, 1.0f, 1.0f);
	
	if (FAILED(hr))
	{
		m_strLastError = "Could not generate sphere";
		return hr;
	}

	// Create the shaders
	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3)); // Pos
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3)); // Normal
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2)); // Texture
	Declaration.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("dot3_lightobject.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwLightShader);
	if (FAILED(hr))
		return hr;

	m_pLightMesh->SetVertexShader(m_dwLightShader);

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3)); // Position
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3)); // Normal
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT3)); // Diffuse
	Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT2)); // Texture
	Declaration.push_back(D3DVSD_REG(5, D3DVSDT_FLOAT3)); // S
	Declaration.push_back(D3DVSD_REG(6, D3DVSDT_FLOAT3)); // T
	Declaration.push_back(D3DVSD_REG(4, D3DVSDT_FLOAT3)); // SxT
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("dot3_directional.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwDot3Shader);
	if (FAILED(hr))
		return hr;


	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_HALF, D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.5f), 1);

	// Camera stuff
	D3DXMATRIX matProj;
	D3DXMATRIX matView;

	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, -3.0f, 0 );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);


	// update mouseUI's orientation
	m_pUI->SetViewMode( MouseUI::VM_CAMERA_AWARE );
	m_pUI->SetControlOrientationMatrix( matView );

	m_pUI->SetTranslationalSensitivityFactor( 0.25f );


	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 0.1f, 50.0f);

	m_pNVDevice->SetViewTransform(&matView);
	m_pNVDevice->SetProjectionTransform(&matProj);

	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);


	if (m_pLightMesh)
		m_pLightMesh->RestoreDeviceObjects(m_pNVDevice);
	
	return S_OK;

}

HRESULT CShaderDot3Directional::Free()
{
	SAFE_DELETE(m_pLightMesh);
	SAFE_RELEASE(m_pSphereVertexBuffer);
	SAFE_RELEASE(m_pSphereIndexBuffer);
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(m_pUI);

	SAFE_DELETE( m_pCA_Water );

	if (m_pD3DDev)
	{
		if (m_dwDot3Shader)
			m_pD3DDev->DeleteVertexShader(m_dwDot3Shader);
		if (m_dwLightShader)
			m_pD3DDev->DeleteVertexShader(m_dwLightShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderDot3Directional::Start()
{
	m_fAngle = 0.0f;
	return S_OK;
}

  
HRESULT CShaderDot3Directional::SetTransform()
{
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;
	D3DXMATRIX matWorldInverse;

	D3DXMATRIX matWorld = m_pNVDevice->GetWorldTransform();
	D3DXMATRIX matProj = m_pNVDevice->GetProjectionTransform();
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();

	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixInverse(&matWorldInverse, NULL, &matWorld);
	
	// Create a 3x3 invertse of the worldview for the normal transformation (we transpose it as we load it into
	// the constant store)
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
		
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Transform to eye space
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	// Transform to world space
	D3DXMatrixTranspose(&matWorld, &matWorld);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &matWorld(0, 0), 4);
	D3DXMatrixTranspose(&matWorld, &matWorld);

	// Transform for normals
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_1, &matWorldViewIT(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_2, &matWorldViewIT(2, 0), 1);

	return S_OK;

}

// Draw the actual light so we can see where it's pointing.
HRESULT CShaderDot3Directional::DrawDirectionalLightObject(D3DXVECTOR3 vecLightDirection)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matTemp;
	D3DXVECTOR3 vecNormalizedDirection;

	const NVBounds* pBounds = m_pLightMesh->GetBounds();
	D3DXMatrixIdentity(&matWorld);

	// Translate to the middle
	D3DXMatrixTranslation(&matTemp, -pBounds->m_vecCenter.x, -pBounds->m_vecCenter.y, -pBounds->m_vecCenter.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

	float fLightScale = 0.1f; 
	D3DXMatrixScaling(&matTemp, fLightScale / pBounds->m_fRadius, fLightScale / pBounds->m_fRadius, fLightScale / pBounds->m_fRadius);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

	// Orientation of the light object (the arrow)
	D3DXVECTOR3 vLightObject = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	D3DXVec3Normalize(&vecNormalizedDirection, &vecLightDirection);

	// Create the rotation axis
	D3DXVECTOR3 vOrthoNormal;
	D3DXVec3Cross(&vOrthoNormal, &vecNormalizedDirection, &vLightObject);
	D3DXVec3Normalize(&vOrthoNormal, &vOrthoNormal);

	// Calculate the angle between the two vectors.
	float fAngle = acos(D3DXVec3Dot(&vecNormalizedDirection, &vLightObject));

	// Rotate the object about our rotation axis to map one vector onto another
	D3DXMatrixRotationAxis(&matTemp, &vOrthoNormal, -fAngle);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

	D3DXMATRIX matTranslate = m_pUI->GetTranslationMatrix();
	// vecLightDirection is a direction vector, pointing at the origin, so we negate it to find the position
	D3DXMatrixTranslation(&matTemp, -vecLightDirection.x + matTranslate._41, -vecLightDirection.y + matTranslate._42, -vecLightDirection.z + matTranslate._43);

	D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

	D3DXVECTOR3 LightColor(1.0f, 1.0f, 1.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_COLOR, &LightColor.x, 1);

	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	m_pLightMesh->Render(m_pNVDevice);		

	return S_OK;
}

HRESULT CShaderDot3Directional::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	
	//////////////////////

	assert( m_pCA_Water != NULL );

	m_pCA_Water->Tick();

	//////////////////////

	// Point at the vertex data
	hr = m_pD3DDev->SetStreamSource(0, m_pSphereVertexBuffer, sizeof(Dot3Vertex));
	if (FAILED(hr))
	{
		m_strLastError = "Could not set VB source";
		return hr;
	}
	
	// Point at the index data
	hr = m_pD3DDev->SetIndices(m_pSphereIndexBuffer, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not set Index source";
		return hr;
	}

	//////////////////////


	D3DXMATRIX matWorld;
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();
	D3DXMATRIX matTemp;

	// Clear to grey
	//	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	// Increase rotation
	
	if (!m_bPause)
	{
		m_fAngle = timeGetTime() * ((float)0.01f);
	}


	D3DXMatrixMultiply( &matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );


	// Load transform data.
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	// Create the light direction vector.
	D3DXVECTOR3 vecLightDirection;
	float fLightDistance = 1.1f;
	vecLightDirection.x = fLightDistance * cos( D3DXToRadian(m_fAngle));
	vecLightDirection.z = fLightDistance * sin( D3DXToRadian(m_fAngle));
	vecLightDirection.y = fLightDistance;
		
	// Do the rendering
	DrawDirectional(vecLightDirection);

	// Draw the light shape
	DrawDirectionalLightObject(vecLightDirection);



	return hr;
}
	
HRESULT CShaderDot3Directional::DrawDirectional(D3DXVECTOR3 vecLightDirection)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matTemp;

	D3DXVec3Normalize(&vecLightDirection, &vecLightDirection);

	// shader math requires that the light direction vector points towards the light.
	vecLightDirection = -vecLightDirection;


	// Setup the vertex shader
	m_pD3DDev->SetVertexShader(m_dwDot3Shader);

	m_pD3DDev->SetPixelShader(0);


	// Normal map in stage 0
	// DotProduct 3 the normal map with the diffuse color set in the vertex
	// shader (this will be the local light vector)


	m_pD3DDev->SetTexture(0, m_pCA_Water->GetOutputTexture() );

	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	m_pD3DDev->SetRenderState( D3DRS_WRAP0, 0 );

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	
	// If you want a base texture, put it in stage 1 and MODULATE:
	// Modulate the bump output from the previous stage by the current texture
	//	m_pD3DDev->SetTexture(1, m_pBaseTexture );
	//	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1  );
	//	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU,	D3DTADDRESS_WRAP);
	//	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV,	D3DTADDRESS_WRAP);

	//	m_pD3DDev->SetRenderState( D3DRS_WRAP1, 0 );

	//	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	//	MODULATE);

	// Override the setup options if the user wants to display the components
	switch(m_eDisplayOption)
	{
		default:
		case DISPLAY_RESULT:
			break;
		case DISPLAY_BUMPMAP:
			// Show the bump texture, disable the base texture pass
			m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			break;
		case DISPLAY_LIGHTVECTOR:
			// Show the diffuse color, output from the vshader which is the texture-space
			// light vector
			m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			break;
		case DISPLAY_BASETEXTURE:
			break;
	}

	// Not using stage 2
	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	// Load the location of the light into the vertex shader.
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, (void*)&vecLightDirection.x, 1);
	
	// Point at the vertex data
	m_pD3DDev->SetStreamSource(0, m_pSphereVertexBuffer, sizeof(Dot3Vertex));
	
	// Point at the index data
	m_pD3DDev->SetIndices(m_pSphereIndexBuffer, 0);
	
	// Backface cull the sphere
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Draw the sphere
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumFaces);


	return S_OK;
}


HRESULT CShaderDot3Directional::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->MaxSimultaneousTextures >= 2))
	{
		m_strLastError = "Device cannot dual texture!";
		return E_FAIL;
	}

	if (!(pCaps->TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3))
	{
		m_strLastError = "Device cannot handle dotproduct3 operation!";
		return E_FAIL;
	}


	return S_OK;
}

void CShaderDot3Directional::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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
	return;
}

void CShaderDot3Directional::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderDot3Directional::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
	/////////////////

	bool res;
	res = m_pCA_Water->Keyboard( dwKey, nFlags, bDown );

	if( res == true )
		return;

	/////////////////

	EBString msg;

    switch ( Action )
    {
		case EB_HELP:
		{

			msg  = "H  or  F1 for Help\n";
			msg += "\n";
			msg += "*** This demo is framerate limited.  ***\n";
			msg += "    Hit 'S' to run as fast as possible!\n";
			msg += "\n";
			msg += "L       : Toggle logo in water\n";
			msg += "\n";
			msg += "NUMPAD7   : Reset to initial conditions\n";
			msg += "\n";
			msg += "1       :   Display normal map\n";	
			msg += "2       :   Display velocity map\n";	
			msg += "3       :   Display prev pos,pos,v,n maps\n";	
			msg += "\n";	
			msg += "W       : Wireframe\n";
			msg += "\n";
			msg += "G          : Start/stop procedural animation       \n";
			msg += "S          : Toggle animation rate limiting\n";
			msg += "  L arrow : Run animation faster\n";
			msg += "  R arrow : Run animation slower\n";
			msg += "\n";
			msg += "B       : Toggle texture border wrapping\n";
			msg += "\n";
			msg += "Up arrow   : Increase droplet frequency\n";	
			msg += "Dwn arrow  : Decrease droplet frequency\n";	
			msg += "\n";
			msg += "Y       : Decrease normal map red/green\n";
			msg += "U       : Increase normal map red/green\n";
			msg += "\n";
			msg += "C       : Decrease height smoothing\n";	
			msg += "V       : Increase height smoothing\n";	
			msg += "<, >    : Dec/Inc velocity apply factor\n";
			msg += "[, ]    : Dec/Inc force apply factor\n";
			msg += "Z,X     : Dec/Inc equilibrium restore force\n";
			msg += "\n";
			msg += "\n";
			
			
			MessageBoxEx( NULL, msg.c_str(), "Dynamic Reflection Map Help",
						MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );

		}
		break;

		case EB_PAUSE:
		{
			m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
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
            m_pUI->Reset();
			m_pUI->SetTranslationalSensitivityFactor( 0.25f );

            m_bWireframe = false;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

		case EB_NORMALMAP:
		{
			if (m_eDisplayOption != DISPLAY_BUMPMAP)
				m_eDisplayOption = DISPLAY_BUMPMAP;
			else
				m_eDisplayOption = DISPLAY_RESULT;

			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		}
		break;

        default :
            break;
    }
}