/*********************************************************************NVMH2****
Path:  C:\development\CubemapLighting\Code
File:  CubeMapLighting.cpp

Copyright (C) 2002, Kenneth Hurley

Comments:
	Kenneth Hurley - Started on 1/2/2002

******************************************************************************/


#include "eb_effect.h"
#include "nvinc.h"
#include "CubeMapLighting.h"
#include "D3dx8core.h"
#include <vector>

// constant memory definitions for the vertex/pixel shader
#include "Constants.h"

#define VK_OEM_COMMA	0xbc
#define VK_OEM_PERIOD	0xbe

#define GRASS_HEIGHT (200.0f)
#define TREE_HEIGHT (2.2f)
#define DIRT_HEIGHT (2.85f)

char *LoadObjects[NUM_OBJECTS] = { "ccterrain.x",
								   "skysphere.x"
								};

using namespace std;

bool gSyncToFrameRate = FALSE;
float gRadius = 30.4f;

DECLARE_EFFECT_MAIN()


char *strSingleStep = "Single Stepping Mode";

extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CCubeMapLighting();
			break;
		default:
			return NULL;
		}
	}
	
}

// ------------------------------------------------------------------------------
// CCubeMapLighting Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CCubeMapLighting::CCubeMapLighting()
{
	DWORD i;
	m_strEffectLocation = "CubeMapping\\CubeMapLighting";
	m_strEffectName = "CubeMapLighting";
	m_strEffectVertexShader = GetFilePath("dirLight.nvv");
	m_strEffectVersion = "1.0";
	m_strEffectPixelShader = GetFilePath("Terrain.nvp");
	
	D3DXMatrixIdentity(&m_MatTerrain);
	D3DXMatrixIdentity(&m_MatSphereRot);
	D3DXMatrixIdentity(&m_MatView);
	D3DXMatrixIdentity(&m_MatProj);

	m_Stepping = false;

	for (i=0; i< NUM_OBJECTS; i++)
	{
		m_pVertexBuffer[i] = NULL;
		m_pIndexBuffer[i] = NULL;
		m_pAttributes[i] = NULL;
	}

	m_dwDirLightVShader = NULL;
	m_dwSphereVShader = NULL;
	m_dwSpherePShader = NULL;
	m_dwTerrainPShader = NULL;
	m_pDecalTexture = NULL;
	m_pCloudTexture = NULL;
	m_bWireframe = false;
	m_mUI = NULL;
}

// ------------------------------------------------------------------------------
// CCubeMapLighting Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CCubeMapLighting::~CCubeMapLighting()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CCubeMapLighting Resize
//
// Description: This is a stupid hack, because the effects browser doesn't allow
//				us to get at the windows handle
// ------------------------------------------------------------------------------ 
void CCubeMapLighting::Resize(HWND hWnd, int w, int h, bool)
{
	EBEffect::Resize(hWnd, w, h, true);

}


