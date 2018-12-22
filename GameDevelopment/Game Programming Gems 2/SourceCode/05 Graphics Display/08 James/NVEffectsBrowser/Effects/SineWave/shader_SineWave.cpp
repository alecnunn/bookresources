/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments   
      
        
******************************************************************************/

#include "eb_effect.h"
#include "nvinc.h"
#include "shader_SineWave.h"
#include "sinewave.h"

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
			return new CShaderSineWave();
			break;
		default:
			return NULL;
	}
}

}

// ------------------------------------------------------------------------------
// CShaderSineWave Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderSineWave::CShaderSineWave()
    : m_pCubeTexture(NULL)
	, m_pVertexBuffer(NULL)
	, m_bMipMap(true)
	, m_bShowNormals(false)
    , mbWireFrame   (false)
    , mpMouseUI(NULL)
{
	m_strEffectLocation = "Vertex Shaders\\Deformation";
	m_strEffectName = "Sine Wave Perturbation";
	m_strEffectVertexShader = GetFilePath("SineWave.nvv");
	m_strEffectPixelShader = "";
}

// ------------------------------------------------------------------------------
// CShaderSineWave Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderSineWave::~CShaderSineWave()
{
	Free();	
}

// ------------------------------------------------------------------------------
// CShaderSineWave::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CShaderSineWave::UpdateProperties()
{
	EBEffect::UpdateProperties();
    AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(mbWireFrame), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("MipMap the CubeMap", OBJECT_MEMBER(m_bMipMap), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Show Normals", OBJECT_MEMBER(m_bShowNormals), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Vertex Deformation", GetFilePath("SineWave.nvv"), EBTYPE_STRING_PROP));

}

// ------------------------------------------------------------------------------
// CShaderSineWave::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//      supports this demo.  In this particular case we check for cube maps and
//      projected textures.
// ------------------------------------------------------------------------------ 
HRESULT CShaderSineWave::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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

