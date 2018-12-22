/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\ToonShade
File:  shader_ToonShade.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "eb_effect.h"
#include "nvinc.h"
#include "shader_ToonShade.h"
#include "ComPtr.h"
#include "Constants.h"
#include "NVFileDialog.h"

using namespace std;

#define STR_LOADMESH "Load .X File Mesh"

DECLARE_EFFECT_MAIN()

extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CShaderToonShade();
			break;
		default:
			return NULL;
		}
	}
	
}

// ------------------------------------------------------------------------------
// CShaderToonShade Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderToonShade::CShaderToonShade()
: m_pVertexBuffer(NULL), 
  m_pIndexBuffer(NULL),
  m_pShadeTexture(NULL), 
  m_pEdgeTexture(NULL),
  m_pAttributes(NULL),
  m_pTFactorColors(NULL),
  m_bWireframe(false),
  m_pUI(NULL),
  m_bUseTrilinear(true),
  m_eDisplayOption(TOONDISPLAY_SILHOUETTESANDSHADING),
  m_fRadius(0.0f),
  m_vecCenter(0.0f, 0.0f, 0.0f)
{
	m_strEffectLocation = "Vertex Shaders\\Render Styles";
	m_strEffectName = "Toon Shading";
	m_strEffectVertexShader = GetFilePath("ToonShade.nvv");
	m_strEffectPixelShader = "";
	
	D3DXMatrixIdentity(&m_world);
	D3DXMatrixIdentity(&m_view);
	D3DXMatrixIdentity(&m_proj);
}

// ------------------------------------------------------------------------------
// CShaderToonShade Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderToonShade::~CShaderToonShade()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderToonShade::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CShaderToonShade::UpdateProperties()
{
	EBEffect::UpdateProperties();
	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Silhouettes only", (DWORD)TOONDISPLAY_SILHOUETTES, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Shading only", (DWORD)TOONDISPLAY_SHADING, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Silhouettes and Shading", (DWORD)TOONDISPLAY_SILHOUETTESANDSHADING, EBTYPE_DWORD_PROP));
	
	AddProperty(pEnumProp);
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBTriggerProperty(STR_LOADMESH));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Toon Lighting", GetFilePath("ToonShade.nvv"), EBTYPE_STRING_PROP));

}


// ------------------------------------------------------------------------------
// CShaderToonShade::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderToonShade::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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

	return S_OK;
}

HRESULT CShaderToonShade::SetBuffers()
{
	HRESULT hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(TSVertex));
	if (FAILED(hr))
		return hr;
	
	//set index buffer
	hr = m_pD3DDev->SetIndices(m_pIndexBuffer, 0);
	if(FAILED(hr))
		return hr;

	return S_OK;
}
	
// ------------------------------------------------------------------------------
// CShaderToonShade::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderToonShade::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	
	// get the D3D device 
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
	
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT1));
	Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT1));
	Declaration.push_back(D3DVSD_END());
	
	// Load the vertex shader object file and create it.
	m_dwCurrentShader = 0;
	hr = LoadAndCreateShader(GetFilePath("ToonShade.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	// select the just created vertex shader as the thing to transform and light vertices
	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	DWORD tempFVF2;
	D3DXFVFFromDeclarator(&Declaration[0], &tempFVF2);
	
	DWORD tempFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE1(0) | D3DFVF_TEXCOORDSIZE1(1);
	hr = LoadXFile(GetFilePath("bigship1.x").c_str(), tempFVF);
	if(FAILED(hr))
		return hr;
	
	hr = SetBuffers();
	if(FAILED(hr))
		return hr;

	//set render states
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	//load textures
	D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("edgeimage.dds").c_str(), &m_pEdgeTexture);
	D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("64shade.bmp").c_str(), &m_pShadeTexture);
	
	m_pD3DDev->SetTexture(0, m_pShadeTexture);
	m_pD3DDev->SetTexture(1, m_pEdgeTexture);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	if(m_bUseTrilinear)
		m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
		
	D3DXVECTOR4 lightDir(1.0f, 0.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&lightDir, &lightDir);
	
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &lightDir, 1);
	
	D3DXVECTOR4 constNums(0.0f, 0.5f, 1.0f, -1.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS, &constNums, 1);
	
	//set view matrix depending on the bounding sphere
	D3DXVECTOR3 eye, lookAt, up;
	
	eye.x    = m_vecCenter.x; eye.y    = m_vecCenter.y; eye.z    = -m_fRadius*2;
	lookAt.x = m_vecCenter.x; lookAt.y = m_vecCenter.y; lookAt.z = m_vecCenter.z;
	up.x	 = 0.0f;          up.y     = 1.0f;          up.z     = 0.0f;
	
	m_pD3DDev->SetVertexShaderConstant(CV_EYE, &eye, 1);
	
	D3DXMatrixIdentity(&m_world);
	D3DXMatrixLookAtLH(&m_view, &eye, &lookAt, &up);
	
	D3DXMatrixPerspectiveFovLH(&m_proj,
		D3DXToRadian(60.0f),
		1,
		1.0f,
		1000.0f);
	m_pD3DDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
	
	//matrices
	SetVertexShaderMatrices();
	
	return S_OK;
}

