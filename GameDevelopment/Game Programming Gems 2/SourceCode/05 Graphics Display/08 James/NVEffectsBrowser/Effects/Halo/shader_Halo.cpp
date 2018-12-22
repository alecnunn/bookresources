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
#include "shader_Halo.h"
#include "ComPtr.h"
#include "Constants.h"

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
			return new CShaderHalo();
			break;
		default:
			return NULL;
		}
	}
	
}

// ------------------------------------------------------------------------------
// CShaderHalo Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderHalo::CShaderHalo()
: m_pVertexBuffer(NULL), 
  m_pIndexBuffer(NULL),
  m_bWireframe(false),
  m_pUI(NULL),
  m_fRadius(0.0f),
  m_vecCenter(0.0f, 0.0f, 0.0f),
  m_dwNumVerts(0),
  m_dwNumFaces(0),
  m_eDisplayOption(HALO_DISPLAY),
  m_dwToonShader(0),
  m_dwNormalShader(0),
  m_dwHaloShader(0),
  m_fEdgeSize(0.4f)
{
	m_strEffectLocation = "Vertex Shaders\\Render Styles";
	m_strEffectName = "Silhouette Rendering";
	m_strEffectVertexShader = GetFilePath("Halo.nvv");
	m_strEffectPixelShader = "";
	
	D3DXMatrixIdentity(&m_world);
	D3DXMatrixIdentity(&m_view);
	D3DXMatrixIdentity(&m_proj);
}

// ------------------------------------------------------------------------------
// CShaderHalo Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderHalo::~CShaderHalo()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderHalo::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
#define STR_INCREASE_EDGE "Silhouette Increase (+)"
#define STR_DECREASE_EDGE "Silhouette Decrease (-)"

void CShaderHalo::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBTriggerProperty(STR_INCREASE_EDGE));
	AddProperty(new EBTriggerProperty(STR_DECREASE_EDGE));

	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Halo Rendering", (DWORD)HALO_DISPLAY, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Toon Shading", (DWORD)TOON_DISPLAY, EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);
	
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Halo Silhouettes", GetFilePath("Halo.nvv"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Normal TnL", GetFilePath("NormalTransform.nvv"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Toon Shading", GetFilePath("ToonShade.nvv"), EBTYPE_STRING_PROP));
}


// ------------------------------------------------------------------------------
// CShaderHalo::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderHalo::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderHalo::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderHalo::Initialize(IDirect3DDevice8* pDev)
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
	Declaration.push_back(D3DVSD_END());
	
	// Load the two vertex shaders
	m_dwNormalShader = m_dwHaloShader = 0;
	hr = LoadAndCreateShader(GetFilePath("NormalTransform.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwNormalShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadAndCreateShader(GetFilePath("Halo.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwHaloShader);
	if (FAILED(hr))
		return hr;

	hr = LoadAndCreateShader(GetFilePath("ToonShade.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwToonShader);
	if (FAILED(hr))
		return hr;
	
	// select the just created vertex shader as the thing to transform and light vertices
	hr = m_pD3DDev->SetVertexShader(m_dwNormalShader);
	if (FAILED(hr))
		return hr;
	
	DWORD tempFVF = D3DFVF_XYZ | D3DFVF_NORMAL;
	hr = LoadXFile(GetFilePath("bigship1.x").c_str(), tempFVF);
	if(FAILED(hr))
		return hr;
	
	// use this vertex buffer as our source for vertices
	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(HVertex));
	if (FAILED(hr))
		return hr;
	
	//set index buffer
	m_pD3DDev->SetIndices(m_pIndexBuffer, 0);
	
	//set render states
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	LPDIRECT3DTEXTURE8 m_pShadeTexture;
	D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("64shade.bmp").c_str(), &m_pShadeTexture);
	
	m_pD3DDev->SetTexture(0, m_pShadeTexture);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	
	D3DXVECTOR4 lightDir(1.0f, 0.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&lightDir, &lightDir);
	
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &lightDir, 1);

	D3DXVECTOR4 perturb(1.0f, 0.0f, -1.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(30, &perturb, 1);
	
	D3DXVECTOR4 constNums(0.0f, 0.2f, 1.0f, 30.0f);
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
		100.0f);
	m_pD3DDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
	
	//matrices
	SetVertexShaderMatrices();

	return S_OK;
}

HRESULT CShaderHalo::SetVertexShaderMatrices()
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
// CShaderHalo::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderHalo::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwNormalShader);
		m_pD3DDev->DeleteVertexShader(m_dwHaloShader);
		m_pD3DDev->DeleteVertexShader(m_dwToonShader);
		SAFE_RELEASE(m_pD3DDev);
	}

	SAFE_DELETE(m_pUI);
	
	return S_OK;
}


