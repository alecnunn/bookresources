/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "shader_Quad2.h"
#include "nvinc.h"

using namespace std;


typedef struct tagQuadVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
} QuadVertex;

// ------------------------------------------------------------------------------
// CShaderQuad2 Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderQuad2::CShaderQuad2()
	: m_pVertexBuffer(NULL),
	m_pLogoTexture(NULL),
	m_dwCurrentShader(0),
	m_bWireframe(false),
	m_pUI(NULL)
{
	m_strEffectLocation = "Vertex Shaders\\Beginner Samples";
	m_strEffectName = "Simple Quad With Specular";
	m_strEffectVertexShader = GetFilePath("quad2.nvv");
	m_strEffectPixelShader = "";
}

void CShaderQuad2::UpdateProperties()
{
	EBEffect::UpdateProperties();

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Dot 3 lit Quad With Specular", GetFilePath("Quad2.nvv"), EBTYPE_STRING_PROP));
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

}

// ------------------------------------------------------------------------------
// CShaderQuad2 Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderQuad2::~CShaderQuad2()
{
	Free();	
}
 
// ------------------------------------------------------------------------------
// CShaderQuad2::Initialize
//
// Description: INitialize and set the vertex shader and vertex buffer
// ------------------------------------------------------------------------------ 
HRESULT CShaderQuad2::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;

    // get and store the d3d device
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


    // here we declare what the vertex format is for this vertex shader: 
    // the vertex shader will expect 3 floats (position) in v0, 3 floats (normal)
    // in v1, and 2 floats (uv) in v2.
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

    // here we instantiate the vertex shader by loading it's object file and setting the declaration
	hr = LoadAndCreateShader(GetFilePath("Quad2.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

    // and use it for all vertices that are rendered
	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

    // Now we are making a vertex buffer for rendering the quad.
    hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

    // initialize all vertices with the proper position, normal, and uv coordinates
	QuadVertex* pBuff;
	if (m_pVertexBuffer)
	{
		hr = m_pVertexBuffer->Lock(0, sizeof(QuadVertex) * 4,(BYTE**)&pBuff, 0);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't lock buffer!";
			return hr;
		}

		// A Simple Quad
		pBuff->Position = D3DXVECTOR3(-1.0f, -1.0f,  0.0f);
		pBuff->Normal   = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
		pBuff->Texture  = D3DXVECTOR2( 0.0f,  1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3( 1.0f, -1.0f,  0.0f);
		pBuff->Normal   = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
		pBuff->Texture  = D3DXVECTOR2( 1.0f,  1.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3( 1.0f,  1.0f,  0.0f);
		pBuff->Normal   = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
		pBuff->Texture  = D3DXVECTOR2( 1.0f,  0.0f);
		pBuff++;

		pBuff->Position = D3DXVECTOR3(-1.0f,  1.0f,  0.0f);
		pBuff->Normal   = D3DXVECTOR3( 0.0f,  0.0f, -1.0f);
		pBuff->Texture  = D3DXVECTOR2( 0.0f, 0.0f);
		pBuff++;

		m_pVertexBuffer->Unlock();
	}

    // and make this vertex buffer the one to be rendered
    hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));
	if (FAILED(hr))
		return hr;

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

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

    // turn off the second stage
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderQuad2::Free
//
// Description: This is called when we switch away from this example: so we 
//      need to get rid off all memory we are holding on to.
// ------------------------------------------------------------------------------ 
HRESULT CShaderQuad2::Free()
{
    // free all the memory we were using
	SAFE_RELEASE(m_pLogoTexture);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_DELETE(m_pUI);

	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderQuad2::Start
//
// Description: This is called when we switch to this example: so we can init
//      some variables here
// ------------------------------------------------------------------------------ 
HRESULT CShaderQuad2::Start()
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderQuad2::Tick
//
// Description: This is called every frame.  So we recompute matrices and eye-
//      vectors and such, and write them to constant memory.  Of course, we also
//      clear z- and back-buffers and render our primitives.
// ------------------------------------------------------------------------------ 
HRESULT CShaderQuad2::Tick(EBTimer* pTimer)
{
	HRESULT     hr;

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 vLookAtPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );

	D3DXMATRIX  matWorld;
    D3DXMATRIX  matView;
	D3DXMATRIX  matProj;
	D3DXMATRIX  matTemp;
	D3DXMATRIX  matWorldViewProj;
	D3DXMATRIX  matWorldView;
	D3DXMATRIX  matWorldI;

    // Setup the world, view and projection matrices
	D3DXMatrixMultiply( &matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookAtPt, &vUp);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 1.0f, 50.0f);
		
	// Setup the composite matrix to get vertices to projection space
	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	
	// Create a 3x3 inverse of the worldview for the light transformation back to model space
	D3DXMatrixInverse(  &matWorldI, NULL, &matWorld);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	
	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(0, &matWorldViewProj(0, 0), 4);
	
	// Light is pointing into the world and spec'd in world space
	D3DXVECTOR3 vecLight(1.0f, 0.0f, 1.0f);     // light is spec'd in world space
	D3DXVec3Normalize(&vecLight, &vecLight);

    // compute the normalized vector that points towards the eye, along view direction
    D3DXVECTOR3 vecToEye(vEyePt - vLookAtPt);
	D3DXVec3Normalize(&vecToEye, &vecToEye);

    // compute halfway vector h = normalize(light + eye)
	D3DXVECTOR3 vecHalfway(vecLight + vecToEye);

    // Transform the halfway vector from world-space into model-space
	D3DXVec3TransformNormal(&vecHalfway, &vecHalfway, &matWorldI);
	D3DXVec3Normalize(&vecHalfway, &vecHalfway);

    // write halfway vector to constant memory
	m_pD3DDev->SetVertexShaderConstant(5, &vecHalfway.x, 1);

    // write a few specular powers to the constant mwemory
	D3DXVECTOR4 vecSpecularPowers(0.0f, 10.0f, 25.0f, 50.0f);
    m_pD3DDev->SetVertexShaderConstant(7, &vecSpecularPowers.x, 1);
 
	// Transform the light from world space to model space and point it away from the vertex 
    // and towards the light
	D3DXVec3TransformNormal(&vecLight, &vecLight, &matWorldI);
	D3DXVec3Normalize(&vecLight, &vecLight);
	vecLight = -vecLight;
	
	m_pD3DDev->SetVertexShaderConstant(4, &vecLight.x, 1);

    // Transform the eye from world space to model space and point it away from the vertex 
    // and towards the eye
    D3DXVec3TransformNormal(&vecToEye, &vecToEye, &matWorldI);
	D3DXVec3Normalize(&vecToEye, &vecToEye);
	vecToEye = -vecToEye;
	
	m_pD3DDev->SetVertexShaderConstant(8, &vecToEye.x, 1);

	// The material color (white)
	D3DXVECTOR4 matColor(1.0f, 1.0f, 1.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(6, &matColor, 1);

	D3DXVECTOR4 LightColor(1.0f, 1.0f, 1.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(10, &LightColor, 1);

    // draw the quad
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	return hr;
}

HRESULT CShaderQuad2::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

void CShaderQuad2::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderQuad2::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderQuad2::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
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