HRESULT CCubeMapLighting::LoadXFile(const char* fileName, const DWORD dwFVF, DWORD index)
{
	ID3DXMesh *tempMesh, *tempMeshFVF, *tempMeshOpt;
    LPBYTE pSrc;
	
	HRESULT hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_SYSTEMMEM, m_pD3DDev, NULL, 
		(ID3DXBuffer**)&m_pMaterials[index], &m_dwNumSections[index], &tempMesh);

	LPDIRECT3DVERTEXBUFFER8 tempVB = NULL;
	tempMesh->GetVertexBuffer(&tempVB);
	tempVB->Lock( 0, 0, (BYTE**)&pSrc, 0 );
	// grab bound sphere, so we may initially scale our view.
	D3DXComputeBoundingSphere(pSrc, tempMesh->GetNumVertices(), tempMesh->GetFVF(), 
		&m_ObjectCenter[index], &m_ObjectRadius[index]);
	tempVB->Unlock();
	SAFE_RELEASE(tempVB);
	
	tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);

	DWORD attribSize = m_dwNumSections[index];
	hr = tempMeshOpt->GetAttributeTable(NULL, &attribSize);
	
	m_pAttributes[index] = new D3DXATTRIBUTERANGE[attribSize];
	
	// convert to our format, we have to do this last since the previous steps require reading from the 
	// IB or VB, and we're converting to writeonly
	CHECKHRESULT(tempMeshOpt->CloneMeshFVF(D3DXMESH_WRITEONLY, dwFVF, m_pD3DDev, &tempMeshFVF));
	
	tempMeshFVF->GetVertexBuffer(&m_pVertexBuffer[index]);
	tempMeshFVF->GetIndexBuffer(&m_pIndexBuffer[index]);
	CHECKHRESULT(tempMeshFVF->GetAttributeTable(m_pAttributes[index], &attribSize));

	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshFVF);
	SAFE_RELEASE(tempMeshOpt);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CCubeMapLighting::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CCubeMapLighting::UpdateProperties()
{
	EBEffect::UpdateProperties();
	EBProperty* pBoolProp;

	pBoolProp = new EBProperty(strSingleStep, OBJECT_MEMBER(m_Stepping), EBTYPE_BOOL_PROP, PROPFLAG_CALLBACK);
	AddProperty(pBoolProp);

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Terrain Rendering", 
							GetFilePath("dirLight.nvv"), EBTYPE_STRING_PROP));

	SetAboutInfo( NULL, _T("CubeMapLighting Demo"), _T(""));
	SetAboutInfo( NULL, _T("Kenneth Hurley"), _T(""));
	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));

}

// ------------------------------------------------------------------------------
// CCubeMapLighting::PropertyUpdateCallback
//
// Description: Our properties were updated, set the correct shader to match
// ------------------------------------------------------------------------------ 
void CCubeMapLighting::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
    // if m_bShowShadow or mUseAlternateQuad changed we need to regenerate the volume
	if (pProperty->IsKindOf(EBTYPE_BOOL_PROP))
	{
		if (bWritten) 
        {
			if (!pProperty->GetPropertyName().compare(strSingleStep))
			{
			}
        }
	}
}
// ------------------------------------------------------------------------------
// CCubeMapLighting::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CCubeMapLighting::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_MIPMAP))
	{
		m_strLastError = "Device does not support mipmaps!";
		return E_FAIL;
	}

	if(pCaps->MaxSimultaneousTextures < 2)
	{
		m_strLastError = "Device does not support two simultaneous textures!";
		return E_FAIL;
	}

	if(D3DSHADER_VERSION_MAJOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support vertex shaders!";
		return E_FAIL;
	}

	return S_OK;
}

// ------------------------------------------------------------------------------
// CCubeMapLighting::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CCubeMapLighting::Initialize(IDirect3DDevice8* pDev)
{
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
	m_mUI = new MouseUI((const RECT)rect, true);
    m_mUI->SetRotationalSensitivityFactor(1.0f);
	
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE,       D3DCULL_CCW ); //D3DCULL_none ); 

	////////////////////////////////////////////////////////
	// Load vertex shader for two-sided lighting

	vector<DWORD> Declaration;

	// Microsoft's D3DVSD_REG is not working properly, so use
	//  this STL vector method instead

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3  )); // Pos
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3  )); // Normal
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2  )); // Texture
	Declaration.push_back(D3DVSD_END());

	CHECKHRESULT(LoadAndCreateShader(GetFilePath("dirLight.vso"), &Declaration[0], 
			0, SHADERTYPE_VERTEX, &m_dwDirLightVShader));

	CHECKHRESULT(LoadAndCreateShader(GetFilePath("Terrain.pso"), NULL, 0, 
		SHADERTYPE_PIXEL, &m_dwTerrainPShader));
	////////////////////////////////////////////////////////

	CHECKHRESULT(LoadAndCreateShader(GetFilePath("skysphere.vso"), &Declaration[0], 
			0, SHADERTYPE_VERTEX, &m_dwSphereVShader));

	CHECKHRESULT(LoadAndCreateShader(GetFilePath("skysphere.pso"), NULL, 0, 
		SHADERTYPE_PIXEL, &m_dwSpherePShader));

	CHECKHRESULT(D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("ccTerrainmap.tga").c_str(), 
		&m_pDecalTexture));

	CHECKHRESULT(D3DXCreateCubeTextureFromFile(m_pD3DDev, GetFilePath("cloudtex.dds").c_str(), 
		&m_pCloudTexture));

	for (i=0; i< NUM_OBJECTS; i++)
	{
		CHECKHRESULT(LoadXFile(GetFilePath(LoadObjects[i]).c_str(), TSVERTEX_FVF, i));
	}

	m_MatTerrain._42 = m_ObjectRadius[SPHERE_OBJECT] * 9.0f/10.0f;
	SetupFixedCamera(0.0f, m_MatTerrain._42 + 20.0f, m_ObjectRadius[TERRAIN_OBJECT]);
	return S_OK;
}

