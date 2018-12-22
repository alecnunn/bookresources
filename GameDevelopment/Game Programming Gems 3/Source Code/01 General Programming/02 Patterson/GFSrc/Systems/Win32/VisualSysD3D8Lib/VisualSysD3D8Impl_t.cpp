/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysD3D8Impl_t.cpp: implementation of the VisualSysD3D8Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisualSysD3D8Impl_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/Win32/WindowSysWin32Impl_t.h"
#include "GFInclude/Objects/Box2D_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"
#include "GFInclude/Objects/MenuBox2D_t.h"
#include <d3d8types.h>
#include "D3D8Font_t.h"

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VisualSysD3D8Impl_t::VisualSysD3D8Impl_t()
{
	m_pSystems = 0;
	m_lpD3D = NULL;
	m_lpD3DDevice = NULL;
	m_backColor = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	m_pFont = NULL;
}

VisualSysD3D8Impl_t::~VisualSysD3D8Impl_t()
{
	Shutdown();
}

bool VisualSysD3D8Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	m_bWindowed = TRUE;
    m_pFont = new D3D8Font_t( _T("Arial"), 12, D3D8FONT_BOLD );

	// get the window sys
	WindowSys_t *pWS = m_pSystems->GetWindowSys();
	int nWidth;
	int nHeight;
	pWS->GetWindowInfo( nWidth, nHeight );

	if( !m_bWindowed )
	{
		//get rid of the mouse cursor
		ShowCursor(FALSE);
	}

	// we know this is the Win32 implementation
	WindowSysWin32Impl_t *pWin = (WindowSysWin32Impl_t *)pWS;
	HRESULT hr = CreateDevice(pWin->m_hWnd, nWidth, nHeight, 16);
	if(FAILED(hr))
	{
		return false;
	}

    // Initialize device-dependent objects
    if( InitDeviceObjects() )
	{
		// Restore device-dependent objects
        if( RestoreDeviceObjects() )
		{
            return true;
		} else {
			return false;
		}
	}
	return false;
}

void VisualSysD3D8Impl_t::Shutdown()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	if( m_pFont )
	{
		delete m_pFont;
		m_pFont = NULL;
	}

	if(m_lpD3DDevice)
	{
		m_lpD3DDevice->Release();
		m_lpD3DDevice = NULL;
	}

	if(m_lpD3D)
	{
		m_lpD3D->Release();
		m_lpD3D = NULL;
	}

	if( !m_bWindowed )
	{
		//restore cursor
		ShowCursor(TRUE);
	}
}

bool VisualSysD3D8Impl_t::InitDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->InitDeviceObjects( m_lpD3DDevice );
	}
	return true;
}

bool VisualSysD3D8Impl_t::RestoreDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->RestoreDeviceObjects();
	}
	return true;
}

bool VisualSysD3D8Impl_t::InvalidateDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->InvalidateDeviceObjects();
	}
	return true;
}

bool VisualSysD3D8Impl_t::DeleteDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->DeleteDeviceObjects();
	}
	return true;
}

bool VisualSysD3D8Impl_t::BeginRender()
{
	m_lpD3DDevice->Clear(0, 
					   NULL,
					   D3DCLEAR_TARGET |
					   D3DCLEAR_ZBUFFER,
					   m_backColor, //for now clear to blue
					   1.0f,
					   0);

	return( m_lpD3DDevice->BeginScene() == S_OK );
}

void VisualSysD3D8Impl_t::EndRender()
{
	m_pFont->DrawText( 2, 2, D3DCOLOR_ARGB(255,255,255,255), "D3D8 Driver" );

	m_lpD3DDevice->EndScene();
	m_lpD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void VisualSysD3D8Impl_t::WindowMove( int nX, int nY )
{
}

HRESULT VisualSysD3D8Impl_t::CreateDevice(HWND hWnd,
								  DWORD dwWidth,
								  DWORD dwHeight,
								  DWORD dwDepth)
{
	HRESULT hr;
	D3DDISPLAYMODE d3ddm;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_dwDepth = dwDepth;
	m_hWnd = hWnd;

	m_lpD3D = Direct3DCreate8( D3D_SDK_VERSION );
	if(m_lpD3D == NULL){
		return E_FAIL;
	}

	//get adapter settings
	hr = m_lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
										&d3ddm);
	if(FAILED(hr)){
		return E_FAIL;
	}

	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	m_d3dpp.hDeviceWindow = m_hWnd;
	m_d3dpp.BackBufferWidth = m_dwWidth;
	m_d3dpp.BackBufferHeight = m_dwHeight;
	m_d3dpp.BackBufferFormat = d3ddm.Format;
	m_d3dpp.BackBufferCount = 2;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.Windowed = m_bWindowed;

	//create
	hr = m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
							   D3DDEVTYPE_HAL,
							   m_hWnd,
							   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							   &m_d3dpp,
							   &m_lpD3DDevice);
	if(FAILED(hr)){

		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
		hr = m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT,
							   D3DDEVTYPE_HAL,
							   m_hWnd,
							   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							   &m_d3dpp,
							   &m_lpD3DDevice);
		if(FAILED(hr)){
			return E_FAIL;
		}
	}

	return S_OK;
}

void VisualSysD3D8Impl_t::RenderBox( Box2D_t *pBox )
{
	m_pFont->DrawBox( pBox->m_fXPos, pBox->m_fYPos, pBox->m_fXSize, pBox->m_fYSize, pBox->m_nColor );
}

void VisualSysD3D8Impl_t::RenderTextBox( TextBox2D_t *pTextBox )
{
	if( pTextBox->m_bDrawBox )
	{
		m_pFont->DrawBox( pTextBox->m_fXPos, pTextBox->m_fYPos, pTextBox->m_fXSize, pTextBox->m_fYSize, pTextBox->m_nBoxColor );
	}
	if( pTextBox->m_bDrawText )
	{
		m_pFont->DrawText( pTextBox->m_fXPos, pTextBox->m_fYPos, pTextBox->m_nTextColor, pTextBox->m_Text.c_str() );
	}
}

void VisualSysD3D8Impl_t::RenderMenuBox( MenuBox2D_t *pMenuBox )
{
	int nChoice = pMenuBox->m_nChoice;
	int nCount = 0;
	TextBox2D_Vector_t::iterator iTBV;
	for( iTBV = pMenuBox->m_TextBoxVector.begin(); iTBV != pMenuBox->m_TextBoxVector.end(); ++iTBV )
	{
		TextBox2D_t &TB = *iTBV;
		if( TB.m_bDrawBox )
		{
			TB.m_nBoxColor = (nChoice == nCount) ? pMenuBox->m_nBoxHighlightColor : pMenuBox->m_nBoxNormalColor;
			m_pFont->DrawBox( pMenuBox->m_fXPos+TB.m_fXPos, pMenuBox->m_fYPos+TB.m_fYPos, TB.m_fXSize, TB.m_fYSize, TB.m_nBoxColor );
		}
		if( TB.m_bDrawText )
		{
			TB.m_nTextColor = (nChoice == nCount) ? pMenuBox->m_nTextHighlightColor : pMenuBox->m_nTextNormalColor;
			m_pFont->DrawText( pMenuBox->m_fXPos+TB.m_fXPos, pMenuBox->m_fYPos+TB.m_fYPos, TB.m_nTextColor, TB.m_Text.c_str() );
		}
		nCount++;
	}
}

void VisualSysD3D8Impl_t::RenderStats()
{
}