// ------------------------------------------------------------------------------
// CShaderSineWave::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderSineWave::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;

    // get the D3D device 
	m_pD3DDev = pDev;
	pDev->AddRef();

    //initialize mouse UI
	D3DVIEWPORT8    viewport;
	RECT            rect;

	m_pD3DDev->GetViewport(&viewport);
	rect.left   = rect.top = 0;
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	mpMouseUI   = new MouseUI(rect);
    mpMouseUI->SetRotationalSensitivityFactor(1.0f);
    mpMouseUI->SetTranslationalSensitivityFactor(0.1f);

    // declare the vertex format we are going to use with this vertex shader: 
    // each vertex will only have uv coordinates in v0
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	// Load the vertex shader object file and create it.
	m_dwCurrentShader = 0;
	hr = LoadAndCreateShader(GetFilePath("SineWave.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

    // select the just created vertex shader as the thing to transform and light vertices
    hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;


    // constants that describe how our vertex mesh grid looks like
	m_dwGridSize  = 60;
	m_fGridExtent = 1.0f;
	m_fGridStep   = (m_fGridExtent * 2.0f) / (float)(m_dwGridSize - 1);

    // create a vertex buffer that will hold all our vertices
	DWORD   dwVertices = m_dwGridSize * m_dwGridSize * 2;
    hr = m_pD3DDev->CreateVertexBuffer(dwVertices * sizeof(SineVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

    // initialize all vertices with their u,v coordinates
	SineVertex* pBuff;
	int         x, y;
    DWORD       dwCurrentVertex = 0;
		
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Lock(0, sizeof(SineVertex) * dwVertices,(BYTE**)&pBuff, 0);

		for (y = 0; y < (m_dwGridSize - 1); y++)
		{
			for (x = 0; x < m_dwGridSize; x++)
			{
				pBuff[dwCurrentVertex].Position.x = -m_fGridExtent + (x * m_fGridStep);
				pBuff[dwCurrentVertex].Position.y = -m_fGridExtent + (y * m_fGridStep);
				dwCurrentVertex++;
					
				pBuff[dwCurrentVertex].Position.x = -m_fGridExtent + (x * m_fGridStep);
				pBuff[dwCurrentVertex].Position.y = -m_fGridExtent + ((y + 1) * m_fGridStep);
				dwCurrentVertex++;
			}
		}
		assert(dwCurrentVertex <= dwVertices);

		m_pVertexBuffer->Unlock();
	}

    // use this vertex buffer as our source for vertices
    hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(SineVertex));
	if (FAILED(hr))
		return hr;


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

	m_pD3DDev->SetTexture(0, m_pCubeTexture);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	

    // write the constant constants to constant memory now
    return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderSineWave::Free
//
// Description: Called when we switch away from this demo.  Free up all the 
//      memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderSineWave::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);

	SAFE_RELEASE(m_pCubeTexture);
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
    SAFE_DELETE(mpMouseUI);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderSineWave::Start
//
// Description: Called to reset this emo: does nothing
// ------------------------------------------------------------------------------ 
HRESULT CShaderSineWave::Start()
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderSineWave::Tick
//
// Description: Thsi function is called every frame.  Here is where we write
//      and update constants, clear the z- and back-buffer and render our 
//      primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderSineWave::Tick(EBTimer* pTimer)
{
	HRESULT hr;

    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 2.0f, -2.4f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;
	D3DXMATRIX matWorldViewProj;

	// set up the World matrix.
	D3DXMatrixIdentity( &matWorld);
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.0f));

	// View
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixMultiply(&matView, &(mpMouseUI->GetTranslationMatrix()), &matView);
	D3DXMatrixMultiply(&matView, &(mpMouseUI->GetRotationMatrix()), &matView);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1.0f, 0.2f, 20.0f);

    // combine them
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
	
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Worldview inverse transpose transform (for normals)
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_1, &matWorldViewIT(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_2, &matWorldViewIT(2, 0), 1);

    // world view transform
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	// create time constants sin(Time), 0, .5, 1
	float fSinTime = sinf(pTimer->GetDuration() / 4.0f);
	if (fSinTime < 0.0f)
		fSinTime = -fSinTime;

	D3DXVECTOR4 vecTime(fSinTime * 13.0f, 0.06f, 0.0f, 1.0f);
    m_pD3DDev->SetVertexShaderConstant(CV_TIME, &vecTime, 1);
		
	// create Pi-related constants 
    D3DXVECTOR4 vecPi(D3DX_PI, 2.0f * D3DX_PI, 1.0f / (2.0f * D3DX_PI), 0.5f);
    m_pD3DDev->SetVertexShaderConstant(CV_PIS, &vecPi, 1);

	// create convenience constants 
    D3DXVECTOR4 vecConstants(vecTime.x * vecTime.y, 1.0f, 0.0f, 0.0f);
    m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS, &vecConstants, 1);

    // Taylor series coefficients for sin and cos
    D3DXVECTOR4 vecSin(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f);
    D3DXVECTOR4 vecCos(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f);
    m_pD3DDev->SetVertexShaderConstant(CV_VECSIN, &vecSin, 1);
    m_pD3DDev->SetVertexShaderConstant(CV_VECCOS, &vecCos, 1);


    // clear buffers
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

    // set renderstate depending on options
    m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, (m_bShowNormals) ? D3DTOP_SELECTARG2 : D3DTOP_SELECTARG1);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, (m_bMipMap) ? D3DTEXF_LINEAR : D3DTEXF_NONE);

    // and render mesh as a bunch of strips
	DWORD dwCurrentVertex = 0;
	DWORD dwVertexStep    = m_dwGridSize * 2;
	DWORD dwPrimCount     = dwVertexStep - 2;

	for (DWORD i = 0; i < m_dwGridSize; i++)
	{
		m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, dwCurrentVertex, dwPrimCount);
		dwCurrentVertex += dwVertexStep;
	}
	return hr;
}

void CShaderSineWave::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
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
            mbWireFrame = !mbWireFrame;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_RESET:
        {
            mpMouseUI->Reset();
        }
		break;

        default:
            break;
    }
}

void CShaderSineWave::MouseMove(HWND hWnd, int x, int y)
{
    mpMouseUI->OnMouseMove(x, y);
}

void CShaderSineWave::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
			mpMouseUI->OnLButtonDown(x, y);
		else
			mpMouseUI->OnLButtonUp(x, y);
	}
}
