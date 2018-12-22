/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// RenderD3DWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "NVEffectsBrowser.h"
#include "RenderD3DWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define DISPLAY_D3D_TEXT

using namespace std;
using namespace nv_objects;

// Old-style D3D vertices
class Vertex
{
public:
	Vertex(const D3DXVECTOR3& Pos, const D3DXVECTOR3& Norm, const D3DXVECTOR2& Tex)
		: Position(Pos), Normal(Norm), Texture(Tex)
	{}

	Vertex()
	{}

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

class TLVertex
{
public:
	D3DXVECTOR4 Position;
	DWORD Diffuse;
	DWORD Specular;
	D3DXVECTOR2 Texture;
};

#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)



/////////////////////////////////////////////////////////////////////////////
// CRenderD3DWnd

IMPLEMENT_DYNCREATE(CRenderD3DWnd, CRenderWnd)

CRenderD3DWnd::CRenderD3DWnd()
  : m_devType(D3DDEVTYPE_REF),
	m_devBhv(0),
	m_bActive(true),
	m_colorBufferFormat(D3DFMT_UNKNOWN),
   	m_MultiSampleType(D3DMULTISAMPLE_NONE),
   	m_pD3DDev(NULL),
 	m_bUseSWVP(false),
 	m_pLogoVB(NULL),
 	m_pLogoTexture(NULL),
	m_bValid(false),
	m_pFontTexture(NULL),
	m_pFontVB(NULL)
{
	// Font setup
    _tcscpy( m_strFontName, _T("Arial"));
    m_dwFontHeight   = 10;
    m_dwFontFlags    = D3DFONT_BOLD;
}

CRenderD3DWnd::~CRenderD3DWnd()
{
}


BEGIN_MESSAGE_MAP(CRenderD3DWnd, CRenderWnd)
	//{{AFX_MSG_MAP(CRenderD3DWnd)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRenderD3DWnd parent class overrides

HRESULT CRenderD3DWnd::Init()
{
	return InitD3D();
}

HRESULT CRenderD3DWnd::Release()
{
	return ReleaseD3D();
}

void CRenderD3DWnd::GetDevice(LPDIRECT3DDEVICE8* pDev) 
{
	if (!m_pD3DDev)
		*pDev = NULL;

	if (m_pD3DDev) {
		*pDev = m_pD3DDev; 
		m_pD3DDev->AddRef();
	}
}

HRESULT CRenderD3DWnd::UpdateDevice()
{
	HRESULT hr;
	m_devType = theApp.GetDeviceType();

	CNVEffectsDoc* pDoc = GetDocument();

	pDoc->DeleteDeviceObjects();

	hr = ReleaseD3D();
	if (FAILED(hr)) 
		return hr;

	hr = InitD3D();
	if (FAILED(hr)) 	
		return hr;

	hr = pDoc->InitDeviceObjects();
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT CRenderD3DWnd::ConfirmDevice(EBEffect *pEffect)
{
	D3DCAPS8 Caps;
	m_pD3DDev->GetDeviceCaps(&Caps);
	
	HRESULT hr = pEffect->ConfirmDevice(&Caps, 0, D3DFMT_UNKNOWN);
    if (FAILED(hr))
        SetDefaultError(_T("This effect cannot run in this window or on this hardware."));

    return hr;
}

HRESULT CRenderD3DWnd::SetupEffect(EBEffect *pEffect)
{
	ASSERT(m_pD3DDev);
	if (!m_pD3DDev)
		return NVEFF_ERR_NOTINITIALIZED;
	
	ASSERT(pEffect);
	if (!pEffect)
		return NVEFF_ERR_NOTINITIALIZED;

	HRESULT hr;

	hr = m_pD3DDev->TestCooperativeLevel();
	if (FAILED(hr))
        return hr;

	hr = ConfirmDevice(pEffect);
	if (FAILED(hr))
        return hr;

	hr = SetDefaultRenderstates();
	if (FAILED(hr))
        return hr;

	hr = pEffect->Initialize(m_pD3DDev);
    if (FAILED(hr)) {
        SetDefaultError(_T("This effect was unable to initialize."));
        return hr;
    }

	CRect r;
	GetClientRect(r);
	pEffect->Resize(GetSafeHwnd(), r.Width(), r.Height(), true);

	return S_OK;
}

HRESULT CRenderD3DWnd::SetDefaultRenderstates()
{
	ASSERT(m_pD3DDev);
	if (!m_pD3DDev)
		return NVEFF_ERR_NOTINITIALIZED;

	return SetDefaultRenderstatesD3D();
}

HRESULT CRenderD3DWnd::Render(bool bDrawLogo)
{
	ASSERT(m_pD3DDev);
	if (!m_pD3DDev)
		return NVEFF_ERR_NOTINITIALIZED;

	CRect r;
	GetClientRect(&r);
	CNVEffectsDoc* pDoc = GetDocument();
	if (!m_bValid || !IsWindowVisible() ||
		(r.Width() <= 0 || r.Height() <= 0) ||
		!pDoc->IsEffectValid())
		return NVEFF_ERR_NOTINITIALIZED;

	HRESULT hr;

	hr = m_pD3DDev->BeginScene();
	if (FAILED(hr))
        return hr;

    hr = pDoc->Tick();
    if (FAILED(hr))
        return hr;

	if (bDrawLogo)
		DrawLogo();

	DrawText();

	hr = m_pD3DDev->EndScene();
	if (FAILED(hr))
        return hr;

	hr = m_pD3DDev->Present(NULL, NULL, NULL, NULL);
	if (FAILED(hr))
        return hr;

	// Update the FPS, TPS, VPS labels...
	GetRenderView()->UpdateLabels();
	
	return hr;
}

HRESULT CRenderD3DWnd::Refresh(CDC* pDC)
{
	ASSERT(m_pD3DDev);
	if (!m_pD3DDev)
		return NVEFF_ERR_NOTINITIALIZED;

	HRESULT hr;

	hr = m_pD3DDev->Present(NULL, NULL, GetSafeHwnd(), NULL);
	if (FAILED(hr))
        return hr;

	return hr;
}

HRESULT CRenderD3DWnd::Clear()
{
	ASSERT(m_pD3DDev);
	if (!m_pD3DDev)
		return NVEFF_ERR_NOTINITIALIZED;

	HRESULT hr;

	// Clear to grey
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);
	if (FAILED(hr))
        return hr;
	
	hr = m_pD3DDev->Present(NULL, NULL, GetSafeHwnd(), NULL);
	if (FAILED(hr))
        return hr;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CRenderD3DWnd message handlers

void CRenderD3DWnd::OnSize(UINT nType, int cx, int cy) 
{
	CRenderWnd::OnSize(nType, cx, cy);

	// Short circuit if this window is hidden
	if (!IsWindowVisible())
		return;

	CNVEffectsDoc* pDoc = GetDocument();

	m_bValid = false;

	// Update the D3D device...
	{
		pDoc->DeleteDeviceObjects();
		FreeTextBuffer();
		
		if (m_pD3DDev) {
			
			HRESULT hr;
			
			hr = m_pD3DDev->Reset(&m_d3dpp);
			if (FAILED(hr)) {
				DISPDBG(0, "ERROR: Failed resize");
				return;
			}
			
			m_bValid = true;
			
			hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);
			if (FAILED(hr))	{
				DISPDBG(0, "ERROR: Failed clear");
				return;
			}
			
			hr = ResizeLogo();
			if (FAILED(hr))	{
				DISPDBG(0, "ERROR: Failed logo resize");
				return;
			}
			
			hr = SetupTextBuffer();
			if (FAILED(hr))	{
				DISPDBG(0, "ERROR: Failed setup of text VB");
				return;
			}
			
			pDoc->InitDeviceObjects();
		}
	}
}

void CRenderD3DWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRenderD3DWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CRenderD3DWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnLButtonDown(nFlags, point);
}

