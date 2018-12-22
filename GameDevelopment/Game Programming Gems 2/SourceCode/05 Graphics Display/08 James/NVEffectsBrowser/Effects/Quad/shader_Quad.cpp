/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "shader_Quad.h"
#include "shader_Quad2.h"
#include "nvinc.h"

using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 2; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{	
	switch(EffectNum)
	{
		case 0:
			return new CShaderQuad2();
			break;
		case 1:
			return new CShaderQuad();
			break;
		default:
			return NULL;
	}
}

}

CShaderQuad::CShaderQuad()
	: m_pVertexBuffer(NULL),
	m_pLogoTexture(NULL),
	m_dwCurrentShader(0),
	m_bWireframe(false),
	m_pUI(NULL)
{
	m_strEffectLocation = "Vertex Shaders\\Beginner Samples";
	m_strEffectName = "Simple Quad";
	m_strEffectVertexShader = GetFilePath("Quad.nvv");
	m_strEffectPixelShader = "";
}

void CShaderQuad::UpdateProperties()
{
	EBEffect::UpdateProperties();

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Dot 3 lit Quad", GetFilePath("Quad.nvv"), EBTYPE_STRING_PROP));
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
}

CShaderQuad::~CShaderQuad()
{
	Free();	
}

HRESULT CShaderQuad::Initialize(LPDIRECT3DDEVICE8 pDev)
{
	HRESULT hr;

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
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("Quad.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

    hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;
		
	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));
	if (FAILED(hr))
		return hr;

	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

	QuadVertex* pBuff;
	if (m_pVertexBuffer)
	{
		float fScale = 1.0f;
		hr = m_pVertexBuffer->Lock(0, sizeof(QuadVertex) * 4,(BYTE**)&pBuff, 0);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't lock buffer!";
			return hr;
		}

		// A Simple Quad
		pBuff->Position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff->Texture = D3DXVECTOR2(0.0f, 1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff->Texture = D3DXVECTOR2(1.0f, 1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff->Texture = D3DXVECTOR2(1.0f, 0.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff->Texture = D3DXVECTOR2(0.0f, 0.0f);
		pBuff++;

		m_pVertexBuffer->Unlock();
	}

	// Create the logo texture
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("nvlogo_only.dds").c_str(),
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
		&m_pLogoTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create nvlogo_only.dds";
		return hr;
	}

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pD3DDev->SetTexture(0, m_pLogoTexture);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);


	return S_OK;

}

HRESULT CShaderQuad::Free()
{
	SAFE_RELEASE(m_pLogoTexture);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_DELETE(m_pUI);

	if (m_pD3DDev)
	{
		if (m_dwCurrentShader)
			m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderQuad::Start()
{
	return S_OK;
}

HRESULT CShaderQuad::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld;
    D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldI;

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// Setup the world, view and projection matrices
	D3DXMatrixMultiply( &matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 1.0f, 50.0f);
		
	// Setup the composite matrix to get vertices to projection space
	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	
	// Create a 3x3 inverse of the worldview for the light transformation back to model space
	D3DXMatrixInverse(&matWorldI, NULL, &matWorld);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(0, &matWorldViewProj(0, 0), 4);
	
	// Transform from world space to model space and point it away from the vertex towards the light
	// Light is pointing into the world
	D3DXVECTOR3 vecLight(0.0f, 0.0f, 1.0f);
	D3DXVec3TransformNormal(&vecLight, &vecLight, &matWorldI);
	D3DXVec3Normalize((D3DXVECTOR3*)&vecLight, (D3DXVECTOR3*)&vecLight);
	vecLight = -vecLight;
	
	m_pD3DDev->SetVertexShaderConstant(4, &vecLight.x, 1);

	// The material color (green)
	D3DXVECTOR4 matColor(1.0f, 1.0f, 1.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(5, &matColor, 1);

	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	return hr;
}

HRESULT CShaderQuad::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

void CShaderQuad::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderQuad::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderQuad::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
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
            m_bWireframe = false;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

        default :
            break;
    }
}
