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
#include "shader_PaletteSkin.h"
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
			return new CShaderPaletteSkin();
			break;
		default:
			return NULL;
		}
	}
	
}

// ------------------------------------------------------------------------------
// CShaderPaletteSkin Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderPaletteSkin::CShaderPaletteSkin()
: m_pRenderVB(NULL), 
  m_bWireframe(false),
  m_pUI(NULL),
  m_fElapsedTime(0.0f),
  m_fLastTime(0.0f),
  m_bAnimateLight(false),
  m_bAddDecal(true),
  m_pBumpMapTexture(NULL),
  m_dwCurrentShader(0),
  m_bPaused(false)
{
	m_strEffectLocation = "Vertex Shaders\\Animation";
	m_strEffectName = "Matrix Palette Skinning";
	m_strEffectVertexShader = GetFilePath("PaletteSkin.nvv");
	m_strEffectPixelShader = "";
	
	m_pmcSelectedMesh = NULL;
	m_pframeSelected  = NULL;
	m_pdeSelected     = NULL;
	m_pdeHead         = NULL;

	D3DXMatrixIdentity(&m_world);
	D3DXMatrixIdentity(&m_view);
	D3DXMatrixIdentity(&m_proj);
}

// ------------------------------------------------------------------------------
// CShaderPaletteSkin Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderPaletteSkin::~CShaderPaletteSkin()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderPaletteSkin::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CShaderPaletteSkin::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Animate Light", OBJECT_MEMBER(m_bAnimateLight), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Add Decal", OBJECT_MEMBER(m_bAddDecal), EBTYPE_BOOL_PROP));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Matrix Palette Skinning", GetFilePath("PaletteSkin.nvv"), EBTYPE_STRING_PROP));
}


// ------------------------------------------------------------------------------
// CShaderPaletteSkin::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderPaletteSkin::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderPaletteSkin::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderPaletteSkin::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr = S_OK;
	
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
	
	m_Declaration.push_back(D3DVSD_STREAM(0));
	m_Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	m_Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT1)); //first blend weight
	m_Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT1)); //second blend weight
	m_Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT2)); //index 1 and 2
	m_Declaration.push_back(D3DVSD_REG(4, D3DVSDT_FLOAT3)); //normal
	m_Declaration.push_back(D3DVSD_REG(5, D3DVSDT_FLOAT2)); //texcoord
	m_Declaration.push_back(D3DVSD_REG(6, D3DVSDT_FLOAT3)); //S
	m_Declaration.push_back(D3DVSD_REG(7, D3DVSDT_FLOAT3)); //T
	m_Declaration.push_back(D3DVSD_REG(8, D3DVSDT_FLOAT3)); //SxT
	m_Declaration.push_back(D3DVSD_END());

	LoadMeshHierarchy(GetFilePath("tiny.x").c_str());

	// Load the vertex shader object file and create it.
	m_dwCurrentShader = 0;
	hr = LoadAndCreateShader(GetFilePath("PaletteSkin.vso"), &m_Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	// select the just created vertex shader as the thing to transform and light vertices
	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

	//load bump texture
	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("tiny_skin2.dds").c_str(), &m_pBumpMapTexture);
	if(FAILED(hr))
		return hr;

	//set render states
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDev->SetTexture(0, m_pBumpMapTexture);

	//L dot N
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	//current modulate texture
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	
	if(m_bAddDecal)
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	else
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	m_lightDirection.x = 0.0f;
	m_lightDirection.y = -1.0f;
	m_lightDirection.z = 0.0f;
	m_lightDirection.w = 0.0f;
	D3DXVec4Normalize(&m_lightDirection, &m_lightDirection);
	
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &m_lightDirection, 1);
	
	D3DXVECTOR4 constNums(0.0f, 0.5f, 1.0f, 0.2f);
	m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS, &constNums, 1);
	
	//character is initially in an odd pose, this repositions her
	D3DXMATRIX tempMat, tempMat2;
	D3DXMatrixRotationX(&tempMat, D3DXToRadian(-60.0f));
	D3DXMatrixRotationY(&tempMat2, D3DXToRadian(180.0f));
	D3DXMatrixMultiply(&m_world, &tempMat, &tempMat2);

	m_pUI->SetTranslationalSensitivityFactor(m_pdeSelected->fRadius * 0.05f);

	//set view matrix depending on the bounding sphere
	D3DXVECTOR3 eye, lookAt, up;
	SDrawElement *pdeCur;
	
	pdeCur = m_pdeHead;
	eye.x    = pdeCur->vCenter.x; eye.y    = pdeCur->vCenter.y + 200; eye.z    = -m_pdeSelected->fRadius*2;
	lookAt.x = pdeCur->vCenter.x; lookAt.y = pdeCur->vCenter.y + 200; lookAt.z = pdeCur->vCenter.z;
	up.x	 = 0.0f;              up.y     = 1.0f;                    up.z     = 0.0f;

	D3DXMatrixLookAtLH(&m_view, &eye, &lookAt, &up);
	
	D3DXMatrixPerspectiveFovLH(&m_proj,
		D3DXToRadian(60.0f),
		1,
		200.0f,
		3000.0f);
	m_pD3DDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
	
	SetVertexShaderMatrices();
	
	return hr;
}