void CCubeMapLighting::SetupFixedCamera(float x, float y, float radius)
{
	D3DXMATRIX temp;
	m_vEyePt = D3DXVECTOR3(x, y, -radius+100.0f);
	m_vLookatPt = D3DXVECTOR3( x, y-10.0f, 0.0f);
	m_vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);

    D3DXMatrixLookAtLH(&m_MatView, &m_vEyePt, &m_vLookatPt, &m_vUp);
    FLOAT fAspect = m_D3DViewPort.Width / (FLOAT)m_D3DViewPort.Height;
    D3DXMatrixPerspectiveFovLH( &m_MatProj, D3DX_PI/3, fAspect, 1.0f,
                                radius*200);
}

HRESULT CCubeMapLighting::SetVertexShaderGlobalConstants(D3DXMATRIX &world)
{
    // set up vertex shading constants to contain proper
    // transformation matrices etc.
    D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldI;
	D3DXMATRIX matViewUI;
	D3DXMATRIX tempMat;

	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &world(0, 0), 4);

	D3DXMatrixInverse(&matWorldI, NULL, &world);

	D3DXMatrixMultiply(&tempMat, &m_mUI->GetRotationMatrix(), &m_mUI->GetTranslationMatrix());
	D3DXMatrixMultiply(&matViewUI, &m_MatView, &tempMat);
	D3DXMatrixMultiply(&matWorldView,     &world,     &matViewUI);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &m_MatProj);
	
	// Write Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

	D3DXVECTOR4 vecLight(0.0f, 1.0f, 0.0f, 0.0f);
	// put light into object space
	D3DXVec4Transform(&vecLight, &vecLight, &matWorldI);
	D3DXVec4Normalize(&vecLight, &vecLight);

	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &vecLight, 1);

	// made this a vector 4 so, we wouldn't have to set the w part in the shader
	D3DXVECTOR4 eyePos;
	// put eye into object space.
	D3DXVec3TransformCoord((D3DXVECTOR3*)&eyePos, &m_vEyePt, &matWorldI);
	eyePos.w = 1.0f;
	m_pD3DDev->SetVertexShaderConstant(CV_EYE_POSITION, &eyePos, 1);

	D3DXVECTOR4 convenience(0.0f, .5f, 1.0f, 2.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_CONVENIENCE, &convenience, 1);

	return S_OK;
}