HRESULT CShaderToonShade::SetVertexShaderMatrices()
{
	D3DXMATRIX worldViewProjMat, worldITMat, worldMat;
	D3DXMATRIX tempMat;
	
	D3DXMatrixMultiply(&tempMat, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix());
	D3DXMatrixTranspose(&worldMat, &tempMat);
	
	D3DXMatrixIdentity(&worldViewProjMat);
	
	D3DXMatrixMultiply(&worldViewProjMat, &tempMat, &m_view);
	
	D3DXMatrixInverse(&worldITMat, NULL, &tempMat);
	
	D3DXMatrixMultiply(&worldViewProjMat, &worldViewProjMat, &m_proj);
	D3DXMatrixTranspose(&worldViewProjMat, &worldViewProjMat);
	
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &worldViewProjMat, 4);
	
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_IT_0, &worldITMat, 4);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &worldMat, 4);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderToonShade::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderToonShade::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	
	SAFE_RELEASE(m_pShadeTexture);
	SAFE_RELEASE(m_pEdgeTexture);
	
	SAFE_DELETE_ARRAY(m_pAttributes);
	SAFE_DELETE_ARRAY(m_pTFactorColors);
	
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}

	SAFE_DELETE(m_pUI);
	
	return S_OK;
}


HRESULT CShaderToonShade::LoadXFile(const char* fileName, const DWORD dwFVF)
{
	ID3DXMesh* tempMesh = 0;
	ID3DXMesh* tempMeshFVF = 0;
	ID3DXMesh* tempMeshOpt = 0;
	ID3DXBuffer* tempMaterials = 0;
	
	HRESULT hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_VB_SYSTEMMEM, m_pD3DDev, NULL, 
		&tempMaterials, &m_dwNumSections, &tempMesh);

	//comptr to ensure this object gets released
	ComPtr< ID3DXBuffer* > tempMaterialBuffer( tempMaterials, COMPTR_DONT_ADD_REF );
	ComPtr< ID3DXMesh* > tempMesh2( tempMesh, COMPTR_DONT_ADD_REF  );

	if(FAILED(hr))
		return hr;

	//compute bounding sphere
	TSVertex* pBuff;
	hr = tempMesh->LockVertexBuffer(D3DLOCK_READONLY, (BYTE**)&pBuff);
	if(FAILED(hr))
		return hr;
	D3DXComputeBoundingSphere(pBuff, tempMesh->GetNumVertices(), tempMesh->GetFVF(), &m_vecCenter, &m_fRadius);
	tempMesh->UnlockVertexBuffer();
	
	//optimize in attribute order
	hr = tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);
	if(FAILED(hr))
		return hr;

	//comptr to ensure this object gets released
	ComPtr< ID3DXMesh* > tempMesh3( tempMeshOpt, COMPTR_DONT_ADD_REF  );

	//get attributes
	DWORD attribSize = m_dwNumSections;
	hr = tempMeshOpt->GetAttributeTable(NULL, &attribSize);
	if(FAILED(hr))
		return hr;
	
	m_pAttributes = new D3DXATTRIBUTERANGE[attribSize];
	
	hr = tempMeshOpt->GetAttributeTable(m_pAttributes, &attribSize);
	if(FAILED(hr))
		return hr;

	//convert to our format, and also make writeonly
	hr = tempMeshOpt->CloneMeshFVF(D3DXMESH_WRITEONLY, dwFVF, m_pD3DDev, &tempMeshFVF);
	if(FAILED(hr))
		return hr;

	//comptr to ensure this object gets released
	ComPtr< ID3DXMesh* > tempMesh4( tempMeshFVF, COMPTR_DONT_ADD_REF  );
	
	tempMeshFVF->GetVertexBuffer(&m_pVertexBuffer);
	tempMeshFVF->GetIndexBuffer(&m_pIndexBuffer);
	
	//create texture colors
	m_pTFactorColors = new D3DCOLOR[ max(m_dwNumSections, 5) ];
	m_pTFactorColors[0] = D3DCOLOR_RGBA(250, 150, 250, 0);
	m_pTFactorColors[1] = D3DCOLOR_RGBA(255, 0, 0, 0);
	m_pTFactorColors[2] = D3DCOLOR_RGBA(0, 0, 255, 0);
	m_pTFactorColors[3] = D3DCOLOR_RGBA(255, 255, 0, 0);
	m_pTFactorColors[4] = D3DCOLOR_RGBA(255, 0, 0, 0);

	for(int i = 4; i < m_dwNumSections; i++)
	{
		m_pTFactorColors[i] = D3DCOLOR_RGBA(255, 255, 255, 255);
	}

	return S_OK;
}


