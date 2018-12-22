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
#include "Noise.h"
#include "constants.h"

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
			return new CShaderNoise();
			break;
		default:
			return NULL;
	}
}

}

static const float DEFAULT_FREQUENCY = 3.5f;
static const float DEFAULT_AMPLITUDE = -0.2f;

// ------------------------------------------------------------------------------
// CShaderNoise Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderNoise::CShaderNoise()
: m_bWireFrame(false),
m_fFrequency(DEFAULT_FREQUENCY),
m_fAmplitude(DEFAULT_AMPLITUDE),
m_dwCurrentShader(0),
m_pVertexBuffer(NULL),
m_pUI(NULL)
{
	m_strEffectLocation = "Vertex Shaders\\Procedural Noise";
	m_strEffectName = "Perlin Noise";
	m_strEffectVertexShader = GetFilePath("Noise.nvv");
	m_strEffectPixelShader = "";
}

// ------------------------------------------------------------------------------
// CShaderNoise Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderNoise::~CShaderNoise()
{
	Free();	
}

// ------------------------------------------------------------------------------
// CShaderNoise::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CShaderNoise::UpdateProperties()
{
	EBEffect::UpdateProperties();
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireFrame), EBTYPE_BOOL_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Perlinesque Noise", GetFilePath("Noise.nvv"), EBTYPE_STRING_PROP));

}

// ------------------------------------------------------------------------------
// CShaderNoise::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//      supports this demo.  In this particular case we check for cube maps and
//      projected textures.
// ------------------------------------------------------------------------------ 
HRESULT CShaderNoise::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

/* returns a random floating point number between 0.0 and 1.0 */
float frand()
{
    return (float) (rand() / (float) RAND_MAX);
}

/* returns a random floating point number between -1.0 and 1.0 */
float sfrand()
{
    return (float) (rand() * 2.0/ (float) RAND_MAX) - 1.0;
}

// create a table containing a random permuation of the integers from 0-63
void CShaderNoise::PermuteTable()
{
	D3DXVECTOR4 c[64];
	int i;

	// want reproducable behaviour for debug
	//srand(1);

	for(i=0; i<64; i++) 
	{
		c[i].w = i;
		c[i].x = frand();
		c[i].y = frand();
		c[i].z = frand();
	}

	for(i=0; i<500; i++) 
	{
		// choose two entries at random and swap
		int a, b, t;
		a = (rand() >> 4) & 63;		// lower bits of rand() are bogus sometimes
		b = (rand() >> 4) & 63;
		t = c[a].w;
		c[a].w = c[b].w;
		c[b].w = t;
	}

	for(i=0; i<64; i++) 
	{
		m_pD3DDev->SetVertexShaderConstant(CV_NOISETABLE + i, &c[i], 1);
	}

}


// ------------------------------------------------------------------------------
// CShaderNoise::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderNoise::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;

    // get the D3D device 
	m_pD3DDev = pDev;
	pDev->AddRef();

    // declare the vertex format we are going to use with this vertex shader: 
    // each vertex will only have uv coordinates in v0
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_END());

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);

	// Load the vertex shader object file and create it.
	m_dwCurrentShader = 0;
	hr = LoadAndCreateShader(GetFilePath("Noise.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
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
    hr = m_pD3DDev->CreateVertexBuffer(dwVertices * sizeof(GridVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

    // initialize all vertices with their u,v coordinates
	GridVertex* pBuff;
	int         x, y;
    DWORD       dwCurrentVertex = 0;
		
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Lock(0, sizeof(GridVertex) * dwVertices,(BYTE**)&pBuff, 0);

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
    hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(GridVertex));
	if (FAILED(hr))
		return hr;

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

    // write the constant constants to constant memory now
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE,  D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_NOISEFACTORS_FREQ, D3DXVECTOR4(1.0f / 64.0f, 64.0f, 0.0f, 1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_SMOOTHSTEP, D3DXVECTOR4(0.0f, 1.0f, 2.0f, 3.0f), 1);

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	PermuteTable();

    return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderNoise::Free
//
// Description: Called when we switch away from this demo.  Free up all the 
//      memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderNoise::Free()
{
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
// CShaderNoise::Start
//
// Description: Called to reset this emo: does nothing
// ------------------------------------------------------------------------------ 
HRESULT CShaderNoise::Start()
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderNoise::Tick
//
// Description: Thsi function is called every frame.  Here is where we write
//      and update constants, clear the z- and back-buffer and render our 
//      primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderNoise::Tick(EBTimer* pTimer)
{
	HRESULT hr;
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

	m_pD3DDev->SetVertexShaderConstant(CV_NOISEFACTORS, D3DXVECTOR4(m_fFrequency, m_fAmplitude, 0.0f, 0.0f), 1);

	// Identity World matrix.
	D3DXMATRIX matRotate;
	D3DXVECTOR3 vRotate(1.0f, 0.0f, 0.0f);
	D3DXMatrixRotationAxis(&matRotate, &vRotate, D3DXToRadian(90.0f));

	D3DXMatrixMultiply(&matWorld, &m_pUI->GetRotationMatrix(), &matRotate);	
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_pUI->GetTranslationMatrix());

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1.0f, 0.2f, 20.0f);

    // combine them
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);
	
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &matWorld(0, 0), 4);


	if (m_bWireFrame)
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

    // clear buffers
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA), 1.0, 0);

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

void CShaderNoise::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Object\n\n Ctrl Left Button & Mouse - Move Object In & Out\n\n PageUp\\PageDown - Change frequency\n\n Add\\Subtract - Change Amplitude\n\n",
                           "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireFrame = !m_bWireFrame;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_RESET:
        {
			m_bWireFrame = false;
			m_fFrequency = DEFAULT_FREQUENCY;
			m_fAmplitude = DEFAULT_AMPLITUDE;
            m_pUI->Reset();
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_PAGEUP:
		{
			m_fFrequency += 0.06f;
		}
		break;

		case EB_PAGEDOWN:
		{
			m_fFrequency -= 0.06f;
		}
		break;

		case EB_ADD:
		{
			m_fAmplitude -= 0.02f;
		}
		break;

		case EB_SUBTRACT:
		{
			m_fAmplitude += 0.02f;
		}
		break;

        default:
            break;
    }

}

void CShaderNoise::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderNoise::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

