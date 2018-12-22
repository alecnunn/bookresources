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
#include "shader_TexKill.h"
#include "TexKill.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderTexKill();
}

}

typedef struct tagQuadVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
} QuadVertex;

CShaderTexKill::CShaderTexKill()
	: m_pTeapot(NULL),
		m_pNVDevice(NULL),
		m_dwClipPixelShader(0),
		m_dwClipVertexShader(0),
		m_dwPlaneVertexShader(0),
		m_pVertexBuffer(NULL),
		m_pUI(NULL),
		m_bWireframe(false),
		m_bPause(false)
{
	m_strEffectName = "TexKill Clip Planes";
	m_strEffectLocation = "Pixel Shaders\\Clipping";
	m_strEffectPixelShader = GetFilePath("texkill.nvp");
	m_strEffectVertexShader = GetFilePath("texkill.nvv");
}

void CShaderTexKill::UpdateProperties()
{
	EBEffect::UpdateProperties();

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Plane Equation Test", GetFilePath("texkill.nvv"), EBTYPE_STRING_PROP));

	// Pixel shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "TexKill Operation", GetFilePath("texkill.nvp"), EBTYPE_STRING_PROP));

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));

}

CShaderTexKill::~CShaderTexKill()
{
	Free();	
}

HRESULT CShaderTexKill::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}

	if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShaderTexKill::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVDevice(pDev);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);

	D3DXMatrixIdentity(&m_matRotate);

	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_END());

	m_dwClipVertexShader = 0;
	hr = LoadAndCreateShader(GetFilePath("TexKill.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwClipVertexShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadAndCreateShader(GetFilePath("TexKill_planeshow.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwPlaneVertexShader);
	if (FAILED(hr))
		return hr;

	hr = LoadAndCreateShader(GetFilePath("texkill.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwClipPixelShader);
	if (FAILED(hr))
		return hr;

	m_pTeapot = new NVMesh();
	hr = m_pTeapot->Create(m_pNVDevice, GetFilePath("woman.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load sphere.x";
		return hr;
	}
	m_pTeapot->SetFVF(m_pNVDevice, D3DFVF_XYZ | D3DFVF_NORMAL);
	m_pTeapot->SetVertexShader(m_dwClipVertexShader);


	hr = m_pD3DDev->CreateVertexBuffer( 8 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;
	QuadVertex* pBuff;
	if (m_pVertexBuffer)
	{
		hr = m_pVertexBuffer->Lock(0, sizeof(QuadVertex) * 8,(BYTE**)&pBuff, 0);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't lock buffer!";
			return hr;
		}

		// x plane
		pBuff->Position = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(-1.0f, 0.0f, 1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		// y plane
		pBuff->Position = D3DXVECTOR3(0.0f, -1.0f, -1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(0.0f, -1.0f, 1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(0.0f, 1.0f, 1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(0.0f, 1.0f, -1.0f);
		pBuff->Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pBuff++;


		m_pVertexBuffer->Unlock();
	}

	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_FIXED_COLOR, D3DXVECTOR4(0.0f,0.3f,0.5f,0.5f), 1);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


	m_pTeapot->RestoreDeviceObjects(m_pNVDevice);

	return S_OK;

}

HRESULT CShaderTexKill::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_DELETE(m_pTeapot);
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(m_pUI);
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwClipVertexShader);
		m_pD3DDev->DeleteVertexShader(m_dwPlaneVertexShader);
		m_pD3DDev->DeletePixelShader(m_dwClipPixelShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderTexKill::Start()
{
	return S_OK;
}

HRESULT CShaderTexKill::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matProj;
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldInverse;
	D3DXMATRIX matWorldViewProj;

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	// Disable culling
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 1.0f, 0.0f, -3.4f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// Identity World matrix.
	D3DXMatrixMultiply(&matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix());
	
	// Move the loaded object to the middle and scale
	const NVBounds* pBounds = m_pTeapot->GetBounds();
	D3DXMatrixScaling(&matTemp, 1.0f / pBounds->m_fRadius, 1.0f / pBounds->m_fRadius, 1.0f / pBounds->m_fRadius);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);
	m_pUI->SetTranslationalSensitivityFactor(pBounds->m_fRadius);

	D3DXMatrixTranslation(&matTemp, -pBounds->m_vecCenter.x, -pBounds->m_vecCenter.y, -pBounds->m_vecCenter.z);
	D3DXMatrixMultiply(&matWorld, &matTemp, &matWorld);

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1.0f, 0.5f, 100.0f);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);

	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Clip Planes
	D3DXPLANE ClipPlane1(1.0f, 0.0f, 0.0f, 0.0f);
	D3DXPLANE ClipPlane2(0.0f, 1.0f, 0.0f, 0.0f);

	if (!m_bPause)
	{
		D3DXVECTOR3 vRotate(-1.0f, 1.0f, 0.0f);
		D3DXMatrixRotationAxis(&m_matRotate, &vRotate, D3DXToRadian(pTimer->GetDuration() * 6.0f));
	}
	D3DXMATRIX matWorldI;
	D3DXMatrixInverse(&matWorldI, NULL, &matWorld);
	matWorldI = m_matRotate * matWorldI;
	D3DXPlaneTransform(&ClipPlane1, &ClipPlane1, &matWorldI);
	D3DXPlaneTransform(&ClipPlane2, &ClipPlane2, &matWorldI);

	// Texture matrix setup as clip plane
	D3DXMATRIX matClipPlane;
	ZeroMemory(&matClipPlane, sizeof(D3DXMATRIX));

	// 1st clip plane
	matClipPlane(0, 0) = ClipPlane1.a;
	matClipPlane(0, 1) = ClipPlane1.b;
	matClipPlane(0, 2) = ClipPlane1.c;
	matClipPlane(0, 3) = ClipPlane1.d;

	// 2nd clip plane
	matClipPlane(1, 0) = ClipPlane2.a;
	matClipPlane(1, 1) = ClipPlane2.b;
	matClipPlane(1, 2) = ClipPlane2.c;
	matClipPlane(1, 3) = ClipPlane2.d;	
	m_pD3DDev->SetVertexShaderConstant(CV_CLIPPLANE_0, &matClipPlane(0, 0), 4);

	// Add a model-space directional light
	D3DXVECTOR3 vecLight(0.0f, 1.0f, 1.0f);
	D3DXMatrixInverse(&matWorldInverse, NULL, &matWorld);
	D3DXVec3TransformNormal(&vecLight, &vecLight, &matWorldInverse);
	D3DXVec3Normalize((D3DXVECTOR3*)&vecLight, (D3DXVECTOR3*)&vecLight);
	vecLight = -vecLight;
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHTVECTOR, (D3DXVECTOR4*)&vecLight, 1);
	m_pD3DDev->SetVertexShaderConstant(CV_MAT_COLOR, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
	
	m_pD3DDev->SetVertexShader(m_dwClipVertexShader);
	m_pD3DDev->SetPixelShader(m_dwClipPixelShader);

	// Draw clipped teapot
	m_pTeapot->Render(m_pNVDevice);

	// Draw the planes
	m_pD3DDev->SetPixelShader(0);
	m_pD3DDev->SetVertexShader(m_dwPlaneVertexShader);
	
	m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));

	// Draw the planes
	D3DXMatrixMultiply(&matWorldView, &m_matRotate, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);

	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);

	m_pD3DDev->SetVertexShaderConstant(CV_FIXED_COLOR, D3DXVECTOR4(0.0f,0.3f,0.5f,0.2f), 1);
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	m_pD3DDev->SetVertexShaderConstant(CV_FIXED_COLOR, D3DXVECTOR4(0.5f,0.3f,0.0f,0.2f), 1);
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 4, 2);

	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);

	return hr;
}

void CShaderTexKill::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderTexKill::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderTexKill::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Object\n\n Ctrl Left Button & Mouse - Move Object In & Out\n\n",
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
			D3DXMatrixIdentity(&m_matRotate);
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

        default:
            break;
    }
}