void CRenderD3DWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnLButtonUp(nFlags, point);
}

void CRenderD3DWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnMouseMove(nFlags, point);
}

void CRenderD3DWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnRButtonDown(nFlags, point);
}

void CRenderD3DWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnRButtonUp(nFlags, point);
}

void CRenderD3DWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CRenderD3DWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CRenderD3DWnd private implementation 

HRESULT CRenderD3DWnd::InitD3D()
{
	D3DDISPLAYMODE DisplayMode;
	UINT i;
	IDirect3D8 *pEnum = Direct3DCreate8(D3D_SDK_VERSION);

	m_bValid = false;
    if (pEnum == NULL)
	{
		assert(0);
        return E_FAIL;
	}

    HRESULT hr;
 
	m_devType = theApp.GetDeviceType();
	
	// Use current display mode
	if(FAILED(pEnum->GetAdapterDisplayMode(D3DADAPTER_DEFAULT , &DisplayMode)))
	{
		MessageBox("Could not get adapter display mode!", "ERROR", MB_OK);
		return E_FAIL;
	}

	m_colorBufferFormat = DisplayMode.Format;
    
	m_devType = theApp.GetDeviceType();
    memset(&m_d3dpp, 0, sizeof(m_d3dpp));
    m_d3dpp.Windowed = TRUE;
    m_d3dpp.BackBufferFormat = m_colorBufferFormat;
    m_d3dpp.MultiSampleType = m_MultiSampleType;
    m_d3dpp.EnableAutoDepthStencil = TRUE;

    D3DFORMAT s_ZS16[] = 
    { 
        D3DFMT_D16_LOCKABLE,
        D3DFMT_D16,
        D3DFMT_D15S1,
        D3DFMT_D32,
        D3DFMT_D24S8,
        D3DFMT_D24X4S4,
        D3DFMT_D24X8
    };

    D3DFORMAT s_ZS32[] = 
    { 
        D3DFMT_D32,
        D3DFMT_D24S8,
        D3DFMT_D24X4S4,
        D3DFMT_D24X8,
        D3DFMT_D16_LOCKABLE,
        D3DFMT_D16,
        D3DFMT_D15S1
    };

    D3DFORMAT *pZS;

    switch(m_colorBufferFormat)
    {
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_X4R4G4B4:
			pZS = s_ZS16;
			break;

		case D3DFMT_R8G8B8:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
		default:
			pZS = s_ZS32;
			break;
    }

    for(i = 0; i < 7; i++)
    {

        if(FAILED(pEnum->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_devType, 
            m_colorBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, pZS[i])))
        {
            continue;
        }

        if(FAILED(pEnum->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, m_devType, 
            m_colorBufferFormat, m_colorBufferFormat, pZS[i])))
        {
            continue;
        }

        break;
    }

    if(i == 7)
	{
		MessageBox("Could not find suitable framebuffer/z buffer format!", "ERROR", MB_OK);
		return E_FAIL;
	}

	m_d3dpp.AutoDepthStencilFormat = pZS[i];

    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_COPY;
	m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	m_devBhv &=~(D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_HARDWARE_VERTEXPROCESSING);
    if((m_devBhv & D3DCREATE_PUREDEVICE) == 0)
    {
        D3DCAPS8 caps;
        // Query caps
        hr = pEnum->GetDeviceCaps(D3DADAPTER_DEFAULT, m_devType, &caps);
        if((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
        {
			if(D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion) < 1)
			{
				m_devBhv |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				m_bUseSWVP = true;

			}
			else
			{
				if (theApp.GetForceSoftwareVP())
				{
					m_bUseSWVP = true;
					m_devBhv |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}
				else
				{
					m_bUseSWVP = false;
					m_devBhv |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
				}
			}
        }
        else
        {
            m_devBhv |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            m_bUseSWVP = true;
        }
    }
    else
    {
        m_devBhv |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

	assert(GetSafeHwnd());
    hr = pEnum->CreateDevice(
            D3DADAPTER_DEFAULT,     // iAdapter
            m_devType,              // device type
            GetSafeHwnd(),          // focus
            m_devBhv,               // dwBehaviorFlags
            &m_d3dpp,                 // presentation parameters
            &m_pD3DDev);

	if (FAILED(hr))
	{
		InterpretError(hr);
		MessageBox("Failed to create device!", "ERROR", MB_OK);
		return E_FAIL;
	}
	m_pD3DDev->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, m_bUseSWVP);
    SAFE_RELEASE(pEnum);

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	hr = InitLogo();
	if (FAILED(hr))
		return hr;

	if (FAILED(SetupTextImage()))
	{
		InterpretError(hr);
		MessageBox("Failed to create font", "ERROR", MB_OK);

		return E_FAIL;
	}

	if (FAILED(SetupTextBuffer()))
	{
		InterpretError(hr);
		MessageBox("Failed to create text VB", "ERROR", MB_OK);

		return E_FAIL;
	}

	m_bValid = true;
	
    return S_OK;
}

