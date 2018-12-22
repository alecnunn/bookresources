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
#include "shader_Reflect.h"
#include "reflect.h"

using namespace nv_objects;
using namespace std;


DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderReflect();
}

}

CShaderReflect::CShaderReflect()
	: m_pCubeTexture(NULL),
		m_pTeapot(NULL),
		m_bMipMap(true),
		m_pNVDevice(NULL),
		m_bWireframe(false),
		m_pUI(NULL)
{
	m_strEffectName = "Cube Mapping";
	m_strEffectLocation = "Vertex Shaders\\Texture Coordinate Generation";
	m_strEffectPixelShader = "";
	m_strEffectVertexShader = GetFilePath("CubeMap.nvv");

}

CShaderReflect::~CShaderReflect()
{
	Free();	
}

void CShaderReflect::UpdateProperties()
{
	EBEffect::UpdateProperties();
	AddProperty(new EBProperty("MipMap the CubeMap", OBJECT_MEMBER(m_bMipMap), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Cubic Reflection Calculation", GetFilePath("CubeMap.nvv"), EBTYPE_STRING_PROP));

}

HRESULT CShaderReflect::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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

HRESULT CShaderReflect::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVDevice(pDev);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("CubeMap.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

	m_pTeapot = new NVMesh();
	hr = m_pTeapot->Create(m_pNVDevice, GetFilePath("teapot.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load teapot.x";
		return hr;
	}
	m_pTeapot->SetFVF(m_pNVDevice, D3DFVF_XYZ | D3DFVF_NORMAL);
	m_pTeapot->SetVertexShader(m_dwCurrentShader);

	    // allocate our cube texture
	hr = D3DXCreateCubeTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sky_cube_mipmap.dds").c_str(),
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

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);
	m_pUI->SetTranslationalSensitivityFactor(0.2f);

	m_pTeapot->SetTexture(m_pCubeTexture);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);


	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_HALF, D3DXVECTOR4(0.5f,0.5f,0.5f,0.5f), 1);

	m_pTeapot->RestoreDeviceObjects(m_pNVDevice);

	return S_OK;

}

HRESULT CShaderReflect::Free()
{
	SAFE_DELETE(m_pTeapot);
	SAFE_RELEASE(m_pCubeTexture);
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(m_pUI);
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderReflect::Start()
{
	return S_OK;
}

HRESULT CShaderReflect::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;
	D3DXMATRIX matWorldViewProj;

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, 0.0f, -2.4f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// Set world rotation/translation
	D3DXMatrixMultiply( &matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );

	// Move the loaded object to the middle and scale
	const NVBounds* pBounds = m_pTeapot->GetBounds();
	D3DXMatrixScaling(&matTemp, 1.0f / pBounds->m_fRadius, 1.0f / pBounds->m_fRadius, 1.0f / pBounds->m_fRadius);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

	D3DXMatrixTranslation(&matTemp, -pBounds->m_vecCenter.x, -pBounds->m_vecCenter.y, -pBounds->m_vecCenter.z);
	D3DXMatrixMultiply(&matWorld, &matTemp, &matWorld);

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 0.2f, 20.0f);

	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
	
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Worldview transform
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	// Worldview ITtransform
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_1, &matWorldViewIT(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_2, &matWorldViewIT(2, 0), 1);
	
	if (m_bMipMap)
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	}

	m_pTeapot->Render(m_pNVDevice);

	return hr;
}


void CShaderReflect::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderReflect::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderReflect::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n +\\- Change Bump Scale\n\n",
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

		case EB_MIPMAP:
		{
			m_bMipMap = !m_bMipMap;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		}
		break;

        default :
            break;
    }
}