HRESULT CShaderPaletteSkin::SetVertexShaderMatrices()
{
	D3DXMATRIX worldViewProjMat, worldMat;
	
	D3DXMatrixIdentity(&worldMat);
	D3DXMatrixIdentity(&worldViewProjMat);
	
	D3DXMatrixMultiply(&worldViewProjMat, &worldMat, &m_view);
	
	D3DXMatrixMultiply(&worldViewProjMat, &worldViewProjMat, &m_proj);
	D3DXMatrixTranspose(&worldViewProjMat, &worldViewProjMat);
	
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &worldViewProjMat, 4);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderPaletteSkin::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderPaletteSkin::Free()
{
	SAFE_RELEASE(m_pRenderVB);
	SAFE_RELEASE(m_pBumpMapTexture);
	
	if (m_pD3DDev)
	{
		DeleteSelectedMesh();
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}

	SAFE_DELETE(m_pUI);
	
	return S_OK;
}


// ------------------------------------------------------------------------------
// CShaderPaletteSkin::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderPaletteSkin::Start()
{
	return S_OK;
}


// ------------------------------------------------------------------------------
// CShaderPaletteSkin::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderPaletteSkin::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	if(m_bPaused)
		m_fElapsedTime = 0.0f;
	else
		m_fElapsedTime = ((float)timeGetTime()) - m_fLastTime;

	m_fLastTime = timeGetTime();

	// set the elapsed time so the animation knows the rate at which to proceed
	SDrawElement *pdeCur;
    SFrame *pframeCur;

    pdeCur = m_pdeHead;
    while (pdeCur != NULL)
    {
        pdeCur->fCurTime += m_fElapsedTime * 4;
        if (pdeCur->fCurTime > 1.0e15f)
            pdeCur->fCurTime = 0;

        pframeCur = pdeCur->pframeAnimHead;
        while (pframeCur != NULL)
        {
            pframeCur->SetTime(pdeCur->fCurTime);
            pframeCur = pframeCur->pframeAnimNext;
        }

        pdeCur = pdeCur->pdeNext;
    }

	//rotate light vector
	static float currAngle = 0.0f;
	if(m_bAnimateLight)
	{
		currAngle += 1.f;
		m_lightDirection.x = -cos(D3DXToRadian(currAngle));
		m_lightDirection.y = 0.0f;
		m_lightDirection.z = -sin(D3DXToRadian(currAngle));
		m_lightDirection.w = 0.0f;
		D3DXVec4Normalize(&m_lightDirection, &m_lightDirection);
		m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &m_lightDirection, 1);
	}

    pdeCur = m_pdeHead;
    while (pdeCur != NULL)
    {
		D3DXMATRIX tempMat;
		D3DXMatrixMultiply(&tempMat, &m_world, &m_pUI->GetRotationMatrix());
        pdeCur->pframeRoot->matRot = tempMat;
		pdeCur->pframeRoot->matTrans = m_pUI->GetTranslationMatrix();
        pdeCur = pdeCur->pdeNext;
    }
  
	//set texture stage state based on user choice
	if(m_bAddDecal)
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	else
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));

	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);

	////////////////////////////////////////////////////////////////////////////////////////
	{
	    UINT cTriangles = 0;
        HRESULT hr = S_OK;
        SDrawElement *pdeCur;
        D3DXMATRIX matD3D;
        D3DXMATRIX mOut;
        D3DXMATRIX mCur;
        D3DXVECTOR3 vTemp;
        D3DXMATRIX mObject;
        D3DXMATRIX mTrans;
        LPDIRECT3DDEVICE8 m_pDevice = m_pD3DDev;

		pdeCur = m_pdeHead;

        if(FAILED(hr))
            return hr;

        while (pdeCur != NULL)
        {
			D3DXMatrixIdentity(&mCur);
            hr = UpdateFrames(pdeCur->pframeRoot, mCur);
            if (FAILED(hr))
                return hr;
            hr = DrawFrames(pdeCur->pframeRoot, cTriangles);
            if (FAILED(hr))
                return hr;

            pdeCur = pdeCur->pdeNext;
        }
	}
	////////////////////////////////////////////////////////////////////////////////////////

	return hr;
}


void CShaderPaletteSkin::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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


void CShaderPaletteSkin::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
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
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n Spacebar - Toggle Animation",
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

			case VK_SPACE:
			{
				m_bPaused = !m_bPaused;
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


void CShaderPaletteSkin::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}