HRESULT CShaderHalo::LoadXFile(const char* fileName, const DWORD dwFVF)
{
	ID3DXMesh* tempMesh, *tempMeshFVF;
	ID3DXBuffer* tempMaterials;
	
	HRESULT hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_VB_SYSTEMMEM, m_pD3DDev, NULL, 
		&tempMaterials, &m_dwNumSections, &tempMesh);

	//comptr to ensure this object gets released
	ComPtr< ID3DXBuffer* > tempMaterialBuffer( tempMaterials, COMPTR_DONT_ADD_REF );
	ComPtr< ID3DXMesh* > tempMesh2( tempMesh, COMPTR_DONT_ADD_REF  );

	if(FAILED(hr))
		return hr;

	//compute bounding sphere
	HVertex* pBuff;
	hr = tempMesh->LockVertexBuffer(D3DLOCK_READONLY, (BYTE**)&pBuff);
	if(FAILED(hr))
		return hr;
	D3DXComputeBoundingSphere(pBuff, tempMesh->GetNumVertices(), tempMesh->GetFVF(), &m_vecCenter, &m_fRadius);
	tempMesh->UnlockVertexBuffer();
	
	//convert to our format, and also make writeonly
	hr = tempMesh->CloneMeshFVF(D3DXMESH_WRITEONLY, dwFVF, m_pD3DDev, &tempMeshFVF);
	if(FAILED(hr))
		return hr;

	//comptr to ensure this object gets released
	ComPtr< ID3DXMesh* > tempMesh4( tempMeshFVF, COMPTR_DONT_ADD_REF  );
	
	tempMeshFVF->GetVertexBuffer(&m_pVertexBuffer);
	tempMeshFVF->GetIndexBuffer(&m_pIndexBuffer);
	m_dwNumVerts = tempMeshFVF->GetNumVertices();
	m_dwNumFaces = tempMeshFVF->GetNumFaces();
	
	return S_OK;
}


// ------------------------------------------------------------------------------
// CShaderHalo::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderHalo::Start()
{
	return S_OK;
}


// ------------------------------------------------------------------------------
// CShaderHalo::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderHalo::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	SetVertexShaderMatrices();

 /*
	float value = sin(D3DXToRadian(m_fAngle));
	value = fabs(value);
	m_fAngle += 2.0f;
	if(m_fAngle > 180.0f)
		m_fAngle = 0.0f;
// */

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));

	if(m_eDisplayOption == HALO_DISPLAY)
	{
        // vary the halo +/- 10%
        float const     kPulse = 0.25f * m_fEdgeSize * sin(1.25f*pTimer->GetDuration());

		m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);

		//pass one

		//arg1 is diffuse
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_pD3DDev->SetVertexShader(m_dwHaloShader);
		D3DXVECTOR4 color(0.9f, 0.9f, 0.0f, 0.0f);
		m_pD3DDev->SetVertexShaderConstant(CV_COLOR, &color, 1);
		D3DXVECTOR4 perturb(kPulse+m_fEdgeSize, 0.0f, 0.0f, 0.0f);
		m_pD3DDev->SetVertexShaderConstant(CV_SCALING, &perturb, 1);
		
		m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVerts, 0, m_dwNumFaces);

		//pass two
		m_pD3DDev->SetVertexShader(m_dwNormalShader);
		m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVerts, 0, m_dwNumFaces);
	}
	else
	{
		m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);
		
		//pass one
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_pD3DDev->SetVertexShader(m_dwHaloShader);
		D3DXVECTOR4 color(0.0f, 0.0f, 0.0f, 0.0f);
		m_pD3DDev->SetVertexShaderConstant(CV_COLOR, &color, 1);
		D3DXVECTOR4 perturb(m_fEdgeSize, 0.0f, 0.0f, 0.0f);
		m_pD3DDev->SetVertexShaderConstant(CV_SCALING, &perturb, 1);
		
		m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVerts, 0, m_dwNumFaces);

		//pass two

		//arg2 is the texture
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		m_pD3DDev->SetVertexShader(m_dwToonShader);
		m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVerts, 0, m_dwNumFaces);
	}


	return hr;
}


void CShaderHalo::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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


void CShaderHalo::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderHalo::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
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
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n + increase silhouette width\n\n - decrease silhouette width",
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

			case VK_ADD:
			case 0xBB:
			case '+':
			case '=':
			{
				if(m_fEdgeSize <= 10.0f)
					m_fEdgeSize += 0.02f;
				break;
			}

			case VK_SUBTRACT:
			case 0xBD:
			case '-':
			case '_':
			{
				m_fEdgeSize -= 0.02f;
				if(m_fEdgeSize < 0)
					m_fEdgeSize = 0;
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

void CShaderHalo::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	EBString name;

	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
	{

		name = pProperty->GetPropertyName();
		
		if(name == EBString(STR_INCREASE_EDGE))
		{
			Keyboard(VK_ADD, 0, true);
		}
		else if (name == EBString(STR_DECREASE_EDGE))
		{
			Keyboard(VK_SUBTRACT, 0, true);
		}
	}
}