// ------------------------------------------------------------------------------
// CCubeMapLighting::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CCubeMapLighting::Free()
{
	DWORD i;
	SAFE_DELETE(m_mUI);
	SAFE_RELEASE(m_pDecalTexture);
	SAFE_RELEASE(m_pCloudTexture);

	for (i=0; i<NUM_OBJECTS; i++)
	{
		SAFE_RELEASE(m_pVertexBuffer[i]);
		SAFE_RELEASE(m_pIndexBuffer[i]);
		SAFE_DELETE_ARRAY(m_pAttributes[i]);
	}

	if (m_pD3DDev != NULL)
	{
		if (m_dwDirLightVShader != NULL)
		{
			m_pD3DDev->DeleteVertexShader(m_dwDirLightVShader);
			m_dwDirLightVShader = NULL;
		}

		if (m_dwTerrainPShader != NULL)
		{
			m_pD3DDev->DeletePixelShader(m_dwTerrainPShader);
			m_dwTerrainPShader = NULL;
		}

		if (m_dwSphereVShader != NULL)
		{
			m_pD3DDev->DeleteVertexShader(m_dwSphereVShader);
			m_dwSphereVShader = NULL;
		}

		if (m_dwSpherePShader != NULL)
		{
			m_pD3DDev->DeletePixelShader(m_dwSpherePShader);
			m_dwSpherePShader = NULL;
		}
	}


	SAFE_RELEASE(m_pD3DDev);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CCubeMapLighting::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CCubeMapLighting::Start()
{
	return S_OK;
}

HRESULT CCubeMapLighting::RenderTerrain(EBTimer *pTimer)
{
	D3DXMATRIX tempMat;
	DWORD i;

	m_pD3DDev->SetTexture(0,(LPDIRECT3DBASETEXTURE8)m_pDecalTexture);
	m_pD3DDev->SetTexture(1,(LPDIRECT3DBASETEXTURE8)m_pCloudTexture);

	m_pD3DDev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,0);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU | 0 );

	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	m_pD3DDev->SetRenderState(D3DRS_LOCALVIEWER, TRUE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);   

	// set scale for terrain in unitized sphere object
	D3DXVECTOR4 invScale;
	invScale.x = invScale.y = invScale.z = (m_ObjectRadius[SPHERE_OBJECT] /
		m_ObjectRadius[TERRAIN_OBJECT]) * 100.0f;
	invScale.w = 1.0f;
	m_pD3DDev->SetVertexShaderConstant(CV_SCALE_POS, &invScale, 1);
	
	// set rotation of sky sphere
	m_pD3DDev->SetVertexShaderConstant(CV_SKYSPHERE_ROT_0, &m_MatSphereRot(0, 0), 3);

	CHECKHRESULT(m_pD3DDev->SetPixelShader(m_dwTerrainPShader));
	CHECKHRESULT(m_pD3DDev->SetVertexShader(m_dwDirLightVShader));

	SetVertexShaderGlobalConstants(m_MatTerrain);
	// use this vertex buffer as our source for vertices
	CHECKHRESULT(m_pD3DDev->SetStreamSource(0, m_pVertexBuffer[TERRAIN_OBJECT], sizeof(TSVertex)));
	//set index buffer
	CHECKHRESULT(m_pD3DDev->SetIndices(m_pIndexBuffer[TERRAIN_OBJECT], 0));

	for(i = 0; i < m_dwNumSections[TERRAIN_OBJECT]; i++)
	{
		CHECKHRESULT(m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_pAttributes[TERRAIN_OBJECT][i].VertexStart,
			m_pAttributes[TERRAIN_OBJECT][i].VertexCount, m_pAttributes[TERRAIN_OBJECT][i].FaceStart*3, 
			m_pAttributes[TERRAIN_OBJECT][i].FaceCount));
	}

	return D3D_OK;
}