HRESULT CRenderD3DWnd::ReleaseD3D()
{
	// Logo stuff
    SAFE_RELEASE(m_pLogoTexture);
	SAFE_RELEASE(m_pLogoVB);

	// Text stuff
	FreeTextImage();
	FreeTextBuffer();
    
	SAFE_RELEASE(m_pD3DDev);
    return S_OK;
}

// Sets up a default set of renderstates
HRESULT CRenderD3DWnd::SetDefaultRenderstatesD3D()
{
	if (!m_bValid)
		return NVEFF_ERR_NOTINITIALIZED;

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pD3DDev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_FOGCOLOR, 0xFF0000);
	m_pD3DDev->SetRenderState(D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
	m_pD3DDev->SetRenderState(D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
	m_pD3DDev->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE );
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CLIPPING, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_COLORVERTEX, FALSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(5, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(6, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(7, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_TEXCOORDINDEX, 4);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_TEXCOORDINDEX, 5);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_TEXCOORDINDEX, 6);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_TEXCOORDINDEX, 7);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(4, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(5, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(6, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(7, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(4, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(5, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(6, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	m_pD3DDev->SetTextureStageState(7, D3DTSS_TEXTURETRANSFORMFLAGS, 0);

	m_pD3DDev->SetRenderState( D3DRS_WRAP0, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP1, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP2, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP3, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP4, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP5, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP6, 0);
	m_pD3DDev->SetRenderState( D3DRS_WRAP7, 0);

	m_pD3DDev->SetTexture(0, NULL);
	m_pD3DDev->SetTexture(1, NULL);
	m_pD3DDev->SetTexture(2, NULL);
	m_pD3DDev->SetTexture(3, NULL);
	m_pD3DDev->SetTexture(4, NULL);
	m_pD3DDev->SetTexture(5, NULL);
	m_pD3DDev->SetTexture(6, NULL);
	m_pD3DDev->SetTexture(7, NULL);
	
	m_pD3DDev->SetPixelShader(0);

	return S_OK;
}

HRESULT CRenderD3DWnd::InitLogo()
{
	// Get the shared logo bits
	HRESULT hr;
	BYTE *lpImg;
	UINT w, h, w2, h2;

	hr = GetRenderView()->GetSharedLogoBits(&lpImg, &w, &h, &w2, &h2);
	if (FAILED(hr) || !lpImg)
		return E_FAIL;

	// Create the logo view
	hr = D3DXCreateTexture(m_pD3DDev, w2, h2, 1, 0, D3DFMT_A8R8G8B8,
						   D3DPOOL_MANAGED, &m_pLogoTexture);
	if (FAILED(hr)) {
		InterpretError(hr);
		MessageBox("Failed to create logo texture!", "ERROR", MB_OK);
		return E_FAIL;
	}

	// Lock the texture bits
	D3DLOCKED_RECT lockedRect;
	hr = m_pLogoTexture->LockRect(0, &lockedRect, NULL, 0x0);
	if (FAILED(hr)) {
		InterpretError(hr);
		MessageBox("Failed to create logo texture!", "ERROR", MB_OK);
		return E_FAIL;
	}

	// Copy the shared bits into the texture
	ASSERT(lockedRect.pBits);
	memcpy(lockedRect.pBits, lpImg, w2*h2*4);

	// Release the texture bits
	hr = m_pLogoTexture->UnlockRect(0);
	if (FAILED(hr)) {
		InterpretError(hr);
		MessageBox("Failed to create logo texture!", "ERROR", MB_OK);
		return E_FAIL;
	}

	// Create a big square for rendering the Logo mask
    hr = m_pD3DDev->CreateVertexBuffer( 4*sizeof(TLVertex),
                                                 D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX, 
                                                  D3DPOOL_MANAGED, &m_pLogoVB );
	if (FAILED(hr)) {
		InterpretError(hr);
		MessageBox("Failed to create logo vertex buffer!", "ERROR", MB_OK);
		return E_FAIL;
	}

	if (FAILED(ResizeLogo())) {
		InterpretError(hr);
		MessageBox("Failed to create logo vertex buffer!", "ERROR", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderD3DWnd::DrawLogo()
{
	DWORD dwVertexShader;
	DWORD dwPixelShader;
	UINT VBStride;
	LPDIRECT3DVERTEXBUFFER8 pVB;
	LPDIRECT3DBASETEXTURE8 pTexture0;
	DWORD i;

	if (!m_bValid)
		return NVEFF_ERR_NOTINITIALIZED;

	typedef struct tagRenderStates
	{
		D3DRENDERSTATETYPE Type;
		DWORD dwValue;
		DWORD dwDesiredValue;
	} tRenderStates;

	typedef struct tagTextureStageStates
	{
		D3DTEXTURESTAGESTATETYPE Type;
		DWORD dwValue;
		DWORD dwDesiredValue;
	} tTextureStates;


	tRenderStates SavedRender[] = 
	{
		{ D3DRS_ZENABLE, 0, FALSE}, 
		{ D3DRS_ALPHATESTENABLE, 0, TRUE },
		{ D3DRS_ALPHAREF, 0, 0x80 },
		{ D3DRS_ALPHAFUNC, 0, D3DCMP_GREATER},
		{ D3DRS_ALPHABLENDENABLE, 0, TRUE},
		{ D3DRS_SRCBLEND, 0, D3DBLEND_SRCALPHA },
		{ D3DRS_DESTBLEND, 0, D3DBLEND_INVSRCALPHA },
		{ D3DRS_FOGENABLE, 0, FALSE },
		{ D3DRS_CULLMODE, 0, D3DCULL_NONE },
		{ D3DRS_FILLMODE, 0, D3DFILL_SOLID},
		{ D3DRS_WRAP0, 0, 0}
	};

	tTextureStates SavedTextureStage0[] = 
	{
		{ D3DTSS_COLOROP, 0, D3DTOP_SELECTARG1 }, 
		{ D3DTSS_COLORARG1, 0, D3DTA_TEXTURE },
		{ D3DTSS_ALPHAOP, 0, D3DTOP_SELECTARG1 },
		{ D3DTSS_ALPHAARG1, 0, D3DTA_TEXTURE },
		{ D3DTSS_TEXCOORDINDEX, 0, 0 },
		{ D3DTSS_MINFILTER, 0, D3DTEXF_LINEAR },
		{ D3DTSS_MAGFILTER, 0, D3DTEXF_LINEAR },
	};

	tTextureStates SavedTextureStage1[] = 
	{
		{ D3DTSS_COLOROP, 0, D3DTOP_DISABLE }, 
		{ D3DTSS_ALPHAOP, 0, D3DTOP_DISABLE },
		{ D3DTSS_TEXCOORDINDEX, 0, 1 }
	};

	// Save state.
	// Note that a 'real' app should never do this.  The effects browser has to contend with all the seperate 
	// plugins which may have their own state management code.  To stop everyone having to coexist with the 
	// same state manager, we do gets and sets.  This means that the EBrowser can't run on the pure device because
	// of the logo.
	m_pD3DDev->GetVertexShader(&dwVertexShader);
	m_pD3DDev->GetPixelShader(&dwPixelShader);
	m_pD3DDev->GetStreamSource(0, &pVB, &VBStride);
	m_pD3DDev->GetTexture(0, &pTexture0);
	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		m_pD3DDev->GetRenderState(SavedRender[i].Type, &SavedRender[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->GetTextureStageState(0, SavedTextureStage0[i].Type, &SavedTextureStage0[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->GetTextureStageState(1, SavedTextureStage1[i].Type, &SavedTextureStage1[i].dwValue);
	}
	
	// Set state
	if (dwVertexShader != D3DFVF_TLVERTEX)
		m_pD3DDev->SetVertexShader(D3DFVF_TLVERTEX);

	if (dwPixelShader != 0)
		m_pD3DDev->SetPixelShader(0);

	if (pVB != m_pLogoVB)
		m_pD3DDev->SetStreamSource(0, m_pLogoVB, sizeof(TLVertex));

	if (pTexture0 != m_pLogoTexture)
		m_pD3DDev->SetTexture(0, m_pLogoTexture);

	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		if (SavedRender[i].dwDesiredValue != SavedRender[i].dwValue)
			m_pD3DDev->SetRenderState(SavedRender[i].Type, SavedRender[i].dwDesiredValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage0[i].dwDesiredValue != SavedTextureStage0[i].dwValue)
			m_pD3DDev->SetTextureStageState(0, SavedTextureStage0[i].Type, SavedTextureStage0[i].dwDesiredValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage1[i].dwDesiredValue != SavedTextureStage1[i].dwValue)
			m_pD3DDev->SetTextureStageState(1, SavedTextureStage1[i].Type, SavedTextureStage1[i].dwDesiredValue);
	}

	// Draw the Logo
	m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );

	// Restore state
	m_pD3DDev->SetVertexShader(dwVertexShader);
	m_pD3DDev->SetStreamSource(0, pVB, VBStride);
	m_pD3DDev->SetTexture(0, pTexture0);
	m_pD3DDev->SetPixelShader(dwPixelShader);

	SAFE_RELEASE(pTexture0);
	SAFE_RELEASE(pVB);

	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		if (SavedRender[i].dwDesiredValue != SavedRender[i].dwValue)
			m_pD3DDev->SetRenderState(SavedRender[i].Type, SavedRender[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage0[i].dwDesiredValue != SavedTextureStage0[i].dwValue)
			m_pD3DDev->SetTextureStageState(0, SavedTextureStage0[i].Type, SavedTextureStage0[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage1[i].dwDesiredValue != SavedTextureStage1[i].dwValue)
			m_pD3DDev->SetTextureStageState(1, SavedTextureStage1[i].Type, SavedTextureStage1[i].dwValue);
	}

	return S_OK;
}

HRESULT CRenderD3DWnd::ResizeLogo()
{
    TLVertex* v;
	float tx, ty, sx, sy, cx, cy;
	UINT winw, winh;

	// Get the logo location
	GetRenderView()->GetSharedLogoPosition(&tx, &ty, &sx, &sy, &cx, &cy, &winw, &winh);

	// Get the window dims
    float wx = (float) winw;
    float wy = (float) winh;

	// Lock the vertex buffer
	HRESULT hr;
	hr = m_pLogoVB->Lock( 0, 0, (BYTE**)&v, 0 );
	if (FAILED(hr))
		return E_FAIL;

	float tc[] = {  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f };
	float vp[] = { -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f };
	
	// Create a quad for the final pass
	for (int i=0; i<4; i++) {

		float fx = (wx-1.0f) * 0.5f * (sx * vp[2*i+0] + cx + 1.0f);
		float fy = (wy-1.0f) * 0.5f * (sy * vp[2*i+1] + cy + 1.0f);

		v[i].Position = D3DXVECTOR4(fx, wy - (fy+1.0f), 0.0f, 1.0f);
		v[i].Diffuse  = 0xffffffff;
		v[i].Specular = 0x00000000;
		v[i].Texture  = D3DXVECTOR2(tx*tc[2*i+0], ty*tc[2*i+1]);
	}
	
	// Release the vertex buffer
	m_pLogoVB->Unlock();

	return S_OK;
}

HRESULT CRenderD3DWnd::SetupTextBuffer()
{
#ifndef DISPLAY_D3D_TEXT
	return S_OK;
#endif

    // Create vertex buffer for the characters
    HRESULT hr = m_pD3DDev->CreateVertexBuffer( MAX_FONT_VERTICES*sizeof(FONT2DVERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                       D3DPOOL_DEFAULT, &m_pFontVB);
	return hr;
}

HRESULT CRenderD3DWnd::FreeTextBuffer()
{
#ifndef DISPLAY_D3D_TEXT
	return S_OK;
#endif

	SAFE_RELEASE(m_pFontVB);
	return S_OK;
}

HRESULT CRenderD3DWnd::SetupTextImage()
{
#ifndef DISPLAY_D3D_TEXT
	return S_OK;
#endif

	HRESULT hr;

    // Establish the font and texture size
    m_fFontTextScale  = 1.0f; // Draw fonts into texture without scaling

    // Large fonts need larger textures
    if( m_dwFontHeight > 40 )
        m_dwFontTexWidth = m_dwFontTexHeight = 1024;
    else if( m_dwFontHeight > 20 )
        m_dwFontTexWidth = m_dwFontTexHeight = 512;
    else
        m_dwFontTexWidth  = m_dwFontTexHeight = 256;

    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS8 d3dCaps;
    m_pD3DDev->GetDeviceCaps( &d3dCaps );

    if( m_dwFontTexWidth > d3dCaps.MaxTextureWidth )
    {
        m_fFontTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)m_dwFontTexWidth;
        m_dwFontTexWidth = m_dwFontTexHeight = d3dCaps.MaxTextureWidth;
    }

    // Create a new texture for the font
    hr = m_pD3DDev->CreateTexture( m_dwFontTexWidth, m_dwFontTexHeight, 1,
                                      0, D3DFMT_A4R4G4B4,
                                      D3DPOOL_MANAGED, &m_pFontTexture );
    if( FAILED(hr) )
        return hr;

    // Prepare to create a bitmap
    DWORD*      pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_dwFontTexWidth;
    bmi.bmiHeader.biHeight      = -(int)m_dwFontTexHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );
    SetMapMode( hDC, MM_TEXT );

    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT nHeight    = -MulDiv( m_dwFontHeight, 
        (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fFontTextScale), 72 );
    DWORD dwBold   = (m_dwFontFlags & D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
    DWORD dwItalic = (m_dwFontFlags & D3DFONT_ITALIC) ? TRUE    : FALSE;
    HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, m_strFontName );
    if( NULL==hFont )
        return E_FAIL;

    SelectObject( hDC, hbmBitmap );
    SelectObject( hDC, hFont );

    // Set text properties
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_TOP );

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD x = 0;
    DWORD y = 0;
    TCHAR str[2] = _T("x");
    SIZE size;

    for( TCHAR c=32; c<127; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32( hDC, str, 1, &size );

        if( (DWORD)(x+size.cx+1) > m_dwFontTexWidth )
        {
            x  = 0;
            y += size.cy+1;
        }

        ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

        m_fFontTexCoords[c-32][0] = ((FLOAT)(x+0))/m_dwFontTexWidth;
        m_fFontTexCoords[c-32][1] = ((FLOAT)(y+0))/m_dwFontTexHeight;
        m_fFontTexCoords[c-32][2] = ((FLOAT)(x+0+size.cx))/m_dwFontTexWidth;
        m_fFontTexCoords[c-32][3] = ((FLOAT)(y+0+size.cy))/m_dwFontTexHeight;

        x += size.cx+1;
    }

    // Lock the surface and write the alpha values for the set pixels
    D3DLOCKED_RECT d3dlr;
    m_pFontTexture->LockRect( 0, &d3dlr, 0, 0 );
    WORD* pDst16 = (WORD*)d3dlr.pBits;
    BYTE bAlpha; // 4-bit measure of pixel intensity

    for( y=0; y < m_dwFontTexHeight; y++ )
    {
        for( x=0; x < m_dwFontTexWidth; x++ )
        {
            bAlpha = (BYTE)((pBitmapBits[m_dwFontTexWidth*y + x] & 0xff) >> 4);
            if (bAlpha > 0)
            {
                *pDst16++ = (bAlpha << 12) | 0x0fff;
            }
            else
            {
                *pDst16++ = 0x0000;
            }
        }
    }

    // Done updating texture, so clean up used objects
    m_pFontTexture->UnlockRect(0);
    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );

    return S_OK;
}

HRESULT CRenderD3DWnd::FreeTextImage()
{
#ifndef DISPLAY_D3D_TEXT
	return S_OK;
#endif

	SAFE_RELEASE(m_pFontTexture);
}

HRESULT CRenderD3DWnd::DrawText()
{
#ifndef DISPLAY_D3D_TEXT
	return S_OK;
#endif

	// Show the framerate
	CNVEffectsDoc* pDoc = GetDocument();

	EBTimer* pTimer = pDoc->GetTimer();
	float fAverageFPS = pTimer->GetAverageFramesPerSec();

	DWORD dwTris, dwVerts;
	BOOL bTrisValid, bVertsValid;

	bTrisValid  = pDoc->GetTriangleCount(dwTris);
	bVertsValid = pDoc->GetVertexCount(dwVerts);

	TCHAR strFrameStats[40];
	_stprintf(strFrameStats, _T("%.02f fps"), fAverageFPS);	
	DrawText(2, 0, D3DCOLOR_ARGB(255,255,255,0), strFrameStats);

	if (bTrisValid)	{
		_stprintf(strFrameStats, _T("%0.2f MTris/sec"), ((fAverageFPS * (float)dwTris) / 1000000.0f));
		DrawText(2, m_dwFontHeight + 8, D3DCOLOR_ARGB(255,255,255,0), strFrameStats);
	}

	if (bVertsValid) {
		_stprintf(strFrameStats, _T("%.02f MVerts/sec"), ((fAverageFPS * (float)dwVerts) / 1000000.0f));
		DrawText(2, (m_dwFontHeight + 8) + (m_dwFontHeight + 8), D3DCOLOR_ARGB(255,255,255,0), strFrameStats);
	}

	return S_OK;
}

HRESULT CRenderD3DWnd::DrawText(FLOAT sx, FLOAT sy, DWORD dwColor, TCHAR* strText, DWORD dwFlags)
{
#ifndef DISPLAY_D3D_TEXT
	return S_OK;
#endif

	if (!m_bValid)
		return NVEFF_ERR_NOTINITIALIZED;

    DWORD dwVertexShader;
	DWORD dwPixelShader;
	UINT VBStride;
	LPDIRECT3DVERTEXBUFFER8 pVB;
	LPDIRECT3DBASETEXTURE8 pTexture0;
	DWORD i;

	typedef struct tagRenderStates
	{
		D3DRENDERSTATETYPE Type;
		DWORD dwValue;
		DWORD dwDesiredValue;
	} tRenderStates;

	typedef struct tagTextureStageStates
	{
		D3DTEXTURESTAGESTATETYPE Type;
		DWORD dwValue;
		DWORD dwDesiredValue;
	} tTextureStates;


	tRenderStates SavedRender[] = 
	{
		{ D3DRS_ZENABLE, 0, FALSE}, 
		{ D3DRS_ALPHATESTENABLE, 0, TRUE },
		{ D3DRS_ALPHAREF, 0, 0x80 },
		{ D3DRS_ALPHAFUNC, 0, D3DCMP_GREATER},
		{ D3DRS_ALPHABLENDENABLE, 0, TRUE},
		{ D3DRS_SRCBLEND, 0, D3DBLEND_SRCALPHA },
		{ D3DRS_DESTBLEND, 0, D3DBLEND_INVSRCALPHA },
		{ D3DRS_FOGENABLE, 0, FALSE },
		{ D3DRS_CULLMODE, 0, D3DCULL_NONE },
		{ D3DRS_FILLMODE, 0, D3DFILL_SOLID},
		{ D3DRS_WRAP0, 0, 0}
	};

	// Set filter states
	D3DTEXTUREFILTERTYPE FilterType;
    if(dwFlags & D3DFONT_FILTERED)
    {
		FilterType = D3DTEXF_LINEAR; 
    }
	else
	{
		FilterType = D3DTEXF_POINT;
	}

	tTextureStates SavedTextureStage0[] = 
	{
		{ D3DTSS_COLOROP, 0, D3DTOP_SELECTARG1 }, 
		{ D3DTSS_COLORARG1, 0, D3DTA_TEXTURE },
		{ D3DTSS_ALPHAOP, 0, D3DTOP_SELECTARG1 },
		{ D3DTSS_ALPHAARG1, 0, D3DTA_TEXTURE },
		{ D3DTSS_TEXCOORDINDEX, 0, 0 },
		{ D3DTSS_MINFILTER, 0, FilterType},
		{ D3DTSS_MAGFILTER, 0, FilterType},
	};

	tTextureStates SavedTextureStage1[] = 
	{
		{ D3DTSS_COLOROP, 0, D3DTOP_DISABLE }, 
		{ D3DTSS_ALPHAOP, 0, D3DTOP_DISABLE },
		{ D3DTSS_TEXCOORDINDEX, 0, 1 }
	};



	// Save state.
	// Note that a 'real' app should never do this.  The effects browser has to contend with all the seperate 
	// plugins which may have their own state management code.  To stop everyone having to coexist with the 
	// same state manager, we do gets and sets.  This means that the EBrowser can't run on the pure device because
	// of the logo.
	m_pD3DDev->GetVertexShader(&dwVertexShader);
	m_pD3DDev->GetPixelShader(&dwPixelShader);
	m_pD3DDev->GetStreamSource(0, &pVB, &VBStride);
	m_pD3DDev->GetTexture(0, &pTexture0);
	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		m_pD3DDev->GetRenderState(SavedRender[i].Type, &SavedRender[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->GetTextureStageState(0, SavedTextureStage0[i].Type, &SavedTextureStage0[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		m_pD3DDev->GetTextureStageState(1, SavedTextureStage1[i].Type, &SavedTextureStage1[i].dwValue);
	}
	
	// Set state
	if (dwVertexShader != D3DFVF_FONT2DVERTEX)
		m_pD3DDev->SetVertexShader(D3DFVF_FONT2DVERTEX);

	if (dwPixelShader != 0)
		m_pD3DDev->SetPixelShader(0);

	if (pVB != m_pFontVB)
		m_pD3DDev->SetStreamSource(0, m_pFontVB, sizeof(FONT2DVERTEX));

	if (pTexture0 != m_pFontTexture)
		m_pD3DDev->SetTexture(0, m_pFontTexture);

	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		if (SavedRender[i].dwDesiredValue != SavedRender[i].dwValue)
			m_pD3DDev->SetRenderState(SavedRender[i].Type, SavedRender[i].dwDesiredValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage0[i].dwDesiredValue != SavedTextureStage0[i].dwValue)
			m_pD3DDev->SetTextureStageState(0, SavedTextureStage0[i].Type, SavedTextureStage0[i].dwDesiredValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage1[i].dwDesiredValue != SavedTextureStage1[i].dwValue)
			m_pD3DDev->SetTextureStageState(1, SavedTextureStage1[i].Type, SavedTextureStage1[i].dwDesiredValue);
	}

    FLOAT fStartX = sx;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices = NULL;
    DWORD         dwNumTriangles = 0;
    m_pFontVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            sx = fStartX;
            sy += (m_fFontTexCoords[0][3]-m_fFontTexCoords[0][1])*m_dwFontTexHeight;
        }
        if( c < _T(' ') )
            continue;

        FLOAT tx1 = m_fFontTexCoords[c-32][0];
        FLOAT ty1 = m_fFontTexCoords[c-32][1];
        FLOAT tx2 = m_fFontTexCoords[c-32][2];
        FLOAT ty2 = m_fFontTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) *  m_dwFontTexWidth / m_fFontTextScale;
        FLOAT h = (ty2-ty1) * m_dwFontTexHeight / m_fFontTextScale;

        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_FONT_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pFontVB->Unlock();
            m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
            pVertices = NULL;
            m_pFontVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pFontVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

	// Restore state
	m_pD3DDev->SetVertexShader(dwVertexShader);
	m_pD3DDev->SetStreamSource(0, pVB, VBStride);
	m_pD3DDev->SetTexture(0, pTexture0);
	m_pD3DDev->SetPixelShader(dwPixelShader);

	SAFE_RELEASE(pTexture0);
	SAFE_RELEASE(pVB);

	for (i = 0; i < sizeof(SavedRender) / sizeof(tRenderStates); i++)
	{
		if (SavedRender[i].dwDesiredValue != SavedRender[i].dwValue)
			m_pD3DDev->SetRenderState(SavedRender[i].Type, SavedRender[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage0) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage0[i].dwDesiredValue != SavedTextureStage0[i].dwValue)
			m_pD3DDev->SetTextureStageState(0, SavedTextureStage0[i].Type, SavedTextureStage0[i].dwValue);
	}
	for (i = 0; i < sizeof(SavedTextureStage1) / sizeof(tTextureStates); i++)
	{
		if (SavedTextureStage1[i].dwDesiredValue != SavedTextureStage1[i].dwValue)
			m_pD3DDev->SetTextureStageState(1, SavedTextureStage1[i].Type, SavedTextureStage1[i].dwValue);
	}


    return S_OK;
}


void CRenderD3DWnd::InterpretError(HRESULT hr)
{
	char Error[256];
	D3DXGetErrorString(hr, Error, 256);

    ostringstream strError;
	strError << "COM Error: 0x" << hex << (DWORD)hr << " : " << Error;

	DISPDBG(0, strError.str().c_str());
}