// ------------------------------------------------------------------------------
// CShaderToonShade::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderToonShade::Start()
{
	return S_OK;
}


HRESULT CShaderToonShade::SetTextureStageStates()
{
	//we're going to pass the material color in thru tfactor
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	if(m_eDisplayOption == TOONDISPLAY_SILHOUETTES)
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	else
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	
	//and modulate this resulting color with the edge texture (which should be white everywhere but on the edge)
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	if(m_eDisplayOption == TOONDISPLAY_SHADING)
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	else
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);	

	return S_OK;
}
// ------------------------------------------------------------------------------
// CShaderToonShade::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderToonShade::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	SetVertexShaderMatrices();

	SetTextureStageStates();
  
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));

	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);

	for(int i = 0; i < m_dwNumSections; i++)
	{
		m_pD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_pTFactorColors[i]);
		m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_pAttributes[i].VertexStart, 
		m_pAttributes[i].VertexCount, m_pAttributes[i].FaceStart*3, m_pAttributes[i].FaceCount);
	}

	return hr;
}


void CShaderToonShade::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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


void CShaderToonShade::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderToonShade::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	if(bDown)
	{
		switch(dwKey)
		{
			case 'W' :
            case 'w' :
            {
                m_bWireframe = !m_bWireframe;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            case 'H' :
            case VK_F1 :
            {
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
					   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
                break;
            }

            case VK_HOME :
            case VK_END :
			case VK_NUMPAD7 :
			case VK_NUMPAD1 :
            case '7' :
            case '1' :
            {
                m_pUI->Reset();
                m_bWireframe = false;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            default :
            {
                break;
            }
        }
	}
	SetVertexShaderMatrices();
	return;
}

void CShaderToonShade::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	EBString	name;

	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
	{

		name = pProperty->GetPropertyName();

		if( name == EBString(STR_LOADMESH) )
		{
			// Now do file dialog
			NVXFileDialog aDialog;

			std::string theResult;
			bWritten = ( aDialog.Open( theResult ) );
			
			if ( bWritten )
			{
				SAFE_RELEASE(m_pVertexBuffer);
				SAFE_RELEASE(m_pIndexBuffer);
	
				SAFE_DELETE_ARRAY(m_pAttributes);
				SAFE_DELETE_ARRAY(m_pTFactorColors);

				bWritten = ( LoadXFile( theResult.c_str(), TSVertex::FVF_Flags ) == S_OK );

				SetBuffers();
			}
		}
	}
}