HRESULT CCubeMapLighting::RenderSkySphere(EBTimer *pTimer)
{
	D3DXMATRIX tempMat;
	DWORD i;

	m_pD3DDev->SetTexture(0,(LPDIRECT3DBASETEXTURE8)m_pCloudTexture);

	m_pD3DDev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,0);
	m_pD3DDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU | 0 );

	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	m_pD3DDev->SetRenderState(D3DRS_LOCALVIEWER, TRUE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);

	D3DXVECTOR4 skyColor(74.0f/255.0f, 78.0f/255.0f, 250.0f/255.0f, 0.0f);
	m_pD3DDev->SetPixelShaderConstant(CP_SKY_COLOR, &skyColor, 1);

	// we're inside the sphere so reverse culling
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);   

	CHECKHRESULT(m_pD3DDev->SetVertexShader(m_dwSphereVShader));
	CHECKHRESULT(m_pD3DDev->SetPixelShader(m_dwSpherePShader));

	float rotAngle = D3DXToRadian((pTimer->GetDuration() * 10.0f));
	D3DXVECTOR3 rotVector(0.7f, 0.0f, 0.7f);
	// rotation about abritrary axis
	D3DXMatrixRotationAxis(&m_MatSphereRot, &rotVector, rotAngle);

	SetVertexShaderGlobalConstants(m_MatSphereRot);
	// use this vertex buffer as our source for vertices
	CHECKHRESULT(m_pD3DDev->SetStreamSource(0, m_pVertexBuffer[SPHERE_OBJECT], sizeof(TSVertex)));
	//set index buffer
	CHECKHRESULT(m_pD3DDev->SetIndices(m_pIndexBuffer[SPHERE_OBJECT], 0));

	for(i = 0; i < m_dwNumSections[SPHERE_OBJECT]; i++)
	{
		CHECKHRESULT(m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_pAttributes[SPHERE_OBJECT][i].VertexStart,
			m_pAttributes[SPHERE_OBJECT][i].VertexCount, m_pAttributes[SPHERE_OBJECT][i].FaceStart*3, 
			m_pAttributes[SPHERE_OBJECT][i].FaceCount));
	}

	return D3D_OK;
}

// ------------------------------------------------------------------------------
// CCubeMapLighting::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CCubeMapLighting::Tick(EBTimer* pTimer)
{
	HRESULT hr;

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00004080, 1.0f, 0L);

	hr = RenderTerrain(pTimer);
	if (hr != D3D_OK)
		return hr;

	return RenderSkySphere(pTimer);
}


void CCubeMapLighting::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if (button == MOUSE_LEFTBUTTON)
	{
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

void CCubeMapLighting::MouseMove(HWND hWnd, int x, int y)
{
	m_mUI->OnMouseMove(x, y);
	return;
}

void CCubeMapLighting::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{

	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);

	switch (Action)
	{

	case EB_HELP:
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n"
								  " Home - Reset To Defaults \n\n"
								  " 'W' - Wireframe Toggle \n\n"
								  " '+' - Zoom in\n\n"
								  " '-' - Zoom out\n\n"
								  " Space - Toggle step mode\n\n"
								  " '>' - Step forward\n\n"
								  " '<' - Step backward\n\n"
								  " 'S' - Toggle sync to rendering framerate\t\n\n",
								  "Help", 
					MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
        break;
	
	case EB_WIREFRAME:
		m_bWireframe = !m_bWireframe;
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		break;

	case EB_RESET:
		m_mUI->Reset();
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		break;

	case EB_ADD:
        m_mUI->Translate( 0.0f, 0.0f, .10f );
        break;

	case EB_SUBTRACT:
		m_mUI->Translate( 0.0f, 0.0f, -.10f );
		break;

	case EB_PAUSE:
		m_Stepping = !m_Stepping;
		break;
	

	default:
		if ( bDown )
		{
			switch ( dwKey )
			  {
				case VK_LEFT :
				case VK_NUMPAD4 :
				{
					m_mUI->Translate( -1.0f, 0.0f, 0.0f );
					break;
				}

				case VK_RIGHT :
				case VK_NUMPAD6 :
				{
					m_mUI->Translate(  1.0f, 0.0f, 0.0f );
					break;
				}

				case VK_UP :
				case VK_NUMPAD8 :
				{
					m_mUI->Translate(  0.0f, 0.0f, -1.0f );
					break;
				}

				case VK_DOWN :
				case VK_NUMPAD2 :
				{
					m_mUI->Translate( 0.0f, 0.0f, 1.0f );
					break;
				}

				case VK_PRIOR :
				case VK_NUMPAD9 :
				{
					m_mUI->Translate(  0.0f, 1.0f, 0.0f );
					break;
				}
				case VK_NEXT :
				case VK_NUMPAD3 :
				{
					m_mUI->Translate(  0.0f, -1.0f, 0.0f );
					break;
				}

				case 'S':
					gSyncToFrameRate = !gSyncToFrameRate;
					break;

				default :
				{
					break;
				}
			}
		}
	}
	return;
}